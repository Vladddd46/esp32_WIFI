#include "header.h"




/* @ Implementation of connect command.
 * connects to specified SSID(wifi name) with 
 *  specified Password.
 */

#define CONNECT_WRONG_SYNTAX      "Wrong syntax: connect SSID PASS; connect status"
#define EXAMPLE_ESP_MAXIMUM_RETRY  100

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1


static const char *TAG = "wifi station";

static int s_retry_num = 0;


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

static void inline print_connection_log() {
    wifi_ap_record_t ap_info;
    tcpip_adapter_ip_info_t ip_info;
    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
    ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&ap_info));

    char ip_log[80];
    char txt_log[50];
    char ssid_log[50];
    char channel_log[50];
    char rrsi_log[50];

    bzero(ip_log, 80);
    bzero(txt_log, 50);
    bzero(ssid_log, 50);
    bzero(channel_log, 50);
    bzero(rrsi_log, 50);

    sprintf(ip_log, "%sSuccess! Got IP:%s  %s",         GREEN_TEXT, RESET_COLOR, ip4addr_ntoa(&ip_info.ip));
    sprintf(txt_log, "%ssta_state\n\rState:%s CONNECTED", GREEN_TEXT, RESET_COLOR);
    sprintf(ssid_log, "%sSSID:%s %s",                     GREEN_TEXT, RESET_COLOR, ap_info.ssid);
    sprintf(channel_log, "%sChannel:%s %d",               GREEN_TEXT, RESET_COLOR, ap_info.primary);
    sprintf(rrsi_log, "%sRRSI:%s %d dBm",                 GREEN_TEXT, RESET_COLOR, ap_info.rssi);

    uart_print(ip_log,      1, NULL);
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
	bzero(ssid,   50);
	bzero(channel, 50);
	bzero(rssi,   50);

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
        // if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
        //     esp_wifi_connect();
        //     s_retry_num++;
        //     ESP_LOGI(TAG, "retry to connect to the AP");
        // } else {
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        // }
        // ESP_LOGI(TAG,"connect to the AP fail");
        // uart_print("Connection Failed",      1, RED_TEXT);
    }
     else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void connect_to_wifi(char *ssid, char *pass) {
	s_wifi_event_group = xEventGroupCreate();
    printf("asdfghj\n");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,  &event_handler,   NULL, &instance_any_id);
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
    printf("qqqqqqqqq\n");
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_station_config);
    esp_wifi_start();

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        print_connection_log();
    } else if (bits & WIFI_FAIL_BIT) {
        uart_print("Connection Failed1",      1, RED_TEXT);
    } else {
        uart_print("Unexpected event",       1, RED_TEXT);
    }
    printf("12345\n");
    /* The event will not be processed after unregister */
    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip);
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id);
    vEventGroupDelete(s_wifi_event_group);
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





