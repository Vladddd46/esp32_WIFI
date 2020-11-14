#include "tehu.h"

/* @ Makes/Stops esp32 send data from dht11 sensor to remote server. 
 * Syntax: tehu start [server_ip] [server_port]
 *         tehu stop 
 * If tehu start -> sends in queue(dht11_data_queue) str with ip and port, 
 *  splited by space. str = "127.0.0.1 8080"
 * If tehu stop -> sends "stop" str in queue(dht11_data_queue).
 */

#define TEHU_WRONG_SYNTAX "Wrong syntax:\n\rtehu start domain port\n\rtehu stop"



// Validates arguments of tehu command.
static int tehu_syntax_validator(char **cmd, int len) {
	if (len != 2 && len != 4) {
		uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
		return 1;
	}
	if (len == 2 && strcmp("stop", cmd[1]) != 0) {
		uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
		return 1;
	}
	// port validation
	if (len == 4) {
		for (int i = 0; cmd[3][i]; ++i) {
			if (isdigit(cmd[3][i]) == 0) {
				uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
				return 1;
			}
		}
	}
	return 0;
}



void tehu_command(char **cmd, int len) {
	if (tehu_syntax_validator(cmd, len)) {return;}

	char data[100];
	bzero(data, 100);

	if (len == 2) {
		sprintf(data, "stop");
		uart_print("Sending data from dht11 stopped", 0, 1, BLUE_TEXT);
	}
	else {
		sprintf(data, "%s %s", cmd[2], cmd[3]);
		uart_print("Start sending data from dht11 to server>", 0, 0, GREEN_TEXT);
		uart_print(cmd[2], 0, 0, GREEN_TEXT);
		uart_print(":",    0, 0, GREEN_TEXT);
		uart_print(cmd[3], 0, 1, GREEN_TEXT);
	}
	xQueueSend(dht11_data_queue, (void *)data, (TickType_t)0);
}