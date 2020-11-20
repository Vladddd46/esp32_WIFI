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



void app_main() {
    uart_init(9600);
    nvc_init();
    // init_i2c_driver();   // do not need in this task (needs to communicate with display)
    // init_tz(); // initialize time-zone (takes time zone from nvs) // do not need in this task

    wifi_init_apsta(); // initialize wifi routines.
    http_server_init(); // turn on http server.

    xTaskCreate(user_input,           "user_input",           72040, NULL, 10, NULL);
    // xTaskCreate(time_synchronization, "time_synchronization", 12040, NULL, 10, NULL); // do not need in this task
    // xTaskCreate(dht11_monitor,        "dht11_monitor",        52040, NULL, 10, NULL); // do not need in this task
    // xTaskCreate(timer_task,           "timer",                12040, NULL, 10, &xTaskClock); // do not need in this task
}

