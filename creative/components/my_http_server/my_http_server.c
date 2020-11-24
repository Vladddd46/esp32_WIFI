#include "my_http_server.h"




// static char *tag_wrapper(char *str, char *tag) {
//     char res[100];
//     bzero(res, 100);

//     sprintf(res, "<%s>%s</%s>", tag, str, tag);
//     char *result = mx_string_copy(res);
//     return result;
// }

static char *form_wrapper(char *ssid_name, int id) {
    char res[700];
    bzero(res, 700);

    sprintf(res, "<form enctype=\"text/plain\" action=\"/\" method=\"post\"><p>%s<input name=\"%s\" value=\"\" type=\"text\" size=\"40\" placeholder=\"password\">\
                        <input type=\"submit\"></p></form>", ssid_name, ssid_name);
    char *result = mx_string_copy(res);
    return result;
}



esp_err_t get_handler(httpd_req_t *req) {
    char response[10000];
    bzero(response, 10000);
    
    printf("scanning\n");
    char **avaliable_networks = scan_wifi_networks();
    printf("end scann\n");
    
    int id = 0;
    int index = 0;
    for (int i = 0; avaliable_networks[i]; ++i) {
        char *wrapped_str = form_wrapper(avaliable_networks[i], id);
        for (int j = 0; wrapped_str[j]; ++j) {
           response[index] = wrapped_str[j];
           index++;
        }
        free(wrapped_str);
        id++;
    }
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    // freeing memmory.
    for (int i = 0; avaliable_networks[i] ; ++i) {
        free(avaliable_networks[i]);
    }
    free(avaliable_networks);
    return ESP_OK;
}



esp_err_t post_handler(httpd_req_t *req) {
    printf("123\n");
    char content[10000];
    bzero(content, 10000);
    size_t recv_size = sizeof(content);

    printf("2\n");
    int ret = httpd_req_recv(req, content, recv_size);
    printf("3\n");
    if (ret <= 0) {
        printf("%d\n", ret);
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            printf("ашибка\n");
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    printf(">>>>>>>%s\n", content);
    const char resp[] = "URI POST Response";
    printf("6\n");
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
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
    config.stack_size = 80000;
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }

    while(1) {
        vTaskDelay(1);
    }

    return server;
}
