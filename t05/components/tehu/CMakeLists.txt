idf_component_register(SRCS "dht11_monitor.c" "tehu_command.c"
                    INCLUDE_DIRS "."
                    REQUIRES libmx nvs_flash utils dht11 connect_to_wifi esp-tls)