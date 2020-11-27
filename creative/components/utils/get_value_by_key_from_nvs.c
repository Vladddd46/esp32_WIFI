#include "utils.h"



char *get_value_by_key_from_nvs(char *nvs_page, char *key) {
    nvs_handle_t my_handle;
    nvs_open(nvs_page, NVS_READWRITE, &my_handle);
    size_t required_size = 0;
    nvs_get_str(my_handle, key, NULL, &required_size);
    if (required_size <= 0) {
        nvs_close(my_handle);
        return NULL;
    }
    char *value = mx_strnew(required_size);
    nvs_get_str(my_handle, key, value, &required_size);
    nvs_close(my_handle);
    return value;
}
