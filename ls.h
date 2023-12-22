#ifndef _LS_H
#define _LS_H

#include "headers.h"

int compare_names(const void *a, const void *b);
void replaceSubstring(char *string, const char *substring, const char *replacement);
void ls_get_args(const char *str);
int compareStrings(const void *a, const void *b);
void count_total(char *input, int *total);
void listFilesInDirectory(const char *path, int showHidden, int showDetails);
void ls_tokenizeCommand(char *string, char (*command_args)[MAX_COMMAND_LENGTH], int *num_tokens);
void ls_call_V2(int flag_a, int flag_l);
void ls_call();
void printColoredText(const char *text, const char *colorCode);


#endif