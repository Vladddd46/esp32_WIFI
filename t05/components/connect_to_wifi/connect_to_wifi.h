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
#include "wifi_module.h"

void connect_command(char **cmd);
void wifi_auto_connect();

int connect_to_wifi(char *ssid, char *pass);
void print_connection_log();