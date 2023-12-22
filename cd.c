#include "cd.h"


void cd_tokenizeCommand(char *string, char (*command_args)[MAX_COMMAND_LENGTH], int *num_tokens)
{
    int temp_num_tokens = *num_tokens;
    char temp_current_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
    for (int i = 0; i < temp_num_tokens; i++)
    {
        strcpy(temp_current_cwd_path[i], command_args[i]);
    }

    *num_tokens = 0;
    char *token = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
    int token_index = 0;
    int in_quotes = 0;
    int escaped = 0;

    for (int i = 0; i < strlen(string); i++)
    {
        char current_char = string[i];

        if (current_char == ' ' && !in_quotes && !escaped)
        {
            token[token_index] = '\0';
            strcpy(command_args[*num_tokens], token);
            (*num_tokens)++;
            token_index = 0;
            escaped = 0;
        }
        else
        {
            if (current_char == '\"' && !escaped)
            {
                in_quotes = !in_quotes;
            }
            else if (current_char == '\\' && !escaped)
            {
                escaped = 1;
            }
            else
            {
                escaped = 0;
                token[token_index] = current_char;
                token_index++;
            }
        }
    }

    token[token_index] = '\0';
    strcpy(command_args[*num_tokens], token);
    (*num_tokens)++;
    if (in_quotes)
    {
        printf("Syntax Error\n");
        *num_tokens = temp_num_tokens;
        char temp_current_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
        for (int i = 0; i < temp_num_tokens; i++)
        {
            strcpy(command_args[i], temp_current_cwd_path[i]);
        }
    }
    return;
}

void removeBackslash(char *str)
{
    char *source = str;
    char *destination = str;

    while (*source)
    {
        if (*source != '\\')
        {
            *destination = *source;
            destination++;
        }
        source++;
    }

    *destination = '\0'; // Add null character at the end
}

void cd_tokenizeCommand_V2(char *string, char (*command_args)[MAX_COMMAND_LENGTH], int *num_tokens)
{
    // Check if the token contains both '"' and '\'
    if (strchr(string, '"') != NULL && strchr(string, '\\') != NULL)
    {
        printf("Path does not exist: %s\n", string);
        return;
    }

    // Remove starting spaces
    while (*string == ' ')
    {
        string++;
    }

    // Check if the string starts with "cd"
    if (strncmp(string, "cd", 2) == 0)
    {
        string += 2; // Skip "cd"

        // Remove the space after "cd" if it exists
        while (*string == ' ')
        {
            string++;
        }
    }

    // Check if the string starts with "warp"
    if (strncmp(string, "warp", 4) == 0)
    {
        string += 4; // Skip "cd"

        // Remove the space after "cd" if it exists
        while (*string == ' ')
        {
            string++;
        }
    }

    // Remove trailing spaces
    size_t length = strlen(string);
    while (length > 0 && string[length - 1] == ' ')
    {
        string[length - 1] = '\0';
        length--;
    }

    // \\ and \/ cases are invalid
    const char *doubleBackslash = "\\\\";
    const char *cone = "\\/";

    if (strstr(string, doubleBackslash) != NULL || strstr(string, cone))
    {
        printf("Invalid Path!!\n");
        return;
    }

    // Remove double quotes at the start and end
    if (length >= 2 && string[0] == '"' && string[length - 1] == '"')
    {
        string[length - 1] = '\0';
        string++;

        char *token = strtok(strdup(string), "/");
        numArgs = 1;
        strcpy(command_args[0], "cd");
        while (token != NULL)
        {
            strtrim(token);
            strcpy(command_args[numArgs++], token);
            token = strtok(NULL, "/");
        }

        return;
    }

    // For cd / and cd
    strtrim(string);
    if (strcmp(string, "/") == 0)
    {
        strcpy(prev_cwd, curr_cwd);
        strcpy(curr_cwd, "/");
        cd_done = 1;
        return;
    }
    removeBackslash(string);
    strtrim(string);
    if (strcmp(string, "") == 0)
    {
        strcpy(prev_cwd, curr_cwd);
        strcpy(curr_cwd, initial_cwd);
        cd_done = 1;
        return;
    }

    char *token = strtok(strdup(string), "/");
    numArgs = 1;
    strcpy(command_args[0], "cd");

    while (token != NULL)
    {
        strtrim(token);
        strcpy(command_args[numArgs++], token);
        token = strtok(NULL, "/");
    }
    if (debug)
    {
        printf("-----------------\n");
        printf("%s\n", string);
    }
    return;
}

