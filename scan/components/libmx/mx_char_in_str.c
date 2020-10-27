#include "libmx.h"


int mx_char_in_str(char *str, char c) {
	for (int i = 0; str[i]; ++i) {
		if (str[i] == c)
			return 1;
	}
	return 0;
}