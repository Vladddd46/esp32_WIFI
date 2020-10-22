#include "header.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"


/* @ Implementation of connect command.
 * connects to specified SSID(wifi name) with 
 *  specified Password.
 */

#define CONNECT_WRONG_SYNTAX "\e[31mWrong syntax: connect SSID PASS; connect status\e[0m"



static void  inline uart_print(char *msg, bool newline) {
    uart_write_bytes(UART_PORT, msg, strlen(msg));  
    if (newline) uart_write_bytes(UART_PORT, "\r\n", 2);
}



static int syntax_validate(char **cmd) {
	int len = 0;
	while(cmd[len]) {
		len += 1;
	}

	if (cmd[1])
		printf("%d %s\n",len, cmd[1]);
	if (len != 2 && len != 3) {
		uart_print(CONNECT_WRONG_SYNTAX, 1);
		return 1;
	}
	return 0;
}


// static void connect_status() {}


static void connect_to_wifi(char *ssid, char *pass) {
	printf("llkfdl\n");


}



void connect_command(char **cmd) {
	if (syntax_validate(cmd)) {
		return;
	}
	
	char *ssid;
	char *pass;
	if (!strcmp(cmd[1], "status")) {
		// connect_status();
	}
	else {
		ssid = cmd[1];
		pass = cmd[2];
		connect_to_wifi(ssid, pass);
	}
}