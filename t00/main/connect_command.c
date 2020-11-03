#include "header.h"

/* @ Implementation of connect command.
 * connects to specified SSID(wifi name) with 
 *  specified Password.
 */



#define CONNECT_WRONG_SYNTAX      "Wrong syntax: connect \"SSID\" \"PASS\"; connect status"
#define EXAMPLE_ESP_MAXIMUM_RETRY  100
#define WIFI_CONNECTED_BIT         BIT0
#define WIFI_FAIL_BIT              BIT1



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



static int syntax_validate(char **cmd) {
    int len = mx_strarr_len(cmd);

    if (len != 2 && len != 3) {
        uart_print(CONNECT_WRONG_SYNTAX, 1, RED_TEXT);
        return 1;
    }
    if (len == 2 && strcmp(cmd[1], "status") != 0) {
        uart_print(CONNECT_WRONG_SYNTAX, 1, RED_TEXT);
        return 1;
    }
    return 0;
}



// Prints log just after connection to AP.
static void inline print_connection_log() {
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

    uart_print(txt_log,     1, NULL);
    uart_print(ssid_log,    1, NULL);
    uart_print(channel_log, 1, NULL);
    uart_print(rrsi_log,    1, NULL);
}



/*
 * @ Prints status of WiFi connection.
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
        sprintf(rssi,    "%sRRSI:%s %d",         GREEN_TEXT, RESET_COLOR, ap_info.rssi);
        msg = "==WiFi Status==";
        uart_print(msg,     1, GREEN_TEXT);
        uart_print(ssid,    1, NULL);
        uart_print(channel, 1, NULL);
        uart_print(rssi,    1, NULL);
    }
    else {
        msg = "ESP32 is not connected to WIFI";
        uart_print(msg, 1, RED_TEXT);
    }
}



void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *info = event_data;
        char msg[1000];
        bzero(msg, 1000);

        if (wifi_info.wifi_connection_state == DISCONNECTING_WIFI_STATE) {
            uart_print("ESP32 is diconnected from WIFI", 1, RED_TEXT);
        }
        else if (wifi_info.wifi_connection_state == CONNECTING_WIFI_STATE) {
            if (info->reason == 204 || info->reason == 15) {
                sprintf(msg, "Password is wrong");
            }
            else if (info->reason == 201) {
                sprintf(msg, "There is no Access Point with given SSID");
            }
            else {
                sprintf(msg, "ESP32 is not connected to WiFi. Error code: %d", (int)info->reason);
            }
            uart_print(msg, 1, RED_TEXT);
        }
        else if (wifi_info.wifi_connection_state == CONNECTED_WIFI_STATE) {
            wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
            sprintf(msg, "ESP32 is disconnected from WiFi. Error code: %d", (int)info->reason);
            uart_print("\r\n", 0, RED_TEXT);
            uart_print(msg, 1, RED_TEXT);
            
            // uart_print("Trying to reconnect...", 1, GREEN_TEXT);

            // int num_of_reconnection = 0;
            // while(num_of_reconnection < NUM_OF_WIFI_RECONNECT) {
            //     connect_to_wifi(wifi_info.ssid, wifi_info.password);
            //     if (wifi_info.wifi_connection_state == CONNECTED_WIFI_STATE) {
            //         break;
            //     }
            //     num_of_reconnection += 1;
            // }
        }
        // if (wifi_info.wifi_connection_state != CONNECTED_WIFI_STATE) {
            wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
        // }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        uart_print("\n\rSuccess! Got IP: ", 0, GREEN_TEXT);
        char got_ip[50];
        bzero(got_ip, 50);
        sprintf(got_ip, "%s", ip4addr_ntoa(&event->ip_info.ip));
        uart_print(got_ip, 1, NULL);
        print_connection_log();
        wifi_info.wifi_connection_state = CONNECTED_WIFI_STATE;
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP) {
        uart_print("Lost IP", 1, RED_TEXT);
        wifi_info.wifi_connection_state = DISCONNECTING_WIFI_STATE;
        esp_wifi_disconnect();
        vTaskDelay(10);
        wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;
    }
}



static void inline write_wifi_account_in_nvc(char *ssid, char *pass) {
    nvs_handle_t my_handle;
    esp_err_t err;
    char error_msg[100];
    bzero(error_msg, 100);

    err = nvs_open(WIFI_STORAGE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while opening nvc: %d", err);
        uart_print(error_msg, 1, RED_TEXT);
        return;
    }
    err = nvs_set_str(my_handle, ssid, pass);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while writing in nvc: %d", err);
        uart_print(error_msg, 1, RED_TEXT);
        return;
    }
    nvs_close(my_handle);
}



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

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_station_config);

    wifi_info.wifi_connection_state = DISCONNECTING_WIFI_STATE;
    esp_wifi_disconnect();
    vTaskDelay(10);
    wifi_info.wifi_connection_state = DISCONNECTED_WIFI_STATE;

    vTaskDelay(10);
    uart_print("Connecting...", 1, GREEN_TEXT);
    wifi_info.wifi_connection_state = CONNECTING_WIFI_STATE;
    esp_wifi_connect();

    while(wifi_info.wifi_connection_state != DISCONNECTED_WIFI_STATE 
       && wifi_info.wifi_connection_state != CONNECTED_WIFI_STATE) {
        vTaskDelay(1);
    }
    if (wifi_info.wifi_connection_state == CONNECTED_WIFI_STATE) {
        write_wifi_account_in_nvc(ssid, pass);
        
        if (wifi_info.ssid != NULL) {
            free(wifi_info.ssid);
        }
        if (wifi_info.password != NULL) {
            free(wifi_info.password);
        }
        wifi_info.ssid     = mx_string_copy(ssid);
        wifi_info.password = mx_string_copy(pass);
    }
    return status;
}



void connect_command(char **cmd) {
    if (syntax_validate(cmd)) {
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
        printf("%s %s\n", ssid, pass);
        connect_to_wifi(ssid, pass);
    }
}





