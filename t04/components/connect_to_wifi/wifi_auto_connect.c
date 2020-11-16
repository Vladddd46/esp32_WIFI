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
    wifi_ap_record_t ap_records[20];
    bzero(ap_records, 20);
    esp_wifi_scan_get_ap_records(&ap_num, ap_records);


    nvs_handle_t my_handle;
    nvs_open(WIFI_STORAGE, NVS_READWRITE, &my_handle);
    char ap_ssid[100];
    char *password;

    for(int i = 0; i < ap_num; i++) {

        if ((char *)ap_records[i].ssid == NULL) {continue;}
        printf("%s\n", ap_records[i].ssid);
        // bzero(ap_ssid, 100);
        // sprintf(ap_ssid, "%s", ap_records[i].ssid);
        // size_t required_size;
        // nvs_get_str(my_handle, ap_ssid, NULL, &required_size);
        // if ((int)required_size == 0) {
        //     continue;
        // }
        // printf("!5\n");
        // password = mx_strnew(required_size);
        // nvs_get_str(my_handle, ap_ssid, password, &required_size);
        // if (strlen(password) != 0) {
        //     err = connect_to_wifi(ap_ssid, password);
        //     if (err != -1) {
        //         free(password);
        //         break;
        //     }
        // }
        // free(password);
    }
    nvs_close(my_handle);\
}

