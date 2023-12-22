#include "historyExec.h"
#include "discover.h"

char commands_history[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char command_args_history[MAX_COMMANDS][MAX_COMMAND_LENGTH];

int numCommands_history = 0;
int numArgs_history = 0;

int cd_done_history = 0;


// Seperate commands with ;
void parseCommands_history(char *input)
{
    char *token = strtok(strdup(input), ";");
    numCommands_history = 0;
    while (token != NULL)
    {
        cleanCommand(token);
        strtrim(token);
        strcpy(commands_history[numCommands_history++], token);
        token = strtok(NULL, ";");
    }
}


// split single command by space
void tokenizeCommand_history(char *command)
{
    char *token = strtok(strdup(command), " ");
    numArgs_history = 0;

    while (token != NULL)
    {
        strtrim(token);
        strcpy(command_args_history[numArgs_history++], token);
        token = strtok(NULL, " ");
    }
}


void cd_call_V2_history()
{

    // Ignore initial slash if present
    // int startIndex = (currentDir[0] == '/') ? 1 : 0;

    // for cd ~ and cd -
    if (numArgs_history == 2)
    {
        if (strcmp(command_args_history[1], "~") == 0)
        {
            strcpy(prev_cwd, curr_cwd);
            strcpy(curr_cwd, initial_cwd);
            // splitString(curr_cwd, current_cwd_path, &current_cwd_path_dept);
            // splitString(prev_cwd, last_cwd_path, &last_cwd_path_dept);
            return;
        }
        if (strcmp(command_args_history[1], "-") == 0)
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
    if (strcmp(command_args_history[1], "~") == 0)
    {
        strcpy(temp_cwd, initial_cwd);
        startIdx++;
    }

    for (int i = startIdx; i < numArgs_history; i++)
    {
        if (strcmp(command_args_history[i], ".") == 0)
        {
            continue;
        }
        else if (strcmp(command_args_history[i], "~") == 0)
        {
            printf("Not Correct Path!\n");
            // strcpy(temp_cwd, initial_cwd);
        }
        else if (strcmp(command_args_history[i], "..") == 0)
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
                strcat(temp_cwd, command_args_history[i]);
            }
            else
            {
                strcat(temp_cwd, "/");
                strcat(temp_cwd, command_args_history[i]);
            }
        }
    }
    if (debug)
    {
        printf("New Location : %s\n", temp_cwd);
    }
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



void historyExec(const char *command) {
    // Store input from user

    char input[1024];
    strcpy(input, command);

    strtrim(input);
    if(strcmp(strdup(input),"") == 0) {
        return;
    }

    // Parse the input and split into separate commands
    parseCommands_history(strdup(input));

    // Example: Print the commands
    for (int i = 0; i < numCommands_history; i++)
    {
        // ---------------
        // Splitting by & for BG works
        int localCommandCount_history = 0;
        char localCommands_history[MAX_COMMANDS][MAX_COMMAND_LENGTH];
        const char delimiter[] = "&";
        char *part;

        part = strtok(strdup(commands_history[i]), delimiter);

        while (part != NULL)
        {
            strcpy(localCommands_history[localCommandCount_history++], part);
            part = strtok(NULL, delimiter);
        }

        // get the args of the single FG commands[i]
        tokenizeCommand_history(strdup(localCommands_history[localCommandCount_history-1]));

        // Running the BG Commands
        for (int i = 0; i < localCommandCount_history - 1; i++)
        {
            // Background command
            // input[strlen(localCommands[i]) - 1] = '\0'; // Remove the '&' character
            executeBackground(localCommands_history[i]);
        }

        // Checking if the last command is also to be run in background
        strtrim(commands_history[i]);
        if(commands_history[i][strlen(commands_history[i])-1] == '&') {
            strcpy(commands_history[i],strdup(localCommands_history[localCommandCount_history-1]));
            strtrim(commands_history[i]);
            executeBackground(commands_history[i]);
            continue;
        }

        // Storing the last command in the command[i]
        strcpy(commands_history[i],strdup(localCommands_history[localCommandCount_history-1]));
        strtrim(commands_history[i]);   
        

        if(debug) {
            printf("--last-commands[i]--%s--\n",commands_history[i]);
        }

        if (debug)
        {
            printf("Command %d: %s\n", i + 1, commands_history[i]);
            for (int j = 0; j < numArgs_history; j++)
            {
                printf("Arg %d: %s\n", j + 1, command_args_history[j]);
            }
        }

        if (strcmp(command_args_history[0], "echo") == 0)
        {
            echo_call(command_args_history, numArgs_history);
        }
        else if (strcmp(command_args_history[0], "pwd") == 0)
        {
            pwd_call(curr_cwd);
        }
        else if (strcmp(command_args_history[0], "cd") == 0 || strcmp(command_args_history[0], "warp") == 0)
        {
            cd_tokenizeCommand_V2(strdup(localCommands_history[localCommandCount_history-1]), command_args_history, &numArgs_history);
            if (debug)
            {
                for (int j = 0; j < numArgs_history; j++)
                {
                    printf("Arg %d: %s\n", j + 1, command_args_history[j]);
                }
            }
            if (cd_done_history == 0)
            {
                cd_call_V2_history();
            }
            cd_done_history = 0;
        }
        else if (strcmp(command_args_history[0], "ls") == 0 || strcmp(command_args_history[0], "peek") == 0)
        {
            // Capturing the flag -a -l -al -la
            int flag_a = 0;
            int flag_l = 0;
            int flag_error = 0;
            for (int i = 0; i < numArgs_history; i++)
            {
                if (strcmp(command_args_history[i], "-a") == 0)
                    flag_a = 1;
                if (strcmp(command_args_history[i], "-l") == 0)
                    flag_l = 1;
                if (strcmp(command_args_history[i], "-la") == 0 || strcmp(command_args_history[i], "-al") == 0)
                {
                    flag_a = 1;
                    flag_l = 1;
                }
                if(command_args_history[i][0] == '-') {
                    for (int j = 1; command_args_history[i][j] != '\0'; j++) {
                        if (command_args_history[i][j] == 'a') {
                            flag_a = 1;
                        }
                        if (command_args_history[i][j] == 'l') {
                            flag_l = 1;
                        }
                        if (command_args_history[i][j] != 'a' && command_args_history[i][j] != 'l') {
                            flag_error = 1;  
                        }
                    }   
                }
            }
            if(flag_error == 1) {
                printf("Invalid Flag\n");
                continue;
            }

            ls_tokenizeCommand(strdup(localCommands_history[localCommandCount_history-1]), command_args_history, &numArgs_history);
            if (debug)
            {
                for (int j = 0; j < numArgs_history; j++)
                {
                    printf("Arg %d: %s\n", j + 1, command_args_history[j]);
                }
            }
            ls_call_V2(flag_a, flag_l);
        }
        else if (strcmp(command_args_history[0], "history") == 0 || strcmp(command_args_history[0], "pastevents") == 0)
        {
            if(numArgs_history == 1) {
                for (int i = 0; i < historyCount; i++)
                {
                    printf("%s", commandHistory[i]);
                }
            }
            if(numArgs_history == 2 && strcmp(command_args_history[1], "purge") == 0) {
                // delete a file named .hiddenfile
                const char *filename = ".hiddenfile";
                remove(filename);
                historyCount = 0;
            }
            if(numArgs_history == 3  && strcmp(command_args_history[1], "execute") == 0) {
                if(strlen(command_args_history[2]) > 2) {
                    printf("Index should be in range 1-15\n");
                }
                else {
                    if(stringToNumber(command_args_history[2]) > historyCount) {
                        printf("Command Doesn't exist\n");
                    }
                    else {
                        char commandLine[256]; // Assuming a maximum length of 256 characters for the command line
                        strcpy(commandLine, commandHistory[historyCount - stringToNumber(command_args_history[2])]);
                        printf("Get -> %s\n", commandLine);
                        
                    }
                }
            }
        }
        else if (strcmp(commands_history[i], "exit\n") == 0)
        {
            break;
        }
        else if (strncmp(commands_history[i], "jobs", 4) == 0)
        {
            int flag_r = 0;
            int flag_s = 0;
            if (strstr(strdup(commands_history[i]), "-s") != NULL) flag_s = 1;
            if (strstr(strdup(commands_history[i]), "-r") != NULL) flag_r = 1;
            if (strstr(strdup(commands_history[i]), "-sr") != NULL) {
                flag_r = 1;
                flag_s = 1;
            } 
            if (strstr(strdup(commands_history[i]), "-rs") != NULL) {
                flag_r = 1;
                flag_s = 1;
            }
            if(flag_r == 0 && flag_s == 0) {
                executeJobs(1,1);    
            }
            else {
                executeJobs(flag_r,flag_s);
            }
        }
        else if (strncmp(commands_history[i], "kill", 4) == 0)
        {
            int jobIndex;
            if (sscanf(commands_history[i], "kill %d", &jobIndex) == 1)
            {
                executeKill(jobIndex);
            }
            else
            {
                printf("Invalid input\n");
            }
        }
        else if (strncmp(commands_history[i], "sig", 3) == 0)
        {
            int signalNum, jobIndex;
            if (sscanf(commands_history[i], "sig %d %d", &jobIndex, &signalNum) == 2)
            {
                executeSig(jobIndex, signalNum);
            }
            else
            {
                printf("Invalid command\n");
            }
        }
        else if (strncmp(commands_history[i], "fg", 2) == 0)
        {
            int jobIndex;
            if (sscanf(commands_history[i], "fg %d", &jobIndex) == 1)
            {
                executeFg(jobIndex);
            }
            else
            {
                printf("Invalid command\n");
            }
        }
        else if (strncmp(commands_history[i], "bg", 2) == 0)
        {
            int jobIndex;
            if (sscanf(commands_history[i], "bg %d", &jobIndex) == 1)
            {
                executeBg(jobIndex);
            }
            else
            {
                printf("Invalid command\n");
            }
        }
        else if (strcmp(commands_history[i], "\n") != 0)
        {
            // Remove the trailing newline character
            commands_history[i][strcspn(commands_history[i], "\n")] = 0;

            if (commands_history[i][strlen(commands_history[i]) - 1] == '&')
            {
                // Background command
                commands_history[i][strlen(commands_history[i]) - 1] = '\0'; // Remove the '&' character
                executeBackground(commands_history[i]);
            }
            else if (strncmp(commands_history[i], "pinfo", 5) == 0)
            {
                char pid[256];
                if (sscanf(commands_history[i], "pinfo %s", pid) == 1)
                {
                    pid_t processId = atoi(pid);
                    executePinfo(processId);
                }
                else
                {
                    shellPinfo();
                    // printf("Invalid command\n");
                }
            }
            else if (strncmp(commands_history[i], "proclore", 8) == 0)
            {
                char pid[256];
                if (sscanf(commands_history[i], "proclore %s", pid) == 1)
                {
                    pid_t processId = atoi(pid);
                    executePinfo(processId);
                }
                else
                {
                    shellPinfo();
                    // printf("Invalid command\n");
                }
            }
            else if(strncmp(commands_history[i], "seek", 4) == 0) {
                // Tokenizing the input using strtok
                char *token = strtok(strdup(input), " ");
                int argc = 0;
                char *argv[20]; // Maximum number of tokens (adjust as needed)

                while (token != NULL) {
                    argv[argc++] = token;
                    token = strtok(NULL, " ");
                }
                if(debug) {
                    printf("Cheeck  : %d %s %s %s %s",argc, argv[0],argv[1],argv[2],argv[3]);
                }
                discover(argc, argv);
            }
            else
            {
                // Foreground command
                // printf("Too Bad\n");
                executeForeground(commands_history[i]);
            }
        }
    }

}
