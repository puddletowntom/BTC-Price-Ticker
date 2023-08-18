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
//#define GET_JSON_ITEM(root, key) cJSON_GetObjectItemCaseSensitive(root, key)

// #include "freertos/queue.h"
// #include "rom/gpio.h"
// #include "driver/gpio.h"
// #include "esp_timer.h"

static const char *TAG = "BTC_PRICE";

char dispTxt[20];
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

typedef struct{
    char *api_url;
    bool statsFlag;
}TaskParams;

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

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        lv_obj_clear_flag(ui_Image11, LV_OBJ_FLAG_HIDDEN); //Show the Wi-Fi logo
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        lv_obj_add_flag(ui_Image11, LV_OBJ_FLAG_HIDDEN);  //Hide the Wi-Fi logo
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
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
            ESP_LOGI("Fees", "\n%f", fees);
            double supply = extractJsonVal(current_message, blockchainAPI.statsKeys[3]);
            ESP_LOGI("Supply", "\n%f", supply);
            double volume = extractJsonVal(current_message, blockchainAPI.statsKeys[4]);
            ESP_LOGI("Volume", "\n%f", volume);
            double blockCount = extractJsonVal(current_message, blockchainAPI.statsKeys[5]);
            ESP_LOGI("BlockCount", "\n%f", blockCount);
            double blockInterval = extractJsonVal(current_message, blockchainAPI.statsKeys[6]);
            ESP_LOGI("BlockInterval", "\n%f", blockInterval);
            double blockSize = extractJsonVal(current_message, blockchainAPI.statsKeys[7]);
            ESP_LOGI("BlockSize", "\n%f", blockSize);
            // Process the JSON data or perform other operations here
            break;

        default:
            break;
    }
    return ESP_OK;
}

esp_err_t clientSingleHandler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        memset(dispTxt, 0, sizeof(dispTxt));
        sprintf(dispTxt, "%s", (char *)evt->data);
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

void btc_api_task(void *pvParameters) {

    TaskParams *params = (TaskParams*)pvParameters;

    esp_http_client_config_t config = {
        .url = params->api_url, //"https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?symbol=BTC&convert=USD" 
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        //.event_handler = clientStatsHandler,
        //.user_data = blockchainAPI.statsKeys[0]
    };

    if(params->statsFlag){
        config.event_handler = clientStatsHandler;
    }else{
        config.event_handler = clientSingleHandler;
    }
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    while (1) {
        esp_err_t err = esp_http_client_perform(client);
        
        if (err == ESP_OK) {
            ESP_LOGE(TAG, "HTTP Success");
        } else {
            ESP_LOGE(TAG, "HTTP Request Failed");
        }
        
        vTaskDelay(3000 / portTICK_PERIOD_MS); // Fetch every 10 seconds
    }
    
    esp_http_client_cleanup(client);
}

void lvgl_task(void* arg) {
    for (;;) {
        lv_label_set_text(ui_Label1, dispTxt);//lv_label_set_text(label1, myStr);
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
extern void screen_init(void);

void app_main() {
    nvs_flash_init();
    wifi_connection();
    screen_init();
    ui_init();
    
    TaskParams *params = (TaskParams *)malloc(sizeof(TaskParams));
    if (params == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    params->api_url = blockchainAPI.stats;
    params->statsFlag = true;

    // TaskParams *params = (TaskParams *)malloc(sizeof(TaskParams));
    // if (params == NULL) {
    //     printf("Memory allocation failed.\n");
    //     return;
    // }
    // params->api_url = blockchainAPI.bcperblock;
    // params->statsFlag = false;

    xTaskCreate(&btc_api_task, "btc_stats_task", 2*4096, params, 5, NULL);
   // xTaskCreatePinnedToCore(lvgl_task, "LCD", 8 * 1024, NULL, 5, NULL, 1);
}
