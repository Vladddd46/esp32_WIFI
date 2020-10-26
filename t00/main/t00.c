#include "header.h"

#define UART_TX_PIN 17
#define UART_RX_PIN 16

/* NVC guide.
 * 1. nvs_flash_init();
 * 2. nvs_flash_erase(); - delete all data from nvs.
 * 3.  nvs_handle_t my_handle;
 * 3.1 nvs_open("name_space_name", NVS_READWRITE, &my_handle);
 * 4. nvs_set_str(my_handle, "key", "valueX"); -set key:value data
 * 5. 
 *     size_t required_size;
 *     nvs_get_str(my_handle, "key", NULL, &required_size);
 *     char* buff = malloc(required_size);
 *	   nvs_get_str(my_handle, "key", buff, &required_size); -
 *     - sets buff to NULL in case of no value.
 * 6. nvs_close(my_handle);
 */



void inline global_variables_init() {
    global_input_queue = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
}



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
    global_variables_init();
    uart_init(9600);
    nvc_init();

    s_wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *my_ap = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    // esp_wifi_start();
    
    xTaskCreate(user_input,    "user_input",    62040, NULL, 10, NULL);
    xTaskCreate(cmd_handler,   "cmd_handler",   62040, NULL, 10, NULL);
}