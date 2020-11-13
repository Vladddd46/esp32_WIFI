#include "http_header.h"

/* @Sends http GET request.
 * Resolves url with help of dns.
 * Forms http get packet.
 * Sends packet and receives response.
 * Prints response.
 */

#define HTTP_GET_WRONG_SYNTAX "Wrong syntax: http_get url/must/be/here"
#define NO_SUCH_HOST       "Host not found"



/*
 * Takes host_name as argument and returns
 * ip address of this host.
 * In case of no ip associated with such host, returns NULL.
 */
static char *resolve_ip_by_host_name(char *host_name) {
    struct hostent *ghost = gethostbyname(host_name);
    if (ghost != NULL) {
        char *ip = inet_ntoa(*(struct in_addr*)ghost->h_addr);
        if (ip != NULL) {
            return mx_string_copy(ip);
        }
    }
    else {
        uart_print(NO_SUCH_HOST, 0, 1, RED_TEXT);
    }
    return NULL;
}



// Sends simple http request.
static void inline send_http_get(int sock, char *path) {
    char get_request[1000];
    bzero(get_request, 1000);

    if (strlen(path) == 0) {
        sprintf(get_request, "GET / HTTP/1.0\r\n\r\n");
    }
    else {
        sprintf(get_request, "GET /%s HTTP/1.0\r\n\r\n", path);
    }
    send(sock, get_request, strlen(get_request), 0);
}



/* @ Splits url into {host_name, path/to/document}
 * Takes url as argument: host_name/path/to/document.
 * Returns array of 2 elements {host_name, path/to/document}
 */
static char **split_url(char *url) {
    char **arr = mx_strarr_new(2);
    char host_name[500];
    bzero(host_name, 500);
    int i = 0;
    int index = 0;
    for (; url[i]; ++i) {
        if (url[i] == '/') {
            i += 1;
            break;
        }
        host_name[i] = url[i];
    }
    char path_to_doc[500];
    bzero(path_to_doc, 500);
    index = 0;
    for (; url[i]; ++i) {
        path_to_doc[index] = url[i];
        index++;
    }
    arr[0] = mx_string_copy(host_name);
    arr[1] = mx_string_copy(path_to_doc);
    return arr;
}



/* @ Receives response from server and prints it on UART.
 * Receiving response and printing are done in one function
 * because of lack of memmory on esp32.
 */
static void receive_response(int sock) {
    char rx_buffer[5000];
    bzero(rx_buffer, 5000);

    char *headers = "HEADERS:" ;
    char *lines   = "----------------------";
    char *payload = "PAYLOAD:";
    char *color     = YELLOW_TEXT;

    bool print_headers = true;
    bool print_payload = true;

    while (recv(sock, rx_buffer, 4449, 0)) {
        if (print_headers == true) {
            uart_print(headers, 1, 0, color);
            uart_print(lines,   1, 1, color);
            print_headers = false;
        }
        for (int i = 0; rx_buffer[i]; ++i) {
            if (rx_buffer[i] == '<' && print_payload == true) {
                color = GREEN_TEXT;
                uart_print(payload, 1, 0, color);
                uart_print(lines,   1, 1, color);
                print_payload = false;
            }
            char tmp = rx_buffer[i];
            uart_print(&tmp, 0, 0, color);
            if (tmp == '\n') {
                uart_print("\r", 0, 0, color);
            }
        }
        bzero(rx_buffer, 5000);
    }
    uart_print("\n\r", 0, 0, NULL);
}



static int inline http_get_syntax_validate(int cmd_len) {
    if (cmd_len != 2) {
        uart_print(HTTP_GET_WRONG_SYNTAX, 0, 1, RED_TEXT);
        return 1;
    }
    return 0;
}



void http_get_command(char **cmd, int len) {
    // syntax validation
    if (http_get_syntax_validate(len)) {return;}
    char **splited_url = split_url(cmd[1]);
    char *ip_adress    = resolve_ip_by_host_name(splited_url[0]);

    if (ip_adress != NULL) {
        int sock = create_connected_socket(ip_adress, 80);
        if (sock != -1) {
            send_http_get(sock, splited_url[1]);
            receive_response(sock);
            // close connection with server
            shutdown(sock, 0);
            close(sock);
        }
    }

    // freeing memory.
    for (int i = 0; splited_url[i] ; ++i) {
        free(splited_url[i]);
    }
    free(splited_url);
    if (ip_adress != NULL) {
        free(ip_adress);
    }
}


