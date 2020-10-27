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



static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
     else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        uart_print("\n\rSuccess! Got IP: ", 0, GREEN_TEXT);
        char got_ip[50];
        bzero(got_ip, 50);
        sprintf(got_ip, "%s", ip4addr_ntoa(&event->ip_info.ip));
        uart_print(got_ip, 1, NULL);
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
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
    esp_wifi_stop();
    vTaskDelay(10);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    &event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);

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
    esp_wifi_start();

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdTRUE,
            pdFALSE,
            1500);

    if (bits & WIFI_CONNECTED_BIT) {
        print_connection_log();
        write_wifi_account_in_nvc(ssid, pass);
    } 
    else if (bits & WIFI_FAIL_BIT) {
        uart_print("Connection to Wifi failed", 1, RED_TEXT);
        status = -1;
    } 
    else {
        uart_print("Unexpected event",  1, RED_TEXT);
        status = -1;
    }

    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip);
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id);
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
		connect_to_wifi(ssid, pass);
	}
}





