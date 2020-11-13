#include "utils.h"

#define NO_SUCH_HOST       "Host not found"

/*
 * Takes host_name as argument and returns
 * ip address of this host.
 * In case of no ip associated with such host, returns NULL.
 */
char *resolve_ip_by_host_name(char *host_name) {
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
