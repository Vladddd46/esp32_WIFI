#include "header.h"
#include "nvs_flash.h"

/* NVC guide.
 * 1. nvs_flash_init();
 * 2. nvs_flash_erase(); - delete all data from nvs.
 * 3.  nvs_handle_t my_handle;
 * 3.1 nvs_open("name_space_name", NVS_READWRITE, &my_handle);
 * 4. nvs_set_str(my_handle, "key", "valueX"); -set key:value data
 * 5. 
 *     size_t required_size;
 *     nvs_get_str(my_handle, "key", NULL, &required_size);
 *     char* buff = malloc(required_size);
 *	   nvs_get_str(my_handle, "key", buff, &required_size); -
 *     - sets buff to NULL in case of no value.
 * 6. nvs_close(my_handle);
 */




void inline global_variables_init() {
    global_input_queue = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
}



void app_main() {
    global_variables_init();
    uart_init(9600);

    esp_err_t err;
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NOT_FOUND) {
    	nvs_flash_erase();
    	nvs_flash_init();
    }

    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);
    // nvs_set_str(my_handle, "key", "valueX");
    // nvs_commit(my_handle);
   

    size_t required_size;
	nvs_get_str(my_handle, "key", NULL, &required_size);

	char* server_name = malloc(required_size);
	nvs_get_str(my_handle, "key", server_name, &required_size);

	if (server_name == NULL)
		printf("NULL\n");
	else 
		printf("%s\n",  server_name);

    // xTaskCreate(user_input,    "user_input",    12040, NULL, 10, NULL);
    // xTaskCreate(cmd_handler,   "cmd_handler",   12040, NULL, 10, NULL);
     nvs_close(my_handle);
}