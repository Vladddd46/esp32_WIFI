idf_component_register(SRCS "my_http_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_http_server nvs_flash utils wifi_module)