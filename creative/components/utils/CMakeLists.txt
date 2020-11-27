idf_component_register(SRCS "uart_print.c" "create_connected_socket.c" "resolve_ip_by_host_name.c" "get_mac_address.c" "scan_wifi_networks.c" "get_value_by_key_from_nvs.c"
                       INCLUDE_DIRS "."
                       REQUIRES libmx nvs_flash)