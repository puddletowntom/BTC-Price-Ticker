#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "nvs_flash.h"
#include "my_data.h"
#include "ui/ui.h"
#include "lvgl.h"
#include "board.h"
#include "cJSON.h"
#include "esp_timer.h"
#include "esp_wps.h"
#include "esp_event.h"
#include <string.h>
#include "math.h"
//#define GET_JSON_ITEM(root, key) cJSON_GetObjectItemCaseSensitive(root, key)

// #include "freertos/queue.h"
// #include "rom/gpio.h"
// #include "driver/gpio.h"
// #include "esp_timer.h"

/*set wps mode via project configuration */
#if CONFIG_EXAMPLE_WPS_TYPE_PBC
#define WPS_MODE WPS_TYPE_PBC
#elif CONFIG_EXAMPLE_WPS_TYPE_PIN
#define WPS_MODE WPS_TYPE_PIN
#else
#define WPS_MODE WPS_TYPE_DISABLE
#endif /*CONFIG_EXAMPLE_WPS_TYPE_PBC*/

#define MAX_RETRY_ATTEMPTS     2

#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

static esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_MODE);
static wifi_config_t wps_ap_creds[MAX_WPS_AP_CRED];
static int s_ap_creds_num = 0;
static int s_retry_num = 0;
static const char *TAG = "BTC_PRICE";

char dispTxt[20];

typedef struct{
    char price[20];
    char hash[50];
    char fees[20];
    char supply[20];
    char volume[50];
    char blockCount[20];
    char blockInterval[20];
    char blockSize[20];
    char blockRemain[20];
}BitcoinStats;

BitcoinStats bitcoinStats;
char mcapResult[20];
char btcPerBlock[20];

bool toggle = false;

typedef struct{
    char *stats;
    char *bcperblock;
    char *avgtxnumber;
    char *marketcap;
    char *hrbtcsent;
    char *transactionCount;
    char *btcTicker;
    char statsKeys[8][50];
}BlockchainAPI;

BlockchainAPI blockchainAPI = {
    .stats = "https://api.blockchain.info/stats",
    .bcperblock = "https://blockchain.info/q/bcperblock",
    .avgtxnumber = "https://blockchain.info/q/avgtxnumber",
    .marketcap = "https://blockchain.info/q/marketcap",
    .hrbtcsent = "https://blockchain.info/q/24hrbtcsent",
    .transactionCount = "https://blockchain.info/q/24hrtransactioncount",
    .btcTicker = "https://api.blockchain.com/v3/exchange/tickers/BTC-USD",
    {"market_price_usd", "hash_rate", "total_fees_btc", "totalbc", "trade_volume_usd", "n_blocks_total", "minutes_between_blocks", "blocks_size"},
};

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static int ap_idx = 1;

    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WIFI_CONNECTED");
            //lv_obj_clear_flag(ui_Image11, LV_OBJ_FLAG_HIDDEN); //Show the Wi-Fi logo
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            //lv_obj_add_flag(ui_Image11, LV_OBJ_FLAG_HIDDEN);  //Hide the Wi-Fi logo
            if (s_retry_num < MAX_RETRY_ATTEMPTS) {
                esp_wifi_connect();
                s_retry_num++;
            } else if (ap_idx < s_ap_creds_num) {
                /* Try the next AP credential if first one fails */

                if (ap_idx < s_ap_creds_num) {
                    ESP_LOGI(TAG, "Connecting to SSID: %s, Passphrase: %s",
                             wps_ap_creds[ap_idx].sta.ssid, wps_ap_creds[ap_idx].sta.password);
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wps_ap_creds[ap_idx++]) );
                    esp_wifi_connect();
                }
                s_retry_num = 0;
            } else {
                ESP_LOGI(TAG, "Failed to connect!");
            }

            break;
        case WIFI_EVENT_STA_WPS_ER_SUCCESS:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_SUCCESS");
            {
                wifi_event_sta_wps_er_success_t *evt =
                    (wifi_event_sta_wps_er_success_t *)event_data;
                int i;

                if (evt) {
                    ESP_LOGI("Ronan: ","Got here");
                    s_ap_creds_num = evt->ap_cred_cnt;
                    for (i = 0; i < s_ap_creds_num; i++) {
                        memcpy(wps_ap_creds[i].sta.ssid, evt->ap_cred[i].ssid,
                               sizeof(evt->ap_cred[i].ssid));
                        memcpy(wps_ap_creds[i].sta.password, evt->ap_cred[i].passphrase,
                               sizeof(evt->ap_cred[i].passphrase));
                    }
                    /* If multiple AP credentials are received from WPS, connect with first one */
                    ESP_LOGI(TAG, "Connecting to SSID: %s, Passphrase: %s",
                             wps_ap_creds[0].sta.ssid, wps_ap_creds[0].sta.password);
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wps_ap_creds[0]) );
                }
                /*
                 * If only one AP credential is received from WPS, there will be no event data and
                 * esp_wifi_set_config() is already called by WPS modules for backward compatibility
                 * with legacy apps. So directly attempt connection here.
                 */
                ESP_ERROR_CHECK(esp_wifi_wps_disable());
                esp_wifi_connect();
            }
            break;
        case WIFI_EVENT_STA_WPS_ER_FAILED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_FAILED");
            ESP_ERROR_CHECK(esp_wifi_wps_disable());
            ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
            ESP_ERROR_CHECK(esp_wifi_wps_start(0));
            break;
        case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_TIMEOUT");
            ESP_ERROR_CHECK(esp_wifi_wps_disable());
            ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
            ESP_ERROR_CHECK(esp_wifi_wps_start(0));
            break;
        case WIFI_EVENT_STA_WPS_ER_PIN:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_PIN");
            /* display the PIN code */
            wifi_event_sta_wps_er_pin_t* event = (wifi_event_sta_wps_er_pin_t*) event_data;
            ESP_LOGI(TAG, "WPS_PIN = " PINSTR, PIN2STR(event->pin_code));
            break;
        default:
            break;
    }
}

