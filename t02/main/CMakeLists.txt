idf_component_register(SRCS "t02.c" "input.c" "execute.c" "get_input_from_uart.c" "connect_command.c" "wifi_auto_connect.c" "http_get_command.c" "uart_print.c"
                    INCLUDE_DIRS "."
                    REQUIRES sh1106 libmx "esp_adc_cal" nvs_flash)