#include "header.h"

#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same
#define BACK_SPACE    127
#define ENTER         13

#define PROMPT              "Enter your command : "
#define LENGTH_ERR          "\e[31mCommand can`t be longer than 100 symbols!\e[0m"
#define INVALID_NUM_DQUOTES "\e[31mInvalid number of doue quotes in command\e[0m"


static void  inline uart_print(char *msg, bool newline) {
    if (newline) uart_write_bytes(UART_PORT, "\r\n", 2);
    uart_write_bytes(UART_PORT, msg, strlen(msg));  
    if (newline) uart_write_bytes(UART_PORT, "\r\n", 2);
}



static void inline erase_char() {
    char c = 8;
    char *tmp = &c;
    uart_write_bytes(UART_PORT, tmp, 1);
    uart_write_bytes(UART_PORT, " ", 1);
    uart_write_bytes(UART_PORT, tmp, 1);
}



/*
 * Reads user`s input from UART and stores it in comman_line string.
 * When user press enter string command_line is sending to cmd_handler
 * task through queue.
 */
void user_input() {
    uint8_t command_line[COMMAND_LINE_MAX_LENGTH];
    uint8_t *buf     = NULL;
    size_t  buf_size = 0;
    int     index    = 0;
    char    analyzed_buffer[1000];
    int     indx = 0;

    uart_print(PROMPT, 0);
    while(1) {
        bzero(command_line, COMMAND_LINE_MAX_LENGTH);
        while (1) {
            buf = (uint8_t *)get_input_from_uart();
            if (buf == NULL) {
                continue;
            }
            buf_size = strlen((char *)buf);

            // Too long buf.
            if (buf_size > 100 || index >= 100) {
                uart_print(LENGTH_ERR, 1);
                uart_print(PROMPT, 0);
                free(buf);
                break;
            }

            // Analyzing input.
            bzero(analyzed_buffer, 1000);
            indx = 0;
            for (int i = 0; i < buf_size; ++i) {
                if (buf[i] == CR_ASCII_CODE || (buf[i] > 31 && buf[i] < 127)) {
                    analyzed_buffer[indx] = buf[i];
                    indx += 1;
                }
                else if (buf[i] == 27) {
                    i += 3;
                }
            }

            // User pressed enter.
            if (mx_char_in_str((char *)buf, 13)  > 0) {
                uart_write_bytes(UART_PORT, "\n\r", 2);
                if (!xQueueSend(global_input_queue, command_line, (200 / portTICK_PERIOD_MS)))
                    printf("Failed to send data in queue\n");
                free(buf);
                break;
            }

            // User pressed backspace.
            if (buf[0] == BACK_SPACE) {
                if (index > 0) {
                    erase_char();
                    command_line[index - 1] = '\0';
                    index -= 1;
                }
            }

            // Displaying input in console.
            uart_print((char *)analyzed_buffer, 0);
            for (int i = 0; analyzed_buffer[i]; ++i) {
                if (analyzed_buffer[i] != BACK_SPACE) {
                    command_line[index] = analyzed_buffer[i];
                    index++;
                }
             }
            free(buf);
        }
        index    = 0;
    }
}


/* All spaces, which are in double quotes, 
 *  are changed to 8 ASCII character in 
 *  order to split input to arr by space. 
 * Then there is function(unlock_spaces_in_quotes),
 *  which changes back all 8 ASCII characters in quotes
 *  to space.
 */
static char *save_spaces_in_quotes(char *input) {
    int num_of_dquotes = mx_count_char(input, '"');
    if (num_of_dquotes % 2 != 0) {
        uart_print(INVALID_NUM_DQUOTES, 0);
        uart_print("\r\n", 0);
        uart_print(PROMPT, 0);
        return NULL;
    }
    char *saved_spaces = mx_strnew(strlen(input));

    bool quote = 0;
    for (int i = 0; input[i]; ++i) {
        if (input[i] == '"') {
            if (quote) {
                quote = 0;
            }
            else {
                quote = 1;
            }
        }
        if (input[i] == ' ' && quote  == 1) {
            saved_spaces[i] = 8;
        }
        else {
            saved_spaces[i] = input[i];
        }
    }
    return saved_spaces;
}



/*
 * Changes all 8 ASCII characters in spaces.
 * If word is in double quotes - double quotes
 *  are deleted.
 */
static char **unlock_spaces_in_quotes(char **cmd) {
    int cmd_len    = mx_strarr_len(cmd);
    char **new_cmd = mx_strarr_new(cmd_len);
    char *tmp;

    int len;
    int j = 0;
    int index;

    for (int i = 0; cmd[i]; ++i) {
        tmp = mx_strnew(strlen(cmd[i]));

        if (cmd[i][0] == '"' && cmd[i][strlen(cmd[i]) - 1] == '"') {
            len = strlen(cmd[i]) - 1;
            j = 1;
        }
        else {
            len = strlen(cmd[i]);
            j = 0;
        }

        index = 0;
        for (; j < len; ++j) {
            if (cmd[i][j] == 8) {
                tmp[index] = ' ';
            }
            else {
                tmp[index] = cmd[i][j];
            }
            index += 1;
        }
        new_cmd[i] = tmp;
    }
    return new_cmd;
}



/*
 * Receives user`s input from Queue.
 * Splits user`s input in arr.
 * Calls execute function, which is in charge 
 * of executing command.
 */
void cmd_handler() {
    char received_input[1000];
    bzero(received_input, 1000);
    int index = 0;
    char *p = NULL;
    char **cmd = (char **)malloc(100 * sizeof(char *));
    if (cmd == NULL) {
        printf("Malloc returned NULL. Fatal error.\n");
        exit(1);
    }
    char *locked_spaces_in_quote;
    char **new_cmd;

    while(1) {  
        if (xQueueReceive(global_input_queue, received_input, (200 / portTICK_PERIOD_MS))) {
            for (int i = 0; i < 100; ++i) cmd[i] = NULL;
            locked_spaces_in_quote = save_spaces_in_quotes(received_input);
            if (locked_spaces_in_quote == NULL) {
                continue;
            }
            // splitting str into arr.
            index = 0;
            p = strtok(locked_spaces_in_quote, " ");
            cmd[index] = p;
            index++;
            while(p != NULL || index < 100) {
                p = strtok(NULL, " ");
                cmd[index] = p;
                index++;
            }
            // 
            new_cmd = unlock_spaces_in_quotes(cmd);
            int cmd_len = 0;
            while(cmd[cmd_len] && cmd_len < 100) cmd_len++;
            execute(new_cmd, cmd_len);
            free(locked_spaces_in_quote);
            for (int i = 0; new_cmd[i]; ++i) {
                free(new_cmd[i]);
            }
            free(new_cmd);
        }
    }
}

