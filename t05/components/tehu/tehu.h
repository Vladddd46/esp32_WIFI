#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "libmx.h"
#include "esp_types.h"
#include "freertos/queue.h"
#include "driver/i2c.h"
#include <unistd.h>
#include "esp_err.h"
#include "driver/i2s.h"
#include <strings.h>
#include <ctype.h>
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
#include "get_dht11_data.h"
#include "connect_to_wifi.h"
// libs for tls.
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include <errno.h>
#include <mbedtls/error.h>

QueueHandle_t dht11_data_queue;

void tehu_command(char **cmd, int len);
void dht11_monitor();

