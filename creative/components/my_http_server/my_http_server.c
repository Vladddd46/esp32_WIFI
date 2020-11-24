#include "my_http_server.h"



// @ Taskes SSID and wrapps it in html from.
static char *form_wrapper(char *ssid_name) {
    char res[400];
    bzero(res, 400);

    sprintf(res, "<form enctype=\"text/plain\" action=\"/\" method=\"post\">\
                    <p>%s <input name=\"%s\" value=\"\" type=\"text\" size=\"40\" placeholder=\"password\">\
                        <input type=\"submit\">\
                    </p>\
                  </form>", ssid_name, ssid_name);
    char *result = mx_string_copy(res);
    return result;
}



/* @Handles http get request URL: /
 *  1. Scans all available wifi networks. 
 *  2. Froms html response, where there are forms 
 *     for selecting wifi ssid and entering password.
 *  3. Sends response.
 */
esp_err_t get_handler(httpd_req_t *req) {
    char response[10000];
    bzero(response, 10000);
    
    printf("scanning wifi_networks...\n");
    char **avaliable_networks = scan_wifi_networks();
    printf("scanning end\n");
    
    // response formation.
    char *wrapped_str;
    int index = 0;
    for (int i = 0; avaliable_networks[i]; ++i) {
        wrapped_str = form_wrapper(avaliable_networks[i]);
        for (int j = 0; wrapped_str[j]; ++j) {
           response[index] = wrapped_str[j];
           index++;
        }
        free(wrapped_str);
    }

    // sending response.
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    // freeing memmory.
    for (int i = 0; avaliable_networks[i]; ++i) {free(avaliable_networks[i]);}
    free(avaliable_networks);
    return ESP_OK;
}



/* @ Received data, user entered to connect to wifi.
 * Received data structure:
 *  wifi_ssid=wifi_password
 *
 */
esp_err_t post_handler(httpd_req_t *req) {
    char content[100];
    bzero(content, 100);
    size_t recv_size = sizeof(content);
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    char *msg = "OK";
    httpd_resp_send(req, msg, strlen(msg));
    
    char **splt_data = mx_strsplit(content, '=');
    char *wifi_ssid  = mx_string_copy(splt_data[0]);
    char *wifi_pass  = mx_string_copy(splt_data[1]);

    for (int i = 0; splt_data[i]; ++i) {
        free(splt_data[i]);
    }
    free(splt_data);

    printf("%s with pass %s\n", wifi_ssid, wifi_pass);
    return ESP_OK;
}



httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL
};



httpd_uri_t uri_post = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = post_handler,
    .user_ctx = NULL
};


// Initialize simple http server.
httpd_handle_t http_server_init() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 80000;
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    return server;
}
