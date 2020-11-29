#include "connect_to_wifi.h"

/* @ Implementation of connect command.
 *  connects to specified SSID(wifi name) with 
 *  specified Password.
 */



#define CONNECT_WRONG_SYNTAX      "Wrong syntax: connect \"SSID\" \"PASS\"; connect status"
#define EXAMPLE_ESP_MAXIMUM_RETRY  100
#define WIFI_CONNECTED_BIT         BIT0
#define WIFI_FAIL_BIT              BIT1


static int connect_command_syntax_validate(char **cmd) {
    int len = mx_strarr_len(cmd);

    if (len != 2 && len != 3) {
        uart_print(CONNECT_WRONG_SYNTAX, 0, 1, RED_TEXT);
        return 1;
    }
    if (len == 2 && strcmp(cmd[1], "status") != 0) {
        uart_print(CONNECT_WRONG_SYNTAX, 0, 1, RED_TEXT);
        return 1;
    }
    return 0;
}



/*  @Prints log just after connection to AP.
 *  LOG:
 *  State: CONNECTED/DISCONNECTED
 *  SSID: wifi_name
 *  Channel: wifi_channel
 *  RRSI: x dBm
 */
void print_connection_log() {
    wifi_ap_record_t ap_info;
    ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&ap_info));

    char txt_log[50];
    char ssid_log[50];
    char channel_log[50];
    char rrsi_log[50];

    bzero(txt_log, 50);
    bzero(ssid_log, 50);
    bzero(channel_log, 50);
    bzero(rrsi_log, 50);

    sprintf(txt_log, "%ssta_state\n\rState:%s CONNECTED", GREEN_TEXT, RESET_COLOR);
    sprintf(ssid_log, "%sSSID:%s %s",                     GREEN_TEXT, RESET_COLOR, ap_info.ssid);
    sprintf(channel_log, "%sChannel:%s %d",               GREEN_TEXT, RESET_COLOR, ap_info.primary);
    sprintf(rrsi_log, "%sRRSI:%s %d dBm",                 GREEN_TEXT, RESET_COLOR, ap_info.rssi);

    uart_print(txt_log,     0, 1, NULL);
    uart_print(ssid_log,    0, 1, NULL);
    uart_print(channel_log, 0, 1, NULL);
    uart_print(rrsi_log,    0, 1, NULL);
}



/*
 * @ Prints status of WiFi connection.
 *  if ESP32 is not connected to wifi => "ESP32 is not connected to WIFI"
 *  else => log:
 *  SSID: wifi_name
 *  WiFi Channel: x
 *  RRSI: x dBm
 */
static void connect_status() {
    wifi_ap_record_t ap_info;
    char ssid[50];
    char channel[50];
    char rssi[50];
    char *msg;
    bzero(ssid,    50);
    bzero(channel, 50);
    bzero(rssi,    50);

    esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);
    if (err == ESP_OK) {
        sprintf(ssid,     "%sSSID:%s %s",        GREEN_TEXT, RESET_COLOR, ap_info.ssid);
        sprintf(channel, "%sWiFi Channel:%s %d", GREEN_TEXT, RESET_COLOR, ap_info.primary);
        sprintf(rssi,    "%sRRSI:%s %d dBm",         GREEN_TEXT, RESET_COLOR, ap_info.rssi);
        msg = "==WiFi Status==";
        uart_print(msg,     0, 1, GREEN_TEXT);
        uart_print(ssid,    0, 1, NULL);
        uart_print(channel, 0, 1, NULL);
        uart_print(rssi,    0, 1, NULL);
    }
    else {
        msg = "ESP32 is not connected to WIFI";
        uart_print(msg, 0, 1, RED_TEXT);
    }
}


/*
 * Saves ssid and pass in
 *  non-volatile storage.
 */
static void inline write_wifi_account_in_nvc(char *ssid, char *pass) {
    nvs_handle_t my_handle;
    esp_err_t err;
    char error_msg[100];
    bzero(error_msg, 100);

    err = nvs_open(WIFI_STORAGE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while opening nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }
    err = nvs_set_str(my_handle, ssid, pass);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while writing in nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        if (ESP_ERR_NVS_KEY_TOO_LONG == err) {
            uart_print("SSID name is too long", 0, 1, RED_TEXT);
        }
        return;
    }
    nvs_close(my_handle);
}



/*
 * Saves ssid and pass in
 *  non-volatile storage.
 */
static void inline write_default_wifi_account_in_nvc(char *ssid, char *pass) {
    nvs_handle_t my_handle;
    esp_err_t err;
    char error_msg[100];
    bzero(error_msg, 100);

    err = nvs_open(WIFI_STORAGE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while opening nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }
    err = nvs_set_str(my_handle, DEFAULT_SSID, ssid);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while writing in nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }
    err = nvs_set_str(my_handle, DEFAULT_PASS, pass);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while writing in nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }
    nvs_close(my_handle);
}




/*
 * Connects to WiFi with given ssid and pass.
 */
int connect_to_wifi(char *ssid, char *pass) {
    int status = 1;

    wifi_config_t wifi_station_config = {
        .sta = {
         .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable  = true,
                .required = false
            },
        },
    };
    memcpy(wifi_station_config.sta.ssid,     ssid, strlen(ssid));
    memcpy(wifi_station_config.sta.password, pass, strlen(pass));

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_station_config);

    wifi_info.wifi_connection_state = DISCONNECTING_WIFI_STATE;
    esp_wifi_disconnect();
    vTaskDelay(10);
    wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;

    vTaskDelay(10);
    uart_print("Connecting...", 1, 0, GREEN_TEXT);
    wifi_info.wifi_connection_state = CONNECTING_WIFI_STATE;
    esp_wifi_connect();


    while(wifi_info.wifi_connection_state != DISCONNECTED_WIFI_STATE 
       && wifi_info.wifi_connection_state != CONNECTED_WIFI_STATE) {
        vTaskDelay(1);
    }
    if (wifi_info.wifi_connection_state == CONNECTED_WIFI_STATE) {
        // write_wifi_account_in_nvc(ssid, pass);
        write_default_wifi_account_in_nvc(ssid, pass);
        if (wifi_info.ssid != NULL) {
            free(wifi_info.ssid);
        }
        if (wifi_info.password != NULL) {
            free(wifi_info.password);
        }
        wifi_info.ssid     = mx_string_copy(ssid);
        wifi_info.password = mx_string_copy(pass);
    }
    else {
        status = -1;
    }
    return status;
}



void connect_command(char **cmd) {
    if (connect_command_syntax_validate(cmd)) {
        return;
    }
    
    char *ssid;
    char *pass;
    if (mx_strarr_len(cmd) == 2) {
          connect_status();
    }
    else {
        ssid = cmd[1];
        pass = cmd[2];
        connect_to_wifi(ssid, pass);
    }
}





