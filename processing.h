#ifndef __PROCESSING_H
#define __PROCESSING_H

#include "headers.h"

void splitString(char *input, char (*output)[MAX_COMMAND_LENGTH], int *index);
void strtrim(char *str);
void cleanCommand(char *command);
void parseCommands(char *input);
void tokenizeCommand(char *command);
int stringToNumber(const char *str);


#endif