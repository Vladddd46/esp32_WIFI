idf_component_register(SRCS "wifi_module.c" "user_wifi_events.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash utils libmx connect_to_wifi wrappers)