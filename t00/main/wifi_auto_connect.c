#include "header.h"

static void inline uart_print(char *msg, bool newline, char *color) {
    if (color != NULL) {
        uart_write_bytes(UART_PORT, color, strlen(color)); 
    }
    uart_write_bytes(UART_PORT, msg, strlen(msg));  

    if (newline) { 
        uart_write_bytes(UART_PORT, "\r\n", 2);
    }

    if (color != NULL) {
        uart_write_bytes(UART_PORT, RESET_COLOR, strlen(RESET_COLOR)); 
    }
}




void wifi_auto_connect() {
    esp_wifi_start();
	esp_err_t err = esp_wifi_scan_start(NULL, true);
	uint16_t ap_num;
	wifi_ap_record_t ap_records[20];
    bzero(ap_records, 20);
    err =  esp_wifi_scan_get_ap_records(&ap_num, ap_records);

    nvs_handle_t my_handle;
    nvs_open(WIFI_STORAGE, NVS_READWRITE, &my_handle);

    char *ap_ssid;
    char *password;

    for(int i = 0; i < ap_num; i++) {
        ap_ssid = mx_string_copy((char *)ap_records[i].ssid);
        size_t required_size;
        nvs_get_str(my_handle, ap_ssid, NULL, &required_size);

        if ((int)required_size == 0) {
            continue;
        }

        password = mx_strnew(required_size);
        nvs_get_str(my_handle, ap_ssid, password, &required_size);

        if (strlen(password) != 0) {
            connect_to_wifi(ap_ssid, password);
            free(password);
            free(ap_ssid);
            break;
        }
        free(password);
        free(ap_ssid);
    }
    nvs_close(my_handle);
}