// void wifi_connection()
// {
//     // 1 - Wi-Fi/LwIP Init Phase
//     esp_netif_init();                    // TCP/IP initiation 					s1.1
//     esp_event_loop_create_default();     // event loop 			                s1.2
//     esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
//     wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
//     esp_wifi_init(&wifi_initiation); // 					                    s1.4
//     // 2 - Wi-Fi Configuration Phase
//     esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
//     esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
//     // wifi_config_t wifi_configuration = {
//     //     .sta = {
//     //         .ssid = SSID,
//     //         .password = PASS}};

//     wifi_config_t config;
//     esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &config); 
//     if (err == ESP_OK) {
//         ESP_LOGI("RONAN: ", "SSID: %s, PW: %s\n", (char*) config.sta.ssid, (char*) config.sta.password);
//     } else {
//         ESP_LOGI("RONAN: ", "Couldn't get config: %d\n", (int) err);
//         //start_wps(); //If you cant get the SSID/Password then run WPS.
//     }

//     esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
//     // 3 - Wi-Fi Start Phase
//     esp_wifi_start();
//     // 4- Wi-Fi Connect Phase
//     esp_wifi_connect();
// }

static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
}

/*init wifi as sta and start wps*/
static void start_wps(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &got_ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config;
    esp_err_t err = esp_wifi_get_config(WIFI_IF_STA, &wifi_config); 
    if (err == ESP_OK) {
        ESP_LOGI("RONAN: ", "SSID: %s, PW: %s\n", (char*) wifi_config.sta.ssid, (char*) wifi_config.sta.password);
        esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
        esp_err_t errStart = esp_wifi_start();
        esp_err_t errConnect = esp_wifi_connect(); 

        if(errStart != ESP_OK || errConnect != ESP_OK){
            ESP_LOGI(TAG, "start wps...");
            ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
            ESP_ERROR_CHECK(esp_wifi_wps_start(0)); 
        }
    } else {
        ESP_LOGI("RONAN: ", "Couldn't get config: %d\n", (int) err);
        ESP_LOGI(TAG, "start wps...");
        ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
        ESP_ERROR_CHECK(esp_wifi_wps_start(0)); 
    }
}

