#include "wifi_module.h"

static const char *TAG = "wifi softAP"; // to del


void WIFIEVENT_sta_connected_to_ap(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t*) event_data;
        if (event != NULL) {
            ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                     MAC2STR(event->mac), event->aid);
        }
    }
}



void WIFIEVENT_sta_disconnected_from_ap(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    printf("1 sta disconnected\n");
}

// WIFI_EVENT_STA_DISCONNECTED
void WIFIEVENT_sta_disconnected_from_wifi(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
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

// IP_EVENT_STA_GOT_IP
void WIFIEVENT_sta_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        uart_print("\n\rSuccess! Got IP: ", 0, 0, GREEN_TEXT);
        char got_ip[50];
        bzero(got_ip, 50);
        sprintf(got_ip, "%s", ip4addr_ntoa(&event->ip_info.ip));
        uart_print(got_ip, 0, 1, NULL);
        print_connection_log();
        wifi_info.wifi_connection_state = CONNECTED_WIFI_STATE;
}


// IP_EVENT_STA_LOST_IP
void WIFIEVENT_sta_lost_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
        uart_print("Lost IP", 0, 1, RED_TEXT);
        wifi_info.wifi_connection_state = DISCONNECTING_WIFI_STATE;
        esp_wifi_disconnect();
        vTaskDelay(10);
        wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
}

