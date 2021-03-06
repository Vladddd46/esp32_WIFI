#include "connect_to_wifi.h"

/* @ Connects to acquainted wifi.
 * Scans all available AP(access points).
 * Checks, whether this AP is in NVC storage.
 * If this AP already exists, connects to this AP.
 */



void wifi_auto_connect() {
    esp_err_t err;
    esp_wifi_start();
    esp_wifi_scan_start(NULL, true);

    uint16_t ap_num;
    esp_wifi_scan_get_ap_num(&ap_num);
    wifi_ap_record_t ap_records[ap_num];
    bzero(ap_records, ap_num);
    esp_wifi_scan_get_ap_records(&ap_num, ap_records);

    nvs_handle_t my_handle;
    nvs_open(WIFI_STORAGE, NVS_READWRITE, &my_handle);

    char *ap_ssid;
    char *password;
    for(int i = 0; i < ap_num; i++) {
        if ((char *)ap_records[i].ssid == NULL) {continue;}
        ap_ssid = mx_string_copy((char *)ap_records[i].ssid);
        size_t required_size;
        nvs_get_str(my_handle, ap_ssid, NULL, &required_size);
        if ((int)required_size == 0) {
            continue;
        }
        password = mx_strnew(required_size);
        nvs_get_str(my_handle, ap_ssid, password, &required_size);
        if (strlen(password) != 0) {
            err = connect_to_wifi(ap_ssid, password);
            if (err != -1) {
                free(password);
                free(ap_ssid);
                break;
            }
        }
        free(password);
        free(ap_ssid);
    }
    nvs_close(my_handle);
}

