#include "header.h"

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
// #include "protocol_examples_common.h"
// #include "addr_from_stdin.h"
#include "lwip/err.h"
#include "lwip/sockets.h"


#define ECHO_WRONG_SYNTAX "Wrong syntax: echo ip port sendings_count"
#define ECHO_INVALID_ARGS "Invalid arguments: echo ip port sendings_count"

static void inline uart_print(char *msg, bool newline, char *color) {
    if (color != NULL) {
        uart_write_bytes(UART_PORT, color, strlen(color)); 
    }
    uart_write_bytes(UART_PORT, msg, strlen(msg));  

    if (newline) { 
        uart_write_bytes(UART_PORT, "\r\n", 2);
    }

    if (color != NULL) {
        uart_write_bytes(UART_PORT, RESET_COLOR, strlen(RESET_COLOR)); 
    }
}



static int inline echo_args_validate(char **cmd) {
	char *ip             = cmd[1];
	char *tcp_port       = cmd[2];
	char *sendings_count = cmd[3];
	bool status = false;

	for (int i = 0; ip[i] && status == false; ++i) {
		if (isdigit(ip[i]) == 0 && ip[i] != '.') {
			status = true;
		}
	}

	for (int i = 0; ip[i + 1] && status == false; ++i) {
		if (ip[i] == '.' && ip[i + 1] == '.') {
			status = true;
		}
	}

	for (int i = 0; tcp_port[i] && status == false; ++i) {
		if (isdigit(tcp_port[i]) == 0) {
			status = true;
		}
	}
	for (int i = 0; sendings_count[i] && status == false; ++i) {
		if (isdigit(sendings_count[i]) == 0) {
			status = true;
		}
	}

	if (status) {
		uart_print(ECHO_INVALID_ARGS, 1, RED_TEXT);
	}
	return status;
}



void echo_command(char **cmd) {
	int cmd_len = mx_strarr_len(cmd);
	if (cmd_len != 4) {
		uart_print(ECHO_WRONG_SYNTAX, 1, RED_TEXT);
		return;
	}
	if (echo_args_validate(cmd)) {
		return;
	}

    struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(5000);

    int sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock < 0) {
    	printf("%d\n", errno);
    }
    else {
    	printf("==>%d\n", sock);
    }


    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
    	printf("error %d %d\n", errno, err);
    }
   printf("successfully created\n");

}












