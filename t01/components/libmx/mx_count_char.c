#include "libmx.h"


int mx_count_char(char *str, char c) {
	int counter = 0;
	
	for (int i = 0; str[i]; ++i) {
		if (str[i] == c) {
			counter += 1;
		}
	}
	return counter;
}