double extractJsonVal(char *jsonMsg, char *jsonKey){
    cJSON *root = cJSON_Parse(jsonMsg);
    double value = 0.0;
    if (root == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    }
    cJSON *jsonValue = cJSON_GetObjectItemCaseSensitive(root, jsonKey);
    if (cJSON_IsNumber(jsonValue)) {
        value = jsonValue->valuedouble;
        printf("The value is %f\n", value);
    } else {
        printf("Error getting message\n");
    }
    cJSON_Delete(root);
    return value;
}

char* format_number(double number) {
    char buffer[30];
    char tmpBuf[30]; //tmpBuf is just for a string length.
    char suffix[2];

    snprintf(tmpBuf, sizeof(tmpBuf), "%.0f", number);
    int length = strlen(tmpBuf);
    
    if (length >= 7 && length < 10) {
        strcpy(suffix, "m");
        number /= 1000000;
    } else if (length >= 10 && length < 13) {
        strcpy(suffix, "b");
        number /= 1000000000;
    } else if (length >= 13) {
        strcpy(suffix, "t");
        number /= 1000000000000;
    } else {
        suffix[0] = '\0';
    }
    snprintf(buffer, sizeof(buffer), "%.0f", number);

    char* result = malloc(strlen(buffer) + strlen(suffix) + 1);
    strcpy(result, buffer);
    strcat(result, suffix);
    
    return result;
}

void formatWithCommas(char *output, size_t size) {
    // Insert commas as thousands separators
    for (int i = strlen(output) - 3; i > 0; i -= 3) {
        memmove(output + i + 1, output + i, strlen(output) - i + 1);
        output[i] = ',';
    }
}

void hashSuffix(double value){
    double giga = pow(10, 9);
    double tera = pow(10, 12);
    double peta = pow(10, 15);
    double exa = pow(10, 18);
    double zetta = pow(10, 21);
    double yotta = pow(10, 24);
    double ronna = pow(10, 27);
    double quetta = pow(10, 30);
    char suffixVal[15];

    value *= giga; //convert to giga hash

    if(value > giga && value < tera){
        value /= giga;
        strcpy(suffixVal, " Giga/h");
    }else if(value > tera && value < peta){
        value /= tera;
        strcpy(suffixVal, " Tera/h");
    }else if(value > peta && value < exa){
        value /= peta;
        strcpy(suffixVal, " Peta/h");
    }else if(value > exa && value < zetta){
        value /= exa;
        strcpy(suffixVal, " Exa/h");
    }else if(value > zetta && value < yotta){
        value /= zetta;
        strcpy(suffixVal, " Zetta/h");
    }else if(value > yotta && value < ronna){
        value /= yotta;
        strcpy(suffixVal, " Yotta/h");
    }else if(value > ronna && value < quetta){
        value /= ronna;
        strcpy(suffixVal, " Ronna/h");
    }else if(value > quetta){
        value /= quetta;
        strcpy(suffixVal, " Quetta/h");
    }

    snprintf(bitcoinStats.hash, sizeof(bitcoinStats.hash), "%.0f", value);
    strcat(bitcoinStats.hash, suffixVal);
    ESP_LOGI("HERE..", "%s", bitcoinStats.hash);
}

#define MAX_RESPONSE_SIZE 4096

// Buffer to accumulate received data
char responseBuffer[MAX_RESPONSE_SIZE];
size_t responseLength = 0;

