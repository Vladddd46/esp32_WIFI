#include "libmx.h"


char **mx_strarr_copy(char **arr) {
	int len        = mx_strarr_len(arr);
	char **new_arr = mx_strarr_new(len);

	int i = 0;
	while(arr[i]) {
		new_arr[i] = mx_string_copy(arr[i]);
		i++;
	}
	return new_arr;
}