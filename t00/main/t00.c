#include "header.h"



void inline global_variables_init() {
    global_input_queue = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
}



void app_main() {
    global_variables_init();
    uart_init(9600);

    xTaskCreate(user_input,    "user_input",    12040, NULL, 10, NULL);
    xTaskCreate(cmd_handler,   "cmd_handler",   12040, NULL, 10, NULL);
}