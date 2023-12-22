#ifndef _HISTORY_H
#define _HISTORY_H

#include "headers.h"

void updateHiddenFile(const char *filename, const char *input);
void printHiddenFile(const char *filename);
void deleteFirstLine(const char *filename);
void historyStore(char *string);
void removeNewline(char *str);

#endif