esp_err_t clientStatsHandler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            // Copy received data to the buffer
            if (responseLength + evt->data_len <= MAX_RESPONSE_SIZE) {
                memcpy(responseBuffer + responseLength, evt->data, evt->data_len);
                responseLength += evt->data_len;
            } else {
                printf("Received data exceeds buffer size\n");
            }
            break;

        // Handle other HTTP events here

        case HTTP_EVENT_ON_FINISH:
            // Process the complete accumulated data
            responseBuffer[responseLength] = '\0'; // Null-terminate the data
            char *current_message = strtok(responseBuffer, "}");
            strcat(current_message, "}");
            printf("Received data: %s\n", current_message);
            double price = extractJsonVal(current_message, blockchainAPI.statsKeys[0]);
            ESP_LOGI("Price", "\n%f", price);
            double hashrate = extractJsonVal(current_message, blockchainAPI.statsKeys[1]);
            ESP_LOGI("Hashrate", "\n%f", hashrate);
            double fees = extractJsonVal(current_message, blockchainAPI.statsKeys[2]);
            if(fees < 0){
                fees = 0;
            }
            ESP_LOGI("Fees", "\n%f", fees);
            double supplyDouble = extractJsonVal(current_message, blockchainAPI.statsKeys[3]);
            int supply = (int)(supplyDouble / 100000000);
            ESP_LOGI("Supply", "\n%d", supply);
            double volume = extractJsonVal(current_message, blockchainAPI.statsKeys[4]);
            ESP_LOGI("Volume", "\n%f", volume);
            double blockCount = extractJsonVal(current_message, blockchainAPI.statsKeys[5]);
            ESP_LOGI("BlockCount", "\n%f", blockCount);
            double blockInterval = extractJsonVal(current_message, blockchainAPI.statsKeys[6]);
            ESP_LOGI("BlockInterval", "\n%f", blockInterval);
            double blockSize = extractJsonVal(current_message, blockchainAPI.statsKeys[7]);
            ESP_LOGI("BlockSize", "\n%f", blockSize);

            memset(bitcoinStats.price, 0, sizeof(bitcoinStats.price));
            memset(bitcoinStats.hash, 0, sizeof(bitcoinStats.hash));
            memset(bitcoinStats.fees, 0, sizeof(bitcoinStats.fees));
            memset(bitcoinStats.supply, 0, sizeof(bitcoinStats.supply));
            memset(bitcoinStats.volume, 0, sizeof(bitcoinStats.volume));
            memset(bitcoinStats.blockCount, 0, sizeof(bitcoinStats.blockCount));
            memset(bitcoinStats.blockInterval, 0, sizeof(bitcoinStats.blockInterval));
            memset(bitcoinStats.blockSize, 0, sizeof(bitcoinStats.blockSize));
            memset(bitcoinStats.blockRemain, 0, sizeof(bitcoinStats.blockRemain));

            snprintf(bitcoinStats.price, sizeof(bitcoinStats.price), "$%.0f", price);
            formatWithCommas(bitcoinStats.price, sizeof(bitcoinStats.price));
            hashSuffix(hashrate);
            //snprintf(bitcoinStats.hash, sizeof(bitcoinStats.hash), "%.0f", hashrate);
            snprintf(bitcoinStats.fees, sizeof(bitcoinStats.fees), "$%.0f", fees);

            snprintf(bitcoinStats.supply, sizeof(bitcoinStats.supply), "%d", supply);
            formatWithCommas(bitcoinStats.supply, sizeof(bitcoinStats.supply));

            snprintf(bitcoinStats.volume, sizeof(bitcoinStats.volume), "$%.0f", volume);

            snprintf(bitcoinStats.blockCount, sizeof(bitcoinStats.blockCount), "%.0f", blockCount);
            formatWithCommas(bitcoinStats.blockCount, sizeof(bitcoinStats.blockCount));

            snprintf(bitcoinStats.blockInterval, sizeof(bitcoinStats.blockInterval), "%.1f mins", blockInterval);
            snprintf(bitcoinStats.blockSize, sizeof(bitcoinStats.blockSize), "%.0f", blockSize);

            lv_label_set_text(ui_Label2, bitcoinStats.price);
            lv_label_set_text(ui_Label7, bitcoinStats.supply);
            lv_label_set_text(ui_Label9, bitcoinStats.hash);
            lv_label_set_text(ui_Label13, bitcoinStats.blockInterval);
            lv_label_set_text(ui_Label15, bitcoinStats.blockCount);
            int currentCycle = blockCount/210000;
            double blockRemain = (210000*(currentCycle+1)) - blockCount;

            snprintf(bitcoinStats.blockRemain, sizeof(bitcoinStats.blockRemain), "%.0f", blockRemain);
            formatWithCommas(bitcoinStats.blockRemain, sizeof(bitcoinStats.blockRemain));
            
            lv_label_set_text(ui_Label17, bitcoinStats.blockRemain);

           // memset(dispTxt, 0, sizeof(dispTxt));
           // sprintf(dispTxt, "$%.0f", price);
            // Process the JSON data or perform other operations here
            break;

        default:
            break;
    }
    return ESP_OK;
}

