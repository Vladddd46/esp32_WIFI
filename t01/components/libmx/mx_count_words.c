#include "libmx.h"



int mx_count_words(const char *str, char c) {
    int count = 0;

    if (str != NULL) {
        while(*str != '\0') {
            if (*str != c) {
                count++;
                while(*str != c && *str != '\0') {
                    str++;
                }
            } 
            else {
                str++;
            }
        }
    } 
    else {
        count = -1;
    }

    return count;
}
