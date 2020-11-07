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
#include "lwip/err.h"
#include "lwip/sockets.h"


#define ECHO_WRONG_SYNTAX "Wrong syntax: echo ip port sendings_count"
#define ECHO_INVALID_ARGS "Invalid arguments: echo ip port sendings_count"
#define ECHO_SOCKET_ERR   "Unable to create socket..."


// Prints msg in uart with given color and newlines.
static void uart_print(char *msg, bool newline_1, bool newline_2, char *color) {
    if (color != NULL) {
        uart_write_bytes(UART_PORT, color, strlen(color)); 
    }

    if (newline_1) { 
        uart_write_bytes(UART_PORT, "\r\n", 2);
    }

    uart_write_bytes(UART_PORT, msg, strlen(msg));  
    if (newline_2) { 
        uart_write_bytes(UART_PORT, "\r\n", 2);
    }

    if (color != NULL) {
        uart_write_bytes(UART_PORT, RESET_COLOR, strlen(RESET_COLOR)); 
    }
}



// @Validates arguments of echo command 
static bool echo_args_validate(char **cmd, int len) {
  if (len != 4) {
    uart_print(ECHO_WRONG_SYNTAX, 0, 1, RED_TEXT);
    return true;
  }
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
		uart_print(ECHO_INVALID_ARGS, 1, 1, RED_TEXT);
	}
	return status;
}



// Creates socket, connected to `ip` with `port`
static int create_connected_socket(char *ip, int port) {
    struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    int sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        uart_print(ECHO_SOCKET_ERR, 1, 1, RED_TEXT);
        printf("Socket was not created. Errno: %d\n", errno);
        return -1;
    }

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        uart_print("Host is unavailable", 1, 1, RED_TEXT);
        close(sock);
        return -1;
    }
    return sock;
}



void echo_command(char **cmd) {
	int cmd_len = mx_strarr_len(cmd);
	if (echo_args_validate(cmd, cmd_len)) {return;}
    int port = atoi(cmd[2]);

    int num_of_ping = atoi(cmd[3]);
    int i = 0;
    char ping_msg[100];
    char rx_buffer[128];
    int len;
    int sock;
    char *color = GREEN_TEXT;
    while(i < num_of_ping) {
        bzero(ping_msg, 100);
        bzero(rx_buffer, 128);
        len = 0;
        sock = create_connected_socket(cmd[1], port);
        if (sock == -1) {
            sprintf(ping_msg, "PING #%d failed| ip: %s port: %d", i, cmd[1], port);
            color = RED_TEXT; 
        }
        else {
            sprintf(ping_msg, "PING #%d\r\n\r\n", i);
         	send(sock, ping_msg, strlen((char *)ping_msg), 0);
         	len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len <= 0) {
                sprintf(ping_msg, "PING #%d failed| ip: %s port: %d", i, cmd[1], port);
                color = RED_TEXT;
            }
            else {
                sprintf(ping_msg, "PING #%d", i);
                color = GREEN_TEXT;
            }
        }
        uart_print(ping_msg, 0, 1, color);
        i  += 1;
        vTaskDelay(100);
        if (sock != -1) {
            close(sock);
        }
    }
}




