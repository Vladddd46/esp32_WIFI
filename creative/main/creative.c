#include "header.h"

#define UART_TX_PIN 17
#define UART_RX_PIN 16

#define GPIO_INPUT_IO_1     39

static void inline nvc_init(bool erase_flash) {
    if (erase_flash) {
        nvs_flash_erase();
    }

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



static bool check_if_button1_is_pressed_during_loading() {
    gpio_set_direction_wrapper(GPIO_INPUT_IO_1, GPIO_MODE_INPUT);
    int counter = 0;
    int wait = 0;
    while(wait < 10) {
        if (gpio_get_level(GPIO_INPUT_IO_1) != 1) {
            counter++;
        }
        wait++;
        vTaskDelay(10);
    }
    if (counter == 10) {
        return true;
    }
    return false;
}



void app_main() {
    bool btn1_is_pressed = check_if_button1_is_pressed_during_loading();

    uart_init(9600);
    nvc_init(btn1_is_pressed);

    char *default_ssid = get_value_by_key_from_nvs(WIFI_STORAGE, DEFAULT_SSID);
    char *default_pass = get_value_by_key_from_nvs(WIFI_STORAGE, DEFAULT_PASS);
    if (default_ssid != NULL && default_pass != NULL) {
        wifi_initialization_in_sta_mode();
        connect_to_wifi(default_ssid, default_pass);
        free(default_ssid);
        free(default_pass);
    }
    else {
        wifi_init_apsta();  // initialize wifi routines.
        http_server_init(); // turn on http server.
    }
    xTaskCreate(user_input, "user_input", 72040, NULL, 10, NULL);
}

