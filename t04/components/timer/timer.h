#pragma once
#include <stdio.h>
#include "driver/gpio.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/ledc.h"
#include "driver/timer.h"
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include "esp_log.h"
#include "driver/dac.h"
#include "esp_sntp.h"
#include <time.h>
#include <string.h>
#include "driver/gpio.h"
#include "libmx.h"
#include "driver/timer.h"
#include "sh1106.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include <ctype.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.h"
#include "connect_to_wifi.h"
#include "input.h"
#include "tehu.h"
#include "timer.h"

#define TIMEZONE_STORAGE "time-zone"

int current_time;
int synchronized_time;
TaskHandle_t  xTaskClock;

void tz_set(char **cmd, int len);
void time_synchronization(void *arg);
void timer_task(void *arg);
void init_tz();

