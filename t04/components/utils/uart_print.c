#include "utils.h"


// Prints msg in uart with given color and newlines.
void uart_print(char *msg, bool newline_1, bool newline_2, char *color) {
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


