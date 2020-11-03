#include "header.h"

/* @ Gets input from UART.
 *   Returns characters placed in UART.
 *   Blocks until input is placed.
 *   Returns dynamic memmory.
 *   In case of any error - return NULL.
 */


#define ESP_IDF 0
#define FRAMEWORK ESP_IDF

#if FRAMEWORK == ESP_IDF
uint8_t *get_input_from_uart() {
    size_t  buf_size = 0;
    uint8_t *buf     = NULL;
    uart_event_t event;

    if (xQueueReceive(uart0_queue, (void * )&event, 10)) {
        if (event.type == UART_DATA) {
            uart_get_buffered_data_len(UART_PORT, &buf_size);
            buf =  malloc(sizeof(uint8_t) * (buf_size + 1));
            if (buf == NULL) {
                printf("%s\n", "Malloc returned NULL. Force exit.");
                exit(1);
            }
            bzero(buf, buf_size + 1);
            uart_read_bytes(UART_PORT, buf, buf_size, 1);
        }
    }
    return buf;
}
#endif