int cd_call(char (*args)[MAX_COMMAND_LENGTH], char (*current_cwd_path)[MAX_COMMAND_LENGTH], char (*initial_cwd_path)[MAX_COMMAND_LENGTH], char (*last_cwd_path)[MAX_COMMAND_LENGTH], char *curr_cwd, char *initial_cwd, char *last_cwd, int *current_cwd_path_dept, int *last_cwd_path_dept, int num)
{
    // Extra Argument Error
    if (num > 2)
    {
        printf("Error, Extra Argument");
        return 1;
    }

    // Get to the home directory
    if (num == 1)
    {
        // Updating Last CWD Path String
        strcpy(prev_cwd, curr_cwd);

        // Updating Curr CWD Path String
        strcpy(curr_cwd, initial_cwd);

        // Updating the Current CWD Path and dept
        splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);

        // Updating the Last CWD Path and dept
        splitString(last_cwd, last_cwd_path, last_cwd_path_dept);

        return 0;
    }

    // Get to last directory
    if (strcmp(args[1], "-") == 0)
    {
        // Swapping the String of Current and prev CWD
        char temp[1024];
        strcpy(temp, curr_cwd);
        strcpy(curr_cwd, prev_cwd);
        strcpy(prev_cwd, temp);

        // Updating the Current CWD Path and dept
        splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);

        // Updating the Last CWD Path and dept
        splitString(last_cwd, last_cwd_path, last_cwd_path_dept);
        return 0;
    }

    // Go to home directory
    else if (strcmp(args[1], "~") == 0)
    {
        // Updating Last CWD Path String
        strcpy(prev_cwd, curr_cwd);

        // Updating the Current CWD String
        strcpy(curr_cwd, initial_cwd);

        // Updating the Current CWD Path and dept
        splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);

        // Updating the Last CWD Path and dept
        splitString(last_cwd, last_cwd_path, last_cwd_path_dept);

        return 0;
    }

    // Get to previous directory
    else if (strcmp(args[1], "..") == 0)
    {
        if (*current_cwd_path_dept > 0)
        {

            // Updating Last CWD Path String
            strcpy(prev_cwd, curr_cwd);

            // Updating the current CWD path
            strcpy(current_cwd_path[(*current_cwd_path_dept) - 1], "");

            // Updating the Current CWD Path Dept
            (*current_cwd_path_dept)--;

            // Updating the current CWD String
            strcpy(curr_cwd, "");
            for (int i = 0; i < *current_cwd_path_dept; i++)
            {
                strcat(curr_cwd, "/");
                strcat(curr_cwd, current_cwd_path[i]);
            }

            // Updating the Last CWD Path and dept
            splitString(last_cwd, last_cwd_path, last_cwd_path_dept);

            // printf("%d", *current_cwd_path_dept);
        }
    }

    // Change to current Directory : i.e. Do nothing
    else if (strcmp(args[1], ".") == 0)
    {
        // Mentos Zibdagi
    }

    // Go to that Directory (/home cases covered)
    else if (chdir(args[1]) == 0 && args[1][0] == '/')
    {
        // Updating Last CWD Path String
        strcpy(prev_cwd, curr_cwd);

        // Updating Current CWd Path String
        int len = strlen(args[1]); // Get the length of args[1]
        if (len > 0 && args[1][len - 1] == '/')
        {
            len--; // Ignore the trailing '/'
        }
        strcpy(curr_cwd, args[1]); // Copy the string
        curr_cwd[len] = '\0';      // Add null terminator at the appropriate position

        // Updating the Current CWD Path and dept
        splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);
        if (debug)
        {
            printf("Current CWD Path Dept : %d\n", *current_cwd_path_dept);
            printf("Current CWD  : %s\n", curr_cwd);
        }

        // Updating the Last CWD Path and dept
        splitString(last_cwd, last_cwd_path, last_cwd_path_dept);
    }
    else
    {
        // Converting the path to Absolute Path if its Relative
        if (args[1][0] != '/')
        {
            // ./VS\ Code
            if (args[1][0] == '.' && strlen(args[1]) > 1 && args[1][1] == '/')
            {
                // Checking if ./abcd exist by strcat and removing .
                char *newPath = malloc(strlen(curr_cwd) + strlen(args[1]) + 2);
                strcpy(newPath, curr_cwd);
                strcat(newPath, args[1] + 1);
                if (chdir(newPath) == 0)
                {

                    // Updating Last CWD Path String
                    strcpy(prev_cwd, curr_cwd);

                    // Updating Current CWD Path String
                    strcat(curr_cwd, args[1] + 1);
                    int len = strlen(args[1]); // Get the length of args[1]
                    if (len > 0 && args[1][len - 1] == '/')
                    {

                        len = strlen(curr_cwd);
                        len--;                // Ignore the trailing '/'
                        curr_cwd[len] = '\0'; // Add null terminator at the appropriate position
                    }

                    if (debug)
                    {
                        printf("Curr CWD = %s\n", curr_cwd);
                    }

                    // Updating the Current CWD Path and dept
                    splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);
                    if (debug)
                    {
                        printf("Current CWD Path Dept : %d\n", *current_cwd_path_dept);
                        printf("Current CWD  : %s\n", curr_cwd);
                    }

                    // Updating the Last CWD Path and dept
                    splitString(last_cwd, last_cwd_path, last_cwd_path_dept);
                }
                else
                {
                    printf("Path Doesn't Exist\n");
                }
            }
            // ../
            else if (args[1][0] == '.' && strlen(args[1]) > 2 && args[1][1] == '.')
            {
                // Checking if ../abcd exist by strcat and removing .
                char *newPath = malloc(strlen(curr_cwd) + strlen(args[1]) + 2);
                strcpy(newPath, curr_cwd);
                strcat(newPath, args[1] + 2);

                if (chdir(newPath) == 0)
                {
                    // current_cwd_path[current_cwd_path_dept-1] or current_cwd_path_dept--;
                    // If this path is trailing with /

                    // Updating Last CWD Path String
                    strcpy(prev_cwd, curr_cwd);

                    // Updating the Last CWD Path and dept
                    splitString(last_cwd, last_cwd_path, last_cwd_path_dept);

                    // Updating the current CWD path
                    strcpy(current_cwd_path[(*current_cwd_path_dept) - 1], "");

                    // Updating the Current CWD Path Dept
                    (*current_cwd_path_dept)--;

                    // Updating the current CWD String
                    strcpy(curr_cwd, "");
                    for (int i = 0; i < *current_cwd_path_dept; i++)
                    {
                        strcat(curr_cwd, "/");
                        strcat(curr_cwd, current_cwd_path[i]);
                    }

                    // Updating Current CWD Path String
                    strcat(curr_cwd, args[1] + 2);
                    int len = strlen(args[1]); // Get the length of args[1]
                    if (len > 0 && args[1][len - 1] == '/')
                    {
                        len = strlen(curr_cwd);
                        len--;                // Ignore the trailing '/'
                        curr_cwd[len] = '\0'; // Add null terminator at the appropriate position
                    }

                    // Updating the Current CWD Path and dept
                    splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);
                }
                else
                {
                    printf("Path Doesn't Exist\n");
                }
            }
            else
            {
                char *absolutePath = malloc(strlen(curr_cwd) + strlen(args[1]) + 2);
                strcpy(absolutePath, curr_cwd);
                strcat(absolutePath, "/");
                strcat(absolutePath, args[1]);

                if (debug)
                {
                    printf("AbsolutePath = %s\n", absolutePath);
                }

                if (chdir(absolutePath) == 0)
                {

                    // Updating Last CWD Path String
                    strcpy(prev_cwd, curr_cwd);

                    // Updating Current CWd Path String
                    int len = strlen(absolutePath); // Get the length of absolutePath
                    if (len > 0 && absolutePath[len - 1] == '/')
                    {
                        len--; // Ignore the trailing '/'
                    }
                    strcpy(curr_cwd, absolutePath); // Copy the string
                    curr_cwd[len] = '\0';           // Add null terminator at the appropriate position

                    // Updating the Current CWD Path and dept
                    splitString(curr_cwd, current_cwd_path, current_cwd_path_dept);
                    if (debug)
                    {
                        printf("Current CWD Path Dept : %d\n", *current_cwd_path_dept);
                        printf("Current CWD  : %s\n", curr_cwd);
                    }

                    // Updating the Last CWD Path and dept
                    splitString(last_cwd, last_cwd_path, last_cwd_path_dept);
                }
                else
                {
                    printf("Path Doesn't Exist\n");
                }
            }
        }
        else
        {
            printf("Path Doesn't Exist\n");
        }
    }

    return 0;
}

