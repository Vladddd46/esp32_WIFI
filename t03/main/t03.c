#include "header.h"

#define UART_TX_PIN 17
#define UART_RX_PIN 16



static void inline nvc_init() {
    esp_err_t err;
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NOT_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
}



static void inline uart_init(int baud_rate) {
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_PORT, 2048, 2048, 20, &uart0_queue, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_pattern_queue_reset(UART_PORT, 20);
}



// Initialize wifi routines.
static void inline wifi_initialization() {
    // init wifi info stuct.
    wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
    wifi_info.ssid     = NULL;
    wifi_info.password = NULL;

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_start();

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    &event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);
}



void app_main() {
    uart_init(9600);
    nvc_init();
    wifi_initialization();
    wifi_auto_connect();
    xTaskCreate(user_input,    "user_input",    52040, NULL, 10, NULL);
    xTaskCreate(dht11_monitor, "dht11_monitor", 12040, NULL, 10, NULL);
}

