idf_component_register(SRCS "wifi_module.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash utils libmx connect_to_wifi)