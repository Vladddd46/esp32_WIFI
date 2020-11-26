#pragma once
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "utils.h"
#include "libmx.h"
#include "connect_to_wifi.h"

#define WIFI_STA_MODE   0
#define WIFI_AP_MODE    1
#define WIFI_APSTA_MODE 2
#define WIFI_MODE WIFI_APSTA_MODE


#define AP_SSID              "ПРИВЕТ"
#define AP_PASSWORD          "qwerty1234"
#define WIFI_CHANNEL           1
#define MAX_NUM_OF_CONNECTIONS 10

#define WIFI_STORAGE			"WIFI_data"
#define NUM_OF_WIFI_RECONNECT   5

/* Wifi Connection States */
#define DISCONNECTED_WIFI_STATE  0
#define DISCONNECTING_WIFI_STATE 1
#define CONNECTING_WIFI_STATE    2
#define CONNECTED_WIFI_STATE     3


// Contains info about wifi connection state.
struct wifi_info_s {
	int  wifi_connection_state; // connection state.
	char *ssid;                // If CONNECTED_WIFI_STATE, ssid = ssid of wifi esp32 connected to. Otherwise => NULL
	char *password;           // If CONNECTED_WIFI_STATE, password = password of wifi esp32 connected to. Otherwise => NULL
};
struct wifi_info_s wifi_info;

// determines, whether some command is being currently executing. (Needs in order not to mix UART output.)
bool cmd_is_executing;

// initialize wifi in STA(station) mode.
void wifi_initialization_in_sta_mode();

// initialize wifi in AP_STA(access point and station) mode.
void wifi_init_apsta(void);

// Handles events occurred with WIFI.
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);



// WIFI EVENTS
void WIFIEVENT_sta_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void WIFIEVENT_sta_lost_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void WIFIEVENT_ap_connected(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void WIFIEVENT_ap_disconnected(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

