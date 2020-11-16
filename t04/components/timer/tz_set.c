#include "timer.h"

/* @ Sets time-zone.
 * Syntax: tz_set time_zone(GMT)
 */


#define TZ_SET_INVALID_SYNTAX   "Wrong syntax: tz_set time-zone"
#define TZ_SET_INVALID_TIMEZONE "Invalid time-zone type. Use:"
#define TZ_SET_SUCCESS			"Time-zone successfully set."
#define TZ_SET_SUCCESS_MSG		"Soon clock will update if device is connected to the Internet."


static void inline invalid_arg_msg() {
	uart_print(TZ_SET_INVALID_TIMEZONE, 0, 1, RED_TEXT);

	char *tz_types[31] = {"GMT",    "GMT+0",  "GMT+1", 
						   "GMT+10", "GMT+11", "GMT+12", 
						   "GMT+2",  "GMT+3",  "GMT+4", 
						   "GMT+5",  "GMT+6",  "GMT+7", 
						   "GMT+8",  "GMT+9",  "GMT-0", 
						   "GMT-1",  "GMT-10", "GMT-11", 
						   "GMT-12", "GMT-13", "GMT-14", 
						   "GMT-2",  "GMT-3",  "GMT-4", 
						   "GMT-5",  "GMT-6",  "GMT-7", 
						   "GMT-8",  "GMT-9",  "GMT0", NULL};
	char msg[50];
	for (int i = 0; tz_types[i]; i+=3) {
		bzero(msg, 50);
		sprintf(msg, "%s, %s, %s", tz_types[i], tz_types[i+1], tz_types[i+2]);
		uart_print(msg, 0, 1, GREEN_TEXT);
	}
}



static bool arg_validate(char *tz_arg) {
	char *tz_types[31] = {"GMT",    "GMT+0",  "GMT+1", 
						   "GMT+10", "GMT+11", "GMT+12", 
						   "GMT+2",  "GMT+3",  "GMT+4", 
						   "GMT+5",  "GMT+6",  "GMT+7", 
						   "GMT+8",  "GMT+9",  "GMT-0", 
						   "GMT-1",  "GMT-10", "GMT-11", 
						   "GMT-12", "GMT-13", "GMT-14", 
						   "GMT-2",  "GMT-3",  "GMT-4", 
						   "GMT-5",  "GMT-6",  "GMT-7", 
						   "GMT-8",  "GMT-9",  "GMT0", NULL};
	bool found = false;
	for (int i = 0; tz_types[i]; ++i) {
		if (!strcmp(tz_types[i], tz_arg)) {
			found = true;
			break;
		}
	}
	return found;
}

/*
 * Saves time-zone in
 *  non-volatile storage.
 */
static void inline write_tz_in_nvc(char *tz) {
    nvs_handle_t my_handle;
    esp_err_t err;
    char error_msg[100];
    bzero(error_msg, 100);

    err = nvs_open(TIMEZONE_STORAGE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while opening nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }
    err = nvs_set_str(my_handle, TIMEZONE_STORAGE, tz);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while writing in nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }
    nvs_close(my_handle);
}


void init_tz() {
	nvs_handle_t my_handle;
    char error_msg[100];
    bzero(error_msg, 100);
    esp_err_t err = nvs_open(TIMEZONE_STORAGE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        sprintf(error_msg, "Error while opening nvc: %d", err);
        uart_print(error_msg, 0, 1, RED_TEXT);
        return;
    }

	size_t required_size;
	nvs_get_str(my_handle, TIMEZONE_STORAGE, NULL, &required_size);
	if (required_size == 0) {
		printf("no var\n");
		return;
	}
	char *tz = mx_strnew(required_size);
	char tz_static[20];
	bzero(tz_static, 20);
	sprintf(tz_static, "%s", tz);
    nvs_get_str(my_handle, TIMEZONE_STORAGE, tz_static, &required_size);
    setenv("TZ", tz_static, 1);
	tzset();
	free(tz);
}


void tz_set(char **cmd, int len) {
	if (len != 2) {
		uart_print(TZ_SET_INVALID_SYNTAX, 0, 1, RED_TEXT);
		return;
	}
	if (arg_validate(cmd[1]) == false) {
		invalid_arg_msg();
		return;
	}
	setenv("TZ", cmd[1], 1);
	tzset();
	uart_print(TZ_SET_SUCCESS,     0, 1, GREEN_TEXT);
	uart_print(TZ_SET_SUCCESS_MSG, 0, 1, GREEN_TEXT);
	write_tz_in_nvc(cmd[1]);
}




