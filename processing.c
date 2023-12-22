#include "processing.h"


// For splitting initial_cwd and current_cwd in initial_cwd_path and current_cwd_path
void splitString(char *input, char (*output)[MAX_COMMAND_LENGTH], int *index)
{
    char *token = strtok(strdup(input), "/");
    *index = 0;

    while (token != NULL)
    {
        strncpy(output[*index], token, MAX_COMMAND_LENGTH - 1);
        output[*index][MAX_COMMAND_LENGTH - 1] = '\0'; // Ensure null-termination
        (*index)++;
        token = strtok(NULL, "/");
    }
}

// Trim a string be removing leading and trailing spaces
void strtrim(char *str)
{
    char *start = str;
    char *end = str + strlen(str) - 1;

    while (isspace(*start))
    {
        start++;
    }

    while (end > start && isspace(*end))
    {
        end--;
    }

    *(end + 1) = '\0';

    // Shift the trimmed string to the beginning
    if (str != start)
    {
        memmove(str, start, strlen(start) + 1);
    }
}


// Get rid of extra spaces in commands
void cleanCommand(char *command)
{
    int i = 0, j = 0;
    int spaceCount = 0;
    int len = strlen(command);

    while (command[i] != '\0')
    {
        if (isspace((unsigned char)command[i]))
        {
            spaceCount++;
            if (spaceCount == 1)
            {
                command[j++] = ' ';
            }
        }
        else
        {
            spaceCount = 0;
            command[j++] = command[i];
        }
        i++;
    }
    command[j] = '\0';
}

// Seperate commands with ;
void parseCommands(char *input)
{
    char *token = strtok(strdup(input), ";");
    numCommands = 0;
    while (token != NULL)
    {
        cleanCommand(token);
        strtrim(token);
        strcpy(commands[numCommands++], token);
        token = strtok(NULL, ";");
    }
}

// split single command by space
void tokenizeCommand(char *command)
{
    char *token = strtok(strdup(command), " ");
    numArgs = 0;

    while (token != NULL)
    {
        strtrim(token);
        strcpy(command_args[numArgs++], token);
        token = strtok(NULL, " ");
    }
}


int stringToNumber(const char *str) {
    int result = 0;

    if (str[0] >= '0' && str[0] <= '9') {
        result = str[0] - '0'; // Convert the first character
        if (str[1] >= '0' && str[1] <= '9') {
            result = result * 10 + (str[1] - '0'); // Convert the second character
        }
    }

    return result;
}