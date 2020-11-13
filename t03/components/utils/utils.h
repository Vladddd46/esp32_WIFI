#include "driver/uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "libmx.h"
#include <netdb.h>

#define UART_PORT   UART_NUM_1

/* Colors */
#define RED_TEXT    "\e[31m"
#define BLUE_TEXT   "\e[34m"
#define GREEN_TEXT  "\e[32m"
#define YELLOW_TEXT "\e[33m"
#define RESET_COLOR "\e[0m"

void uart_print(char *msg, bool newline_1, bool newline_2, char *color);
int create_connected_socket(char *ip, int port);
char *resolve_ip_by_host_name(char *host_name);