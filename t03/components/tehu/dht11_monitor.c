#include "tehu.h"
  

/* @ Sends dht11 data to server with ip and port, received from queue.
 * Received data from queue.
 * If received data == "stop" => stops sending data to server.
 * If received data == "ip port" => start sending dht11_data 
 *  to server with ip and port.
 */ 


#define DHT11_DATA  4
#define DHT11_POWER 2
#define QUEUE_ERROR "Error with retreiving data from queue."



/* @ Creates HTTP Post packet.
 *   1. gets mac address of device.
 *   2. Splits dht11_data by space. split_data[0] = temperature; split_data[1] = humidity.
 *   3. Converts mac address, temperature, humidity in json format:
 *      {"id": mac_address, "t": temperature, "h": humidity}
 *   4. frees all malloced memmory and returns json data.
 */
static char *packet_former(char *dht11_data, char *domain, char *path) {
    char *mac_address = get_mac_address();
    if (mac_address == NULL) {return NULL;}
    char **split_data = mx_strsplit(dht11_data, ' ');

    char json_data[100];
    bzero(json_data, 100);
    sprintf(json_data, "{\"id\":\"%s\", \"t\":\"%s\", \"h\":\"%s\"}", mac_address, 
                                                                      split_data[0], 
                                                                      split_data[1]);
    int content_len = strlen((char *)json_data);

    char res[500];
    bzero(res, 500);
    if (path == NULL) {
        sprintf(res, "POST / HTTP/1.0\r\nHost: %s\r\nContent-Type: \
                  application/json\r\nContent-Length: %d\r\n\r\n%s",
                 domain, content_len, json_data);
    }
    else {
        sprintf(res, "POST /%s HTTP/1.0\r\nHost: %s\r\nContent-Type: \
                  application/json\r\nContent-Length: %d\r\n\r\n%s",
                 path, domain, content_len, json_data);
    }

    char *ret = mx_string_copy((char *)res);
    for (int i = 0; split_data[i]; ++i) {
        free(split_data[i]);
    }
    free(split_data);
    free(mac_address);
    return ret;
}



static esp_err_t tls_send(char *payload, char *host_domain) {
    static char errortext[256];
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_init(&ssl);
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    int ret = 0;
    char *pers = "ssl_client1";
    const char *port = "443";

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char*)pers,
                                     strlen(pers))) != 0) {
        ESP_LOGE("error: ", "mbedtls_ctr_drbg_seed failed %d\n", ret);
        return (ESP_FAIL);
    }

    if ((ret = mbedtls_net_connect(&server_fd, host_domain, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        ESP_LOGE("error: ", "mbedtls_net_connect failed %d\n", ret);
        return (ESP_FAIL);
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE("error: ", "mbedtls_ssl_config_defaults failed %d\n", ret);
    }
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    ret = mbedtls_ssl_setup(&ssl, &conf);
    if (ret != 0) {
        mbedtls_strerror(ret, errortext, sizeof(errortext));
        ESP_LOGE("error: ", "error from mbedtls_ssl_setup: %d - %x - %s\n", ret,
                 ret, errortext);
    }

    if ((ret = mbedtls_ssl_set_hostname(&ssl, host_domain)) != 0) {
        ESP_LOGE("error: ", "mbedtls_ssl_set_hostname failed %d\n", ret);
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
    unsigned char rx_buffer[1024];
    bzero(rx_buffer, 1024);
    unsigned char pay_buff[1024];
    char *p = (char*)pay_buff;
    memset(pay_buff, 0, 1024);
    memcpy(pay_buff, payload, strlen(payload));
    // printf("$pay_buff %s$\n", pay_buff);
    esp_err_t err = mbedtls_ssl_write(&ssl, pay_buff, strlen(p));

    if (err < 0) {
        ESP_LOGE("error: ", "Error occurred during sending: errno %d", errno);
    }
    else {
        int32_t len = mbedtls_ssl_read(&ssl, rx_buffer, sizeof(rx_buffer) - 1);
        if (len < 0) {
            ESP_LOGE("error: ", "recv failed: errno %d", errno);
        }
        else {
            printf(">>%s\n", rx_buffer);
        }
    }

    // freeing all memmory.
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    return ESP_OK;
}



/* @ Gets data from dht11 and sends it to server with ip and port.
 *  1. Creates socket with ip, port.
 *  2. Gets data from dht11.
 *  3. Converts data into json format.
 *  4. Sends data.
 *  5. Closes socket.
 */
static bool send_dht11_data_to_server(char *host_name, int port) {
    if (host_name == NULL || port == -1) {return false;}


    char *domain = NULL;
    char *path   = NULL;
    char *tmp = mx_string_copy(host_name);
    if (mx_count_char(tmp, '/')) {
        domain = strtok(tmp, "/");
        path =  strtok(NULL, " ");
    }
    else {
        domain = tmp;
    }

    char *dht11_data = get_dht11_data(DHT11_POWER, DHT11_DATA); 
    if (dht11_data != NULL) {
        char *payload = packet_former(dht11_data, domain, path);
        tls_send(payload, domain);
        if (payload != NULL) {free(payload);}
    }
    free(tmp);
    return true;
}



/*  @ Gets data from dht11 sensor and sends it to server.
 *  Sends only if is_send == true.
 */
void dht11_monitor() {
    dht11_data_queue = xQueueCreate(1, 100);
    char queue_data[100];
    bzero(queue_data, 100);

    bool is_send = false;

    char *host_name  = NULL;
    int  port_to_send = -1;
    char *tmp_port;
    while(true) {
        if (xQueueReceive(dht11_data_queue, (void *)queue_data, (TickType_t)10)) {
            if (!strcmp(queue_data, "stop")) {
                is_send = false;
            }
            else {
                host_name = strtok(queue_data, " ");
                if (host_name == NULL) {
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
            if (send_dht11_data_to_server(host_name, port_to_send) == false) {
                is_send = false;
            }
        }
        vTaskDelay(350);
    }
}


