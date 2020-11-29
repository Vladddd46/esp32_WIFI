#include "header.h"

#define UART_TX_PIN 17
#define UART_RX_PIN 16

#define GPIO_INPUT_SW1     39



// Initialize non-volatile storage.
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
    gpio_set_direction_wrapper(GPIO_INPUT_SW1, GPIO_MODE_INPUT);
    int counter = 0;
    int wait    = 0;
    while(wait < 10) {
        if (gpio_get_level(GPIO_INPUT_SW1) != 1) {
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
    /* if button1 is pressed -> device should
     * be reset to default(factory) settings.
     */
    bool btn1_is_pressed = check_if_button1_is_pressed_during_loading();
    nvc_init(btn1_is_pressed);

    uart_init(9600);

    // Get default wifi ssid and pass from nvs.
    char *default_ssid = get_value_by_key_from_nvs(WIFI_STORAGE, DEFAULT_SSID);
    char *default_pass = get_value_by_key_from_nvs(WIFI_STORAGE, DEFAULT_PASS);

    /* If there is default ssid and pass in nvc,
     *  initialize device in station(sta) mode and 
     *  try to connect to wifi with default_ssid and default_pass.
     * Otherwise, initialize device in apsta mode and run http_server.
     * User, with help of web-browser, can get webpage with list of all available
     *  wifi networks, select one, enter password and send this data to http server,
     *  which is running on device. When http server gets post request(with ssid and pass), 
     *  it tries to connect to wifi with ssid and pass. If connection to wifi network is 
     *  success, ssid and pass saves in nvs and device is being restarted in order to initialize sta
     *  mode and connect to saved in nvs wifi network. Also, http_server sends web-response about 
     * connection status ("OK. Device is connected" OR "Failed. Device is not connected. Try again.")
     */
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
    
    // Task, which implements CLI.
    xTaskCreate(user_input, "user_input", 72040, NULL, 10, NULL);
}

