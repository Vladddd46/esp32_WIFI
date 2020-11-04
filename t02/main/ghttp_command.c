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

static void inline uart_print(char *msg, bool newline, char *color) {
    if (color != NULL) {
        uart_write_bytes(UART_PORT, color, strlen(color)); 
    }
    if (newline) { 
        uart_write_bytes(UART_PORT, "\r\n", 2);
    }

    uart_write_bytes(UART_PORT, msg, strlen(msg));  

    if (newline) { 
        uart_write_bytes(UART_PORT, "\r\n", 2);
    }

    if (color != NULL) {
        uart_write_bytes(UART_PORT, RESET_COLOR, strlen(RESET_COLOR)); 
    }
}



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
	return NULL;
}

void send_http_get(char *path, char *ip) {
	struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_addr.s_addr = inet_addr("10.111.1.6");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(5000);

    int sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock < 0) {
    	printf("++++%d\n", errno);
    }
    else {
    	printf("==>%d\n", sock);
    }


    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
    	printf("error %d %d\n", errno, err);
    }
    else {
   		printf("socket successfully created\n");
   	}

   	char *p = "GET /\nHost: google.com\n";
   	send(sock, p, strlen(p), 0);
   	char rx_buffer[2000];
   	bzero(rx_buffer, 2000);


   	int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
   	printf(">>%s\n", rx_buffer);

   shutdown(sock, 0);
   close(sock);
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

void ghttp_command(char **cmd, int len) {
	if (len != 2) {
		uart_print(GHTTP_WRONG_SYNTAX, 1, RED_TEXT);
		return;
	}
	char **splited_url = split_url(cmd[1]);
    char *ip_adress = resolve_ip_by_host_name(splited_url[0]);

    if (ip_adress != NULL) {
    	uart_print(ip_adress,    1, GREEN_TEXT);
    }
    else {
		uart_print(NO_SUCH_HOST, 1, RED_TEXT);
	}


	send_http_get(splited_url[1], ip_adress);

	// freeing memory.
	for (int i = 0; splited_url[i] ; ++i) {
		free(splited_url[i]);
	}
	free(splited_url);
	if (ip_adress != NULL) {
		free(ip_adress);
	}
}


