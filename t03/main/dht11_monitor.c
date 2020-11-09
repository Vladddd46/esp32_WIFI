#include "header.h"

/* @ Sends dht11 data to server with ip and port, received from queue.
 * Received data from queue.
 * If received data == "stop" => stops sending data to server.
 * If received data == "ip port" => start sending dht11_data 
 *  to server with ip and port.
 */ 

#define DHT11_DATA  4
#define DHT11_POWER 2
#define QUEUE_ERROR "Error with retreiving data from queue."



// Returns mac address of esp32 or null in case of error.
static char *get_mac_address(void) {
    wifi_ap_record_t ap_info;
    esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);
    if (err != ESP_OK) {
        uart_print("ESP32 is not connected to WIFI", 0, 1, RED_TEXT);
        return NULL;
    }
    char mac_address[50];
    bzero(mac_address, 50);
    sprintf(mac_address, "%x:%x:%x:%x:%x:%x", ap_info.bssid[0], 
                                              ap_info.bssid[1], 
                                              ap_info.bssid[2], 
                                              ap_info.bssid[3],
                                              ap_info.bssid[4], 
                                              ap_info.bssid[5]);
    char *res = mx_string_copy(mac_address);
    return res;
}

/* @ Converts received data from dht11 to json format.
 *   1. gets mac address of device.
 *   2. Splits data by space. split_data[0] = temperature; split_data[1] = humidity.
 *   3. Converts mac address, temperature, humidity in json format:
 *      {"id": mac_address, "t": temperature, "h": humidity}
 *   4. frees all malloced memmory and returns json data.
 */
static char *dht11_data_to_json(char *data) {
    char *mac_address = get_mac_address();
    if (mac_address == NULL) {return NULL;}
    char **split_data = mx_strsplit(data, ' ');
    char res[100];
    bzero(res, 100);
    sprintf(res, "{\"id\":\"%s\", \"t\":\"%s\", \"h\":\"%s\"}\n\r\n\r", mac_address, 
                                                                        split_data[0], 
                                                                        split_data[1]);
    char *ret = mx_string_copy((char *)res);

    for (int i = 0; split_data[i]; ++i) {
        free(split_data[i]);
    }
    free(split_data);
    free(mac_address);
    return ret;
}



// static int send_dht11_data_to_server(char *ip, int port) {
//     int sock = -1;

// }


/*
 * Gets data from  dht11 temperature/humidity sensor each 5 seconds.
 * Sends got data into dht11_data_queue.
 */
void dht11_monitor() {
    dht11_data_queue = xQueueCreate(1, 100);
    char queue_data[100];
    bzero(queue_data, 100);

    bool is_send = false;
    char *data = NULL;

    char *ip_to_send;
    int  port_to_send;
    char *tmp_port;

    int sock = -1;
    while(true) {
        if (xQueueReceive(dht11_data_queue, (void *)queue_data, (TickType_t)10)) {
            if (!strcmp(queue_data, "stop")) {
                is_send = false;
            }
            else {
                ip_to_send     = strtok(queue_data, " ");
                if (ip_to_send == NULL) {
                    uart_print(QUEUE_ERROR, 1, 1, RED_TEXT);
                    continue;
                }
                tmp_port = strtok(NULL, " ");
                if (tmp_port == NULL) {
                    uart_print(QUEUE_ERROR, 1, 1, RED_TEXT);
                    continue;
                }
                port_to_send = atoi(tmp_port);
                is_send      = true;
            }
        }

        if (is_send == true) {
            sock = create_connected_socket(ip_to_send, port_to_send);
            if (sock == -1) {
                is_send = false;
                continue;
            }

            data = get_dht11_data(DHT11_POWER, DHT11_DATA); 
            if (data != NULL) {
                char *send_data = dht11_data_to_json(data);
                if (send_data != NULL) {
                    send(sock, send_data, strlen(send_data), 0);
                    free(send_data);
                }
            }

            if (data != NULL) {
        	   free(data);
            }

            if (sock != -1) {
                close(sock);
            }
        }
        vTaskDelay(350);
    }
}


