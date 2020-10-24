#include "header.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"


/* @ Implementation of connect command.
 * connects to specified SSID(wifi name) with 
 *  specified Password.
 */

#define CONNECT_WRONG_SYNTAX      "\e[31mWrong syntax: connect SSID PASS; connect status\e[0m"
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


static void  inline uart_print(char *msg, bool newline) {
    uart_write_bytes(UART_PORT, msg, strlen(msg));  
    if (newline) uart_write_bytes(UART_PORT, "\r\n", 2);
}



static int syntax_validate(char **cmd) {
	int len = mx_strarr_len(cmd);

	if (len != 2 && len != 3) {
		uart_print(CONNECT_WRONG_SYNTAX, 1);
		return 1;
	}
	return 0;
}


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
    	sprintf(ssid,     "SSID: %s",        ap_info.ssid);
    	sprintf(channel, "WiFi Channel: %d", ap_info.primary);
    	sprintf(rssi,    "RRSI: %d",         ap_info.rssi);
    	msg = "WiFi Status:";
    	uart_print(msg, 1);
    	uart_print(ssid, 1);
    	uart_print(channel, 1);
    	uart_print(rssi, 1);
    }
    else {
    	msg = "ESP32 is not connected to WIFI";
    	uart_print(msg, 1);
    }
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void connect_to_wifi(char *ssid, char *pass) {
	s_wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // esp_wifi_scan_start(constwifi_scan_config_t *config, bool block)
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);

    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_station_config = {
        .sta = {
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
         .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    memcpy(wifi_station_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_station_config.sta.password, pass, strlen(pass));

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_station_config);
    esp_wifi_start();

    // ESP_LOGI(TAG, "wifi_init_sta finished.");

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
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ssid, pass);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ssid, pass);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }


         wifi_ap_record_t ap_info;
         tcpip_adapter_ip_info_t ip_info;

    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
    ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&ap_info));
    printf("\nSuccess! Got IP:  %s\n\n", ip4addr_ntoa(&ip_info.ip));
    printf("sta_state\nState: ");
    printf("CONNECTED\n");
    printf("SSID: %s\n", ap_info.ssid);
    printf("Channel: %d\n", ap_info.primary);
    printf("RRSI: %d dBm\n", ap_info.rssi);


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
	if (mx_strarr_len(cmd) == 2 && !strcmp(cmd[1], "status")) {
		connect_status();
	}
	else {
		ssid = cmd[1];
		pass = cmd[2];
		connect_to_wifi(ssid, pass);
	}
}