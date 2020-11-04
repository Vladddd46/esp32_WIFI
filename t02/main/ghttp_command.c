#include "header.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"

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



static void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr == NULL) {
        DNSFound = false;
    }
    else {
	    ip_Addr = *ipaddr;
	    DNSFound = true;
    }
}





void ghttp_command(char **cmd, int len) {
	if (len != 2) {
		uart_print(GHTTP_WRONG_SYNTAX, 1, RED_TEXT);
		return;
	}
	dns_gethostbyname(cmd[1], &ip_Addr, dns_found, NULL);

	int num_of_ms = 0;
    while(!DNSFound) {
        if (num_of_ms == 50) {
            break;
        }
        num_of_ms +=1;
        vTaskDelay(1);
    }

    char ip_adress[32];
    bzero(ip_adress, 32);

	if (DNSFound) {
		sprintf(ip_adress, "%i.%i.%i.%i", 
            ip4_addr1(&ip_Addr.u_addr.ip4), 
            ip4_addr2(&ip_Addr.u_addr.ip4), 
            ip4_addr3(&ip_Addr.u_addr.ip4), 
            ip4_addr4(&ip_Addr.u_addr.ip4));

		uart_print(ip_adress, 1, GREEN_TEXT);
	}
	else {
		uart_print(NO_SUCH_HOST, 1, RED_TEXT);
	}


}