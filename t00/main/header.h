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

/* Config */
#define COMMAND_LINE_MAX_LENGTH 100
#define UART_PORT 				UART_NUM_1
#define NEWLINE 				"\n\r"		

/* Colors */
#define RED_TEXT    "\e[31m"
#define BLUE_TEXT   "\e[34m"
#define GREEN_TEXT  "\e[32m"
#define YELLOW_TEXT "\e[33m"
#define RESET_COLOR "\e[0m"

xQueueHandle  global_input_queue;
QueueHandle_t uart0_queue;

void user_input();
void cmd_handler();
void execute(char **cmd, int len);

uint8_t *get_input_from_uart();
void connect_command(char **cmd);