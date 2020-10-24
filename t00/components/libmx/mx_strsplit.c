#include "libmx.h"



static int strlen_mod(const char *s, char c) {
    int count = 0;

    while (*s != c && *s != '\0') {
        count++;
        s++;
    }
    return count;
}



static int mx_count_words(const char *str, char c) {
    int count = 0;

    if (str != NULL) {
        while(*str != '\0') {
            if (*str != c) {
                count++;
                while(*str != c && *str != '\0') {
                    str++;
                }
            } else {
                str++;
            }
        }
    } else {
        count = -1;
    }

    return count;
}



char **mx_strsplit(const char *str, char c) {
    int amount_words = 0;
    int j = 0;
    char **arr = NULL;

    if (str != NULL) {
        amount_words = mx_count_words(str, c);
        arr = malloc((amount_words + 1) * sizeof(char *));
        if (arr != NULL) {
            for (int i = 0; i < amount_words; i++) {
                j = 0;
                while (*str == c && *str != '\0') {
                    str++;
                }
                arr[i] = mx_strnew(strlen_mod(str, c));
                while (*str != c && *str != '\0') {
                    arr[i][j] = *str;
                    str++;
                    j++;
                }
            }
            arr[amount_words] = NULL;
        }
    }
    return arr;
}
