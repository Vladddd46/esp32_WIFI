idf_component_register(SRCS "t00.c" "input.c" "execute.c" "get_input_from_uart.c"
                    INCLUDE_DIRS "."
                    REQUIRES wrappers sh1106 libmx dht11 "esp_adc_cal" nvs_flash)