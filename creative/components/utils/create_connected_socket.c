#include "utils.h"



// Creates socket, connected to `ip` with `port`
int create_connected_socket(char *ip, int port) {
    struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_port        = htons(port);

    int sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        uart_print("Unable to create socket...", 1, 1, RED_TEXT);
        printf("Socket was not created. Errno: %d\n", errno);
        return -1;
    }

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        uart_print("Host is unavailable", 1, 1, RED_TEXT);
        return -1;
    }
    return sock;
}
