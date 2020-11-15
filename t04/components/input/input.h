#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "libmx.h"
#include "esp_types.h"
#include "freertos/queue.h"
#include <unistd.h>
#include "esp_err.h"
#include <strings.h>
#include <ctype.h>
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "utils.h"
#include "tehu.h"
#include "http_header.h"
#include "timer.h"

/* Config */
#define COMMAND_LINE_MAX_LENGTH 100
#define NEWLINE 				"\n\r"		

QueueHandle_t uart0_queue;

// clears input in case occured event printed something in UART.
bool clear_input_flag; 

uint8_t *get_input_from_uart();
void user_input();
void execute(char **cmd, int len);