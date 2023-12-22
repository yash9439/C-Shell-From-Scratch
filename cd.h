#ifndef __CD_H
#define __CD_H

#include "headers.h"

void cd_tokenizeCommand(char *string, char (*command_args)[MAX_COMMAND_LENGTH], int *num_tokens);
void removeBackslash(char *str);
void cd_tokenizeCommand_V2(char *string, char (*command_args)[MAX_COMMAND_LENGTH], int *num_tokens);
int cd_call(char (*args)[MAX_COMMAND_LENGTH], char (*current_cwd_path)[MAX_COMMAND_LENGTH], char (*initial_cwd_path)[MAX_COMMAND_LENGTH], char (*last_cwd_path)[MAX_COMMAND_LENGTH], char *curr_cwd, char *initial_cwd, char *last_cwd, int *current_cwd_path_dept, int *last_cwd_path_dept, int num);
void cd_call_V2(char *string);


#endif