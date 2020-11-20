idf_component_register(SRCS "connect_to_wifi.c" "wifi_auto_connect.c"
                    INCLUDE_DIRS "."
                    REQUIRES libmx nvs_flash utils wifi_module)