esp_err_t perBlockHandler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("perBlockHandler: %.*s\n", evt->data_len, (char *)evt->data);
        memset(btcPerBlock, 0, sizeof(btcPerBlock));
        snprintf(btcPerBlock, sizeof(btcPerBlock), "%.*s btc", evt->data_len, (char *)evt->data);
        lv_label_set_text(ui_Label11, btcPerBlock);
        break;

    default:
        break;
    }
    return ESP_OK;
}

esp_err_t mcapHandler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("mcapHandler: %.*s\n", evt->data_len, (char *)evt->data);
        memset(mcapResult, 0, sizeof(mcapResult));
        double decimal_number = strtod((char *)evt->data, NULL);
        char* formatted_number = format_number(decimal_number);
        snprintf(mcapResult, sizeof(mcapResult), "$%.*s", strlen(formatted_number), formatted_number);
        //snprintf(mcapResult, sizeof(mcapResult), "$%.2f", formatted_number);
        free(formatted_number);
       // snprintf(mcapResult, sizeof(mcapResult), "%.*s", evt->data_len, (char *)evt->data);
        lv_label_set_text(ui_Label4, mcapResult);
        break;

    default:
        break;
    }
    return ESP_OK;
}


// http_header_t headers[] = {
//     { "Accepts", "application/json" },
//     { "X-CMC_PRO_API_KEY", "91c1467c-1de3-4cf1-874f-bd82e4685fa1" },
//     { NULL, NULL }
// };

// void btc_api_task(void *pvParameters) {

//     TaskParams *params = (TaskParams*)pvParameters;

//     esp_http_client_config_t config = {
//         .url = params->api_url, //"https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?symbol=BTC&convert=USD" 
//         .method = HTTP_METHOD_GET,
//         .cert_pem = NULL,
//         //.event_handler = clientStatsHandler,
//         //.user_data = blockchainAPI.statsKeys[0]
//     };

//     if(params->statsFlag){
//         config.event_handler = clientStatsHandler;
//     }else{
//         config.event_handler = clientSingleHandler;
//     }
    
//     esp_http_client_handle_t client = esp_http_client_init(&config);
    
//     while (1) {
//         esp_err_t err = esp_http_client_perform(client);
        
//         if (err == ESP_OK) {
//             ESP_LOGE(TAG, "HTTP Success");
//         } else {
//             ESP_LOGE(TAG, "HTTP Request Failed");
//         }
        
//         vTaskDelay(3000 / portTICK_PERIOD_MS); // Fetch every 10 seconds
//     }
    
//     esp_http_client_cleanup(client);
// }

extern void screen_init(void);

void lvgl_task(void *pvParameters) {
    // LVGL initialization, UI setup, and event handling
    // This task should regularly call lv_task_handler() to update the UI.
    while (1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust the delay as needed
    }
}

void app_main() {
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_LOGI("pp", "Got passed here");
    //wifi_connection();
    start_wps();
    ESP_LOGI("..", "Done with WPS");
    screen_init();
    ui_init();
    xTaskCreatePinnedToCore(lvgl_task, "LCD", 8 * 1024, NULL, 3, NULL, 1);

    while(1){
        esp_http_client_config_t config = {
            //.url = blockchainAPI.stats, //"https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?symbol=BTC&convert=USD" 
            .method = HTTP_METHOD_GET,
            .cert_pem = NULL,
            //.event_handler = clientStatsHandler,
            //.user_data = blockchainAPI.statsKeys[0]
        };
        static int URLSwitch = 1;
        switch(URLSwitch){
            case 1:
                config.url = blockchainAPI.stats;
                config.event_handler = clientStatsHandler;
            break;
            case 2:
                config.url = blockchainAPI.bcperblock;
                config.event_handler = perBlockHandler;
            break;
            case 3:
                config.url = blockchainAPI.marketcap;
                config.event_handler = mcapHandler;
            break;
            default:
                config.url = blockchainAPI.stats;
                config.event_handler = clientStatsHandler;
            break;
        }
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_perform(client);
        esp_http_client_cleanup(client);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        URLSwitch++;
        if(URLSwitch == 4){
            URLSwitch = 1;
        }
    }
}
