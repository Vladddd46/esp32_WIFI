idf_component_register(SRCS "t04.c"
                    INCLUDE_DIRS "." 
                    REQUIRES sh1106 libmx "esp_adc_cal" nvs_flash utils dht11 http_commands esp-tls connect_to_wifi input timer)