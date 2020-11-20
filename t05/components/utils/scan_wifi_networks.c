#include "utils.h"



static bool str_is_in_arr(char **arr, char *str) {
    for (int i = 0; arr[i]; ++i) {
        if (!strcmp(str, arr[i])) {
            return true;
        }
    }
    return false;
}



char **scan_wifi_networks() {
    esp_wifi_scan_start(NULL, true);
    uint16_t ap_num;
    esp_wifi_scan_get_ap_num(&ap_num);
    
    wifi_ap_record_t ap_records[ap_num];
    bzero(ap_records, ap_num);
    esp_wifi_scan_get_ap_records(&ap_num, ap_records);

    char buff[100];
    char **res = mx_strarr_new(ap_num);
    int index = 0;
    for(int i = 0; i < ap_num; i++) {
        if ((char *)ap_records[i].ssid == NULL) {continue;}
        bzero(buff, 100);
        sprintf(buff, "%s", ap_records[i].ssid);
        if (str_is_in_arr(res, (char *)buff) == false) {
            res[index] = mx_string_copy(buff);
            index++;
        }
    }
    return res;
}
