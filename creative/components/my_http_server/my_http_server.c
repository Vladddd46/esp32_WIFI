#include "my_http_server.h"




static char *tag_wrapper(char *str, char *tag) {
    char res[100];
    bzero(res, 100);

    sprintf(res, "<%s>%s</%s>", tag, str, tag);
    char *result = mx_string_copy(res);
    return result;
}

static char *form_wrapper(char *ssid_name) {
    char res[300];
    bzero(res, 300);

    sprintf(res, "<form name=\"test\" method=\"post\"><p>%s<input type=\"text\" size=\"40\" placeholder=\"password\">\
            <input type=\"submit\" value=\"Connect\"></p></form>", ssid_name);
    char *result = mx_string_copy(res);
    return result;
}



esp_err_t get_handler(httpd_req_t *req) {
    char response[800];
    bzero(response, 800);
    
    printf("scanning\n");
    char **avaliable_networks = scan_wifi_networks();
    printf("end scann\n");
    
    int index = 0;
    for (int i = 0; avaliable_networks[i]; ++i) {
        printf("1\n");
        char *wrapped_str = form_wrapper(avaliable_networks[i]);
        printf("2\n");
        for (int j = 0; wrapped_str[j]; ++j) {
           response[index] = wrapped_str[j];
           index++;
        }
        free(wrapped_str);
    }
    printf("123456789\n");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    printf("hello world\n");
    // freeing memmory.
    for (int i = 0; avaliable_networks[i] ; ++i) {
        free(avaliable_networks[i]);
    }
    printf("yes\n");
    free(avaliable_networks);
    return ESP_OK;
}



esp_err_t post_handler(httpd_req_t *req) {
    char content[100];
    size_t recv_size = sizeof(content);

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
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



httpd_handle_t http_server_init() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }

    return server;
}
