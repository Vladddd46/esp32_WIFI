#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "wrappers.h"
#include "driver/dac.h"
#include "driver/ledc.h"
#include <regex.h> 
#include "get_dht11_data.h"
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

/* Config */
#define COMMAND_LINE_MAX_LENGTH 100
#define UART_PORT 				UART_NUM_1
#define NEWLINE 				"\n\r"		

xQueueHandle  global_input_queue;
QueueHandle_t uart0_queue;

void user_input();
void cmd_handler();
void execute(char **cmd, int len);


uint8_t *get_input_from_uart();