void cd_call_V2(char *string)
{

    // Ignore initial slash if present
    // int startIndex = (currentDir[0] == '/') ? 1 : 0;

    // for cd ~ and cd -
    if (numArgs == 2)
    {
        if (strcmp(command_args[1], "~") == 0)
        {
            strcpy(prev_cwd, curr_cwd);
            strcpy(curr_cwd, initial_cwd);
            // splitString(curr_cwd, current_cwd_path, &current_cwd_path_dept);
            // splitString(prev_cwd, last_cwd_path, &last_cwd_path_dept);
            return;
        }
        if (strcmp(command_args[1], "-") == 0)
        {
            if (strlen(prev_cwd) == 0)
            {
                printf("OLDPWD not set.\n");
                return;
            }
            strcpy(last_cwd, curr_cwd);
            strcpy(curr_cwd, prev_cwd);
            strcpy(prev_cwd, last_cwd);
            // splitString(curr_cwd, current_cwd_path, &current_cwd_path_dept);
            // splitString(prev_cwd, last_cwd_path, &last_cwd_path_dept);
            return;
        }
    }

    // if the path start with ~/somthing
    int startIdx = 1;
    strcpy(temp_cwd, curr_cwd);
    if (strcmp(command_args[1], "~") == 0)
    {
        strcpy(temp_cwd, initial_cwd);
        startIdx++;
    }

    for (int i = startIdx; i < numArgs; i++)
    {
        if (strcmp(command_args[i], ".") == 0)
        {
            continue;
        }
        else if (strcmp(command_args[i], "~") == 0)
        {
            printf("Not Correct Path!\n");
            // strcpy(temp_cwd, initial_cwd);
        }
        else if (strcmp(command_args[i], "..") == 0)
        {
            if (strcmp(temp_cwd, "/") == 0)
            {
                continue;
            }

            size_t len_temp = strlen(temp_cwd);

            if (len_temp == 0)
            {
                len_temp = 1;
                strcpy(temp_cwd, "/");
                continue;
            }

            // if(strcmp(temp_cwd[len_temp-1],'/')) {
            //     strcpy(temp_cwd[len_temp-1],"\0");
            //     len_temp--;
            // }
            if (temp_cwd[len_temp - 1] == '/')
            {
                temp_cwd[len_temp - 1] = '\0';
                len_temp--;
            }

            char *lastSlash = strrchr(temp_cwd, '/');
            temp_cwd[lastSlash - temp_cwd] = '\0';
            // strcpy(temp_cwd[lastSlash-temp_cwd],"\0");
        }
        else
        {
            // Move up one directory level
            size_t len_temp = strlen(temp_cwd);

            if (len_temp == 0)
            {
                len_temp = 1;
                strcpy(temp_cwd, "/");
            }

            if (temp_cwd[len_temp - 1] == '/')
            {
                strcat(temp_cwd, command_args[i]);
            }
            else
            {
                strcat(temp_cwd, "/");
                strcat(temp_cwd, command_args[i]);
            }
        }
    }
    if (debug)
    {
        printf("New Location : %s\n", temp_cwd);
    }

    memmove(string, string + 4, strlen(string) - 4 + 1);

    strtrim(string);
    if(strlen(string) != 0) {
        if(string[0] == '/') {
            strcpy(temp_cwd,string);
        }
    }

    // printf("New Location : %s\n", temp_cwd);
    if(strcmp(temp_cwd,"") == 0) {
        strcpy(temp_cwd,"/");
    }
    if (chdir(temp_cwd) == 0)
    {
        strcpy(prev_cwd, curr_cwd);
        strcpy(curr_cwd, temp_cwd);
    }
    else
    {
        printf("Path does not Exist!");
    }
}