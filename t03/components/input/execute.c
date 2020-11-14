#include "input.h"

/*
 * Determines the type of command
 * to be executed.
 */

static void inline no_such_command_error() {
    uart_print("No such command.", 0, 0, RED_TEXT);
    uart_print("Write help to list all supported commands.", 0, 1, BLUE_TEXT);
}



void execute(char **cmd, int len) {
    if (len == 0) {
        // pass
    }
    else if (!strcmp(cmd[0], "connect")) {
        connect_command(cmd);
    }
    else if (!strcmp(cmd[0], "http_get")) {
        http_get_command(cmd, len);
    }
    else if (!strcmp(cmd[0], "tehu")) {
        tehu_command(cmd, len);
    }
    else  {
        no_such_command_error();
    }

    const char *prompt = "Enter your command : ";
    uart_write_bytes(UART_PORT, prompt, strlen(prompt));
}

