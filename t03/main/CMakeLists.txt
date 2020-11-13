idf_component_register(SRCS "t03.c" "input.c" "execute.c" "get_input_from_uart.c" "connect_command.c" "wifi_auto_connect.c" "tehu_command.c" "dht11_monitor.c"
                    INCLUDE_DIRS "." 
                    REQUIRES sh1106 libmx "esp_adc_cal" nvs_flash utils dht11 http_commands esp-tls)