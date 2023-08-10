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

// #include "freertos/queue.h"
// #include "rom/gpio.h"
// #include "driver/gpio.h"
// #include "esp_timer.h"

static const char *TAG = "BTC_PRICE";

char dispTxt[20];


static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
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

double extractBTCPrice(char *jsonMsg){
    cJSON *root = cJSON_Parse(jsonMsg);
    double value = 0.0;
    if (root == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    }
    cJSON *last_trade_price = cJSON_GetObjectItemCaseSensitive(root, "last_trade_price");
    if (cJSON_IsNumber(last_trade_price)) {
        value = last_trade_price->valuedouble;
        printf("The value of last_trade_price is %f\n", value);
    } else {
        printf("Error getting last_trade_price\n");
    }
    cJSON_Delete(root);
    return value;
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        double btcVal = extractBTCPrice((char *)evt->data);
        memset(dispTxt, 0, sizeof(dispTxt));
        sprintf(dispTxt, "$%.0f", btcVal);
        break;

    default:
        break;
    }
    return ESP_OK;
}

void btc_price_task(void *pvParameters) {
    esp_http_client_config_t config = {
        .url = "https://api.blockchain.com/v3/exchange/tickers/BTC-USD",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler
    };
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
        lv_label_set_text(ui_Label2, dispTxt);//lv_label_set_text(label1, myStr);
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
    xTaskCreate(&btc_price_task, "btc_price_task", 4096, NULL, 5, NULL);
    xTaskCreatePinnedToCore(lvgl_task, "LCD", 8 * 1024, NULL, 5, NULL, 1);
}
