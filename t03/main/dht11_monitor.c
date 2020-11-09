#include "header.h"


#define DHT11_DATA  4
#define DHT11_POWER 2
#define QUEUE_ERROR "Error with retreiving data from queue."

static char *dht11_data_to_json(char *data) {
    char **split_data = mx_strsplit(data, ' ');

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
    char res[100];
    bzero(res, 100);
    sprintf(res, "{\"id\":\"%s\", \"t\":\"%s\", \"h\":\"%s\"}\n\r\n\r", mac_address, split_data[0], split_data[1]);

    char *ret = mx_string_copy((char *)res);

    for (int i = 0; split_data[i]; ++i) {
        free(split_data[i]);
    }
    free(split_data);
    return ret;
}



/*
 * Gets data from  dht11 temperature/humidity sensor each 5 seconds.
 * Sends got data into dht11_data_queue.
 */
void dht11_monitor() {s
    dht11_data_queue = xQueueCreate(1, 100);
    char queue_data[100];
    bzero(queue_data, 100);

    bool is_send = false;
    char *data = NULL;
    // char data_to_send[100];


    char *ip_to_send;
    int  port_to_send;
    char *tmp_port;

    int socket = -1;
    while(true) {
        // bzero(data_to_send, 100);
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
            socket = create_connected_socket(ip_to_send, port_to_send);
            if (socket == -1) {
                is_send = false;
                continue;
            }

            data = get_dht11_data(DHT11_POWER, DHT11_DATA); 
            if (data != NULL) {
                char *send_data = dht11_data_to_json(data);
                // sprintf(data_to_send, "%s\n\r\n\r", data);
                if (send_data != NULL) {
                    send(socket, send_data, strlen(send_data), 0);
                    free(send_data);
                }
            }

            if (data != NULL) {
        	   free(data);
            }

            if (socket != -1) {
                close(socket);
            }
        }
        vTaskDelay(350);
    }
}


