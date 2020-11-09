#include "header.h"

#define TEHU_WRONG_SYNTAX "Wrong syntax:\n\rtehu start ip_address port\n\rtehu stop"

static int inline tehu_syntax_validator(char **cmd, int len) {
	if (len != 2 && len != 4) {
		uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
		return 1;
	}
	if (len == 2 && strcmp("stop", cmd[1]) != 0) {
		uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
		return 1;
	}
	// port and ip validation
	if (len == 4) {
		for (int i = 0; cmd[3][i]; ++i) {
			if (isdigit(cmd[3][i]) == 0) {
				uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
				return 1;
			}
		}
		for (int i = 0; cmd[2][i]; ++i) {
			if (isdigit(cmd[2][i]) == 0 && cmd[2][i] != '.') {
				uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
				return 1;
			}
			else if () {
				uart_print(TEHU_WRONG_SYNTAX, 0, 1, RED_TEXT);
				return 1;
			}
		}
	}
	return 0;
}

void tehu_command(char **cmd, int len) {
	if (tehu_syntax_validator(cmd, len)) {return;}

	
}