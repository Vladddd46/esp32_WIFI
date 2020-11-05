#include "header.h"

/*
 * Determines the type of command
 * to be executed.
 */

static void no_such_command_error() {
    char *msg = "\e[31mNo such command.\e[36mWrite \e[32mhelp \e[36mto list all supported commands.\e[0m\n\r";
    uart_write_bytes(UART_PORT, msg, strlen(msg));
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
    else 
        no_such_command_error();

    const char *prompt = "Enter your command : ";
    uart_write_bytes(UART_PORT, prompt, strlen(prompt));
}

