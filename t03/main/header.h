#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/dac.h"
#include "driver/ledc.h"
#include <regex.h> 
#include "libmx.h"
#include "esp_types.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "sh1106.h"
#include "driver/i2c.h"
#include <unistd.h>
#include "esp_err.h"
#include "driver/i2s.h"
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
#include "get_dht11_data.h"
#include "http_header.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "connect_to_wifi.h"

/* Config */
#define COMMAND_LINE_MAX_LENGTH 100
#define NEWLINE 				"\n\r"		


QueueHandle_t uart0_queue;
QueueHandle_t dht11_data_queue;

// clears input in case occured event printed something in UART.
bool clear_input_flag; 


void execute(char **cmd, int len);
void user_input();
uint8_t *get_input_from_uart();


void tehu_command(char **cmd, int len);
void dht11_monitor();





