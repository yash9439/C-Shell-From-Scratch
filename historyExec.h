#ifndef _HISTORYEXEC_H
#define _HISTORYEXEC_H

#include "headers.h"

void historyExec(const char *command);
void parseCommands_history(char *input);
void tokenizeCommand_history(char *command);
void cd_call_V2_history();

#endif