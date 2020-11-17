#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "libmx.h"
#include "esp_types.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include <unistd.h>
#include "esp_err.h"
#include <strings.h>
#include <ctype.h>
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.h"

#define WIFI_STORAGE			"WIFI_data"
#define NUM_OF_WIFI_RECONNECT   5

/* Wifi Connection States */
#define DISCONNECTED_WIFI_STATE  0
#define DISCONNECTING_WIFI_STATE 1
#define CONNECTING_WIFI_STATE    2
#define CONNECTED_WIFI_STATE     3

struct wifi_info_s {
	int  wifi_connection_state;
	char *ssid;
	char *password;
};

struct wifi_info_s wifi_info;
bool cmd_is_executing;

void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void connect_command(char **cmd);
void wifi_auto_connect();

int connect_to_wifi(char *ssid, char *pass);
