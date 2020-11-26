#include "wifi_module.h"


static const char *TAG = "wifi softAP";


/*
 * Handles events, occured with WIFI
 */
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *info = event_data;
        char msg[1000];
        bzero(msg, 1000);

        if (wifi_info.wifi_connection_state == DISCONNECTING_WIFI_STATE) {
            uart_print("ESP32 is diconnected from WIFI", 0, 0, RED_TEXT);
        }
        else if (wifi_info.wifi_connection_state == CONNECTING_WIFI_STATE) {
            if (info->reason == 204 || info->reason == 15) {
                sprintf(msg, "Error while connection. Maybe password was wrong");
            }
            else if (info->reason == 201) {
                sprintf(msg, "There is no Access Point with given SSID");
            }
            else {
                sprintf(msg, "ESP32 is not connected to WiFi. Error code: %d", (int)info->reason);
            }
            uart_print(msg, 1, 1, RED_TEXT);
        }
        else if (wifi_info.wifi_connection_state == CONNECTED_WIFI_STATE) {
            // Delay in case any data is printed in UART.
            while (cmd_is_executing == true) {
                vTaskDelay(1);
            }
            sprintf(msg, "ESP32 is disconnected from WiFi. Error code: %d", (int)info->reason);
            uart_print("\r\n", 0, 0, RED_TEXT);
            uart_print(msg, 0, 1, RED_TEXT);
            int n = 0;
            while(n < NUM_OF_WIFI_RECONNECT) {
                uart_print("trying to reconnect...", 0, 1, RED_TEXT);
                esp_wifi_connect();
                vTaskDelay(200);
                n++;
            }
            uart_print("Press enter...", 1, 1, GREEN_TEXT);
        }
        wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
    }
}


static void inline init_wifi_info_struct() {
    // init wifi info stuct.
    wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
    wifi_info.ssid     = NULL;
    wifi_info.password = NULL;
}


void wifi_init_apsta(void) {
    init_wifi_info_struct();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    // wifi_config_t wifi_config = {
    //     .ap = {
    //         .ssid           = AP_SSID,
    //         .ssid_len       = strlen(AP_SSID),
    //         .channel        = WIFI_CHANNEL,
    //         .password       = AP_PASSWORD,
    //         .max_connection = MAX_NUM_OF_CONNECTIONS,
    //         .authmode       = WIFI_AUTH_WPA_WPA2_PSK
    //     },
    // };
    // if (strlen(AP_PASSWORD) == 0) {
    //     wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    // }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    // ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    &event_handler, NULL, &instance_any_id);


    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_GOT_IP,  &WIFIEVENT_sta_got_ip,  NULL, &instance_got_ip);
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_LOST_IP, &WIFIEVENT_sta_lost_ip, NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, &WIFIEVENT_ap_connected, NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &WIFIEVENT_ap_disconnected, NULL, NULL);


    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             AP_SSID, AP_PASSWORD, 1);
}

