#include "wifi_module.h"


/* @ IP_EVENT_STA_GOT_IP
 * Event occurs, when esp32 connect to wifi and get ip address.
 */
void WIFIEVENT_sta_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    uart_print("\n\rSuccess! Got IP: ", 0, 0, GREEN_TEXT);
    char got_ip[50];
    bzero(got_ip, 50);
    if (event != NULL) {
    	sprintf(got_ip, "%s", ip4addr_ntoa(&event->ip_info.ip));
    }
    uart_print(got_ip, 0, 1, NULL);
    print_connection_log();
    wifi_info.wifi_connection_state = CONNECTED_WIFI_STATE;
}

/* @ IP_EVENT_STA_LOST_IP
 * Event occurs, when esp32 sta lose ip address.
 */
void WIFIEVENT_sta_lost_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    uart_print("Lost IP", 0, 1, RED_TEXT);
    wifi_info.wifi_connection_state = DISCONNECTING_WIFI_STATE;
    esp_wifi_disconnect();
    vTaskDelay(10);
    wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
}

/* @ WIFI_EVENT_AP_STACONNECTED
 * Occures, when sta connects to AP on esp32.
 */
void WIFIEVENT_ap_connected(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
	wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    if (event != NULL) {
        ESP_LOGI("wifi softAP", "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

/* @ WIFI_EVENT_AP_STADISCONNECTED
 * Occures, when sta disconnects from AP on esp32.
 */
void WIFIEVENT_ap_disconnected(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
	printf("sta disconnected\n");
}






