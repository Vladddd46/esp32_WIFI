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

/* Config */
#define COMMAND_LINE_MAX_LENGTH 100
#define NEWLINE 				"\n\r"		
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


QueueHandle_t uart0_queue;
struct wifi_info_s wifi_info;

// clears input in case occured event printed something in UART.
bool clear_input_flag; 
bool cmd_is_executing;

/* dns */
bool DNSFound;
ip_addr_t ip_Addr;

void execute(char **cmd, int len);
void user_input();
uint8_t *get_input_from_uart();

/* Connect */
void connect_command(char **cmd);
void wifi_auto_connect();
int connect_to_wifi(char *ssid, char *pass);

void cmd_handle(char *input);

void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

void http_get_command(char **cmd, int len);


