#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char *mx_string_copy(char *str);
char *mx_strnew(int size);
char **mx_strarr_new(int size);
char *mx_upper_to_lower(char *str);
int  mx_strarr_len(char **arr);
char **mx_strsplit(const char *str, char c);
int mx_count_words(const char *str, char c);
int mx_count_char(char *str, char c);
int mx_char_in_str(char *str, char c);
char **mx_strarr_copy(char **arr);