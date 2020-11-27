#include "wifi_module.h"



static void inline init_wifi_info_struct() {
    // init wifi info stuct.
    wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
    wifi_info.ssid     = NULL;
    wifi_info.password = NULL;
}

/* @ AP config
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
    // ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
 */

// Initialize wifi routines in sta mode.

void wifi_initialization_in_sta_mode() {
    init_wifi_info_struct();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_start();

    esp_event_handler_instance_t instance_sta_disconnected;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_GOT_IP,           &WIFIEVENT_sta_got_ip,      NULL, &instance_got_ip);
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_LOST_IP,          &WIFIEVENT_sta_lost_ip,     NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,   &WIFIEVENT_sta_disconnected,NULL, &instance_sta_disconnected);
}



void wifi_init_apsta(void) {
    init_wifi_info_struct();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_event_handler_instance_t instance_sta_disconnected;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_GOT_IP,           &WIFIEVENT_sta_got_ip,      NULL, &instance_got_ip);
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_LOST_IP,          &WIFIEVENT_sta_lost_ip,     NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED,    &WIFIEVENT_ap_connected,    NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &WIFIEVENT_ap_disconnected, NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,   &WIFIEVENT_sta_disconnected,NULL, &instance_sta_disconnected);

    ESP_LOGI("wifi softAP", "wifi_init_softap finished. SSID:%s password:%s channel:%d", AP_SSID, AP_PASSWORD, 1);
}

