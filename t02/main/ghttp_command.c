#include "header.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include <sys/types.h>
#include <sys/socket.h>


/* @Sends http GET request.
 * Resolves url with help of dns.
 * Forms http get packet.
 * Sends packet and receives response.
 */

#define GHTTP_WRONG_SYNTAX "Wrong syntax: ghttp url/must/be/here"
#define NO_SUCH_HOST 	   "Host not found"



static void dns_found(char *name, ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr == NULL) {
        DNSFound = false;
    }
    else {
	    ip_Addr = *ipaddr;
	    DNSFound = true;
    }
}


/*
 * Takes host_name as argument and returns
 * ip address of this host.
 * In case of no ip associated with such host, return NULL.
 */
static char *resolve_ip_by_host_name(char *host_name) {
	dns_gethostbyname(host_name, &ip_Addr, dns_found, NULL);
	int num_of_ms = 0;
    while(!DNSFound) {
        if (num_of_ms == 100) {
            break;
        }
        num_of_ms +=1;
        vTaskDelay(1);
    }

    char ip_adress[32];
    bzero(ip_adress, 32);
    char *res;
    if (DNSFound) {
		sprintf(ip_adress, "%i.%i.%i.%i", 
                ip4_addr1(&ip_Addr.u_addr.ip4), 
                ip4_addr2(&ip_Addr.u_addr.ip4), 
                ip4_addr3(&ip_Addr.u_addr.ip4), 
                ip4_addr4(&ip_Addr.u_addr.ip4));
		res = mx_string_copy(ip_adress);
		DNSFound = false;
		return res;
	}
	DNSFound = false;
    uart_print(NO_SUCH_HOST, 0, 1, RED_TEXT);
	return NULL;
}


// Sends simple http request.
static void inline send_http_get(int sock, char *path) {
    char get_request[1000];
    bzero(get_request, 1000);
    sprintf(get_request, "GET /%s HTTP/1.0\r\n\r\n", path);
   	send(sock, get_request, strlen(get_request), 0);
}



// Creates socket, connected to `ip` with `port`
static int create_connected_socket(char *ip, int port) {
    struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

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



static char *receive_response(int sock) {
    char rx_buffer[500];
    bzero(rx_buffer, 500);
    recv(sock, rx_buffer, 200, 0);
    uart_print(rx_buffer, 1, 1, GREEN_TEXT);
    return NULL;
}

static int inline http_get_syntax_validate(int cmd_len) {
    if (cmd_len != 2) {
        uart_print(GHTTP_WRONG_SYNTAX, 1, 0, RED_TEXT);
        return 1;
    }
    return 0;
}

void ghttp_command(char **cmd, int len) {
    // syntax validation
	if (http_get_syntax_validate(len)) {return;}

	char **splited_url = split_url(cmd[1]);
    char *ip_adress    = resolve_ip_by_host_name(splited_url[0]);
    if (ip_adress == NULL) {return;}

    uart_print(ip_adress, 1, 0, GREEN_TEXT);
    
    int sock = create_connected_socket("10.111.1.12", 5000);
    if (sock == -1) {return;}

	send_http_get(sock, splited_url[1]);
    char *server_respone = receive_response(sock);

	// freeing memory.
	for (int i = 0; splited_url[i] ; ++i) {
		free(splited_url[i]);
	}
	free(splited_url);
	if (ip_adress != NULL) {
		free(ip_adress);
	}
    // close connection with server
    shutdown(sock, 0);
    close(sock);
}


