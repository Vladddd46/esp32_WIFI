#include "header.h"

#define ECHO_WRONG_SYNTAX "Wrong syntax: echo ip port sendings_count"

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



void echo_command(char **cmd) {
	int cmd_len = mx_strarr_len(cmd);
	if (cmd_len != 4) {
		uart_print(ECHO_WRONG_SYNTAX, 1, RED_TEXT);
		return;
	}
}