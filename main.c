#include "cd.h"
#include "echo.h"
#include "fgbg.h"
#include "headers.h"
#include "history.h"
#include "jobs.h"
#include "ls.h"
#include "pinfo.h"
#include "processing.h"
#include "prompt.h"
#include "pwd.h"
#include "discover.h"
#include "historyExec.h"
int debug = 0;

char commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char command_args[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char initial_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char current_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char last_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
int numCommands = 0;
int numArgs = 0;
int initial_cwd_path_dept = 0;
int current_cwd_path_dept = 0;
int last_cwd_path_dept = 0;

char username[1024] = "\0";
char hostname[1024] = "\0";
char initial_cwd[1024] = "\0";
char curr_cwd[1024] = "\0";
char prev_cwd[1024] = "\0";
char last_cwd[1024] = "\0";
char temp_cwd[1024] = "\0";

char commandHistory[MAX_HISTORY_SIZE][MAX_COMMAND_LENGTH];
int historyCount = 0;

int cd_done = 0;


// FGBG Var Starts
int fgrunning = 0;
int fgstopped = 0;
char fgname[256];
pid_t fgpid;

JobNode *head = NULL;
int numJobs = 0;

// FGBG Var Ends


// LS Var Starts
FileInfo myFileInfo;

// LS Var Ends


int main()
{
    signal(SIGCHLD, handler);
    signal(SIGTSTP, ctrlZHandler);
    // signal(SIGINT, SIG_IGN); // Ignore SIGINT signal

    signal(SIGINT, interruptHandler);
    // signal(SIGINT, eofHandler);

    set_display();

    // Store input from user
    char input[1024];

    while (1)
    {
        display();
        fgets(input, sizeof(input), stdin);

        if (feof(stdin))
        {
            // CTRL+D was pressed, end the program
            break;
        }

        // Reading the .hiddenfile for past commands
        readingHistory();

        strtrim(input);
        if (!(strstr(strdup(input), "pastevents") != NULL)) {
            if(debug) {
                printf("Storing %s\n",input);
            }
            if(strcmp(strdup(input),"") != 0) {
                historyStore(strdup(input));
            }
        }
        if (strcmp(input, "exit\n") == 0)
        {
            break;
        }
        // strtrim(input);
        if(strcmp(strdup(input),"") == 0) {
            continue;
        }
        

        // Parse the input and split into separate commands
        parseCommands(strdup(input));

        // Example: Print the commands
        for (int i = 0; i < numCommands; i++)
        {
            // ---------------
            // Splitting by & for BG works
            int localCommandCount = 0;
            char localCommands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
            const char delimiter[] = "&";
            char *part;

            part = strtok(strdup(commands[i]), delimiter);

            while (part != NULL)
            {
                strcpy(localCommands[localCommandCount++], part);
                // printf("%s\n", part);
                part = strtok(NULL, delimiter);
            }

            // Add delimiter to all chunks except the last one
            // for (int i = 0; i < localCommandCount - 1; i++)
            // {
            //     strcat(localCommands[i], delimiter);
            // }
            // --------------

            // get the args of the single FG commands[i]
            tokenizeCommand(strdup(localCommands[localCommandCount-1]));

            // Running the BG Commands
            for (int i = 0; i < localCommandCount - 1; i++)
            {
                // Background command
                // input[strlen(localCommands[i]) - 1] = '\0'; // Remove the '&' character
                executeBackground(localCommands[i]);
            }

            // Checking if the last command is also to be run in background
            strtrim(commands[i]);
            if(commands[i][strlen(commands[i])-1] == '&') {
                strcpy(commands[i],strdup(localCommands[localCommandCount-1]));
                strtrim(commands[i]);
                executeBackground(commands[i]);
                continue;
            }

            // Storing the last command in the command[i]
            strcpy(commands[i],strdup(localCommands[localCommandCount-1]));
            strtrim(commands[i]);   
            

            if(debug) {
                printf("--last-commands[i]--%s--\n",commands[i]);
            }

            if (debug)
            {
                printf("Command %d: %s\n", i + 1, commands[i]);
                for (int j = 0; j < numArgs; j++)
                {
                    printf("Arg %d: %s\n", j + 1, command_args[j]);
                }
            }

            if (strcmp(command_args[0], "echo") == 0)
            {
                echo_call(command_args, numArgs);
            }
            else if (strcmp(command_args[0], "pwd") == 0)
            {
                pwd_call(curr_cwd);
            }
            else if (strcmp(command_args[0], "cd") == 0 || strcmp(command_args[0], "warp") == 0)
            {
                cd_tokenizeCommand_V2(strdup(localCommands[localCommandCount-1]), command_args, &numArgs);
                if (debug)
                {
                    for (int j = 0; j < numArgs; j++)
                    {
                        printf("Arg %d: %s\n", j + 1, command_args[j]);
                    }
                }
                if (cd_done == 0)
                {
                    cd_call_V2(strdup(input));
                }
                cd_done = 0;
            }
            else if (strcmp(command_args[0], "ls") == 0 || strcmp(command_args[0], "peek") == 0)
            {
                // Capturing the flag -a -l -al -la
                int flag_a = 0;
                int flag_l = 0;
                int flag_error = 0;
                for (int i = 0; i < numArgs; i++)
                {
                    if (strcmp(command_args[i], "-a") == 0)
                        flag_a = 1;
                    if (strcmp(command_args[i], "-l") == 0)
                        flag_l = 1;
                    if (strcmp(command_args[i], "-la") == 0 || strcmp(command_args[i], "-al") == 0)
                    {
                        flag_a = 1;
                        flag_l = 1;
                    }
                    if(command_args[i][0] == '-') {
                        for (int j = 1; command_args[i][j] != '\0'; j++) {
                            if (command_args[i][j] == 'a') {
                                flag_a = 1;
                            }
                            if (command_args[i][j] == 'l') {
                                flag_l = 1;
                            }
                            if (command_args[i][j] != 'a' && command_args[i][j] != 'l') {
                                flag_error = 1;  
                            }
                        }   
                    }
                }
                if(flag_error == 1) {
                    printf("Invalid Flag\n");
                    continue;
                }

                ls_tokenizeCommand(strdup(localCommands[localCommandCount-1]), command_args, &numArgs);
                if (debug)
                {
                    for (int j = 0; j < numArgs; j++)
                    {
                        printf("Arg %d: %s\n", j + 1, command_args[j]);
                    }
                }
                ls_call_V2(flag_a, flag_l);
            }
            else if (strcmp(command_args[0], "history") == 0 || strcmp(command_args[0], "pastevents") == 0)
            {
                if(numArgs == 1) {
                    for (int i = 0; i < historyCount; i++)
                    {
                        printf("%s\n", commandHistory[i]);
                    }
                    printf("\n");
                }
                if(numArgs == 2 && strcmp(command_args[1], "purge") == 0) {
                    // delete a file named .hiddenfile
                    const char *filename = ".hiddenfile";

                    // Construct the full path to the hidden file
                    char full_path[strlen(initial_cwd) + strlen(filename) + 2]; // +2 for the slash and null terminator
                    snprintf(full_path, sizeof(full_path), "%s/%s", initial_cwd, filename);
                    remove(full_path);
                    historyCount = 0;
                }
                if(numArgs == 3  && strcmp(command_args[1], "execute") == 0) {
                    if(strlen(command_args[2]) > 2) {
                        printf("Index should be in range 1-15\n");
                    }
                    else {
                        if(stringToNumber(command_args[2]) > historyCount) {
                            printf("Command Doesn't exist\n");
                        }
                        else {
                            char commandLine[256]; // Assuming a maximum length of 256 characters for the command line
                            strcpy(commandLine, commandHistory[historyCount - stringToNumber(command_args[2])]);
                            historyExec(commandLine);
                        }
                    }
                }
            }
            else if (strcmp(commands[i], "exit\n") == 0)
            {
                break;
            }
            else if (strncmp(commands[i], "jobs", 4) == 0)
            {
                int flag_r = 0;
                int flag_s = 0;
                if (strstr(strdup(commands[i]), "-s") != NULL) flag_s = 1;
                if (strstr(strdup(commands[i]), "-r") != NULL) flag_r = 1;
                if (strstr(strdup(commands[i]), "-sr") != NULL) {
                    flag_r = 1;
                    flag_s = 1;
                } 
                if (strstr(strdup(commands[i]), "-rs") != NULL) {
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
            else if (strncmp(commands[i], "kill", 4) == 0)
            {
                int jobIndex;
                if (sscanf(commands[i], "kill %d", &jobIndex) == 1)
                {
                    executeKill(jobIndex);
                }
                else
                {
                    printf("Invalid input\n");
                }
            }
            else if (strncmp(commands[i], "sig", 3) == 0)
            {
                int signalNum, jobIndex;
                if (sscanf(commands[i], "sig %d %d", &jobIndex, &signalNum) == 2)
                {
                    executeSig(jobIndex, signalNum);
                }
                else
                {
                    printf("Invalid command\n");
                }
            }
            else if (strncmp(commands[i], "fg", 2) == 0)
            {
                int jobIndex;
                if (sscanf(commands[i], "fg %d", &jobIndex) == 1)
                {
                    executeFg(jobIndex);
                }
                else
                {
                    printf("Invalid command\n");
                }
            }
            else if (strncmp(commands[i], "bg", 2) == 0)
            {
                int jobIndex;
                if (sscanf(commands[i], "bg %d", &jobIndex) == 1)
                {
                    executeBg(jobIndex);
                }
                else
                {
                    printf("Invalid command\n");
                }
            }
            else if (strcmp(commands[i], "\n") != 0)
            {
                // Remove the trailing newline character
                commands[i][strcspn(commands[i], "\n")] = 0;

                if (commands[i][strlen(commands[i]) - 1] == '&')
                {
                    // Background command
                    commands[i][strlen(commands[i]) - 1] = '\0'; // Remove the '&' character
                    executeBackground(commands[i]);
                }
                else if (strncmp(commands[i], "pinfo", 5) == 0)
                {
                    char pid[256];
                    if (sscanf(commands[i], "pinfo %s", pid) == 1)
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
                else if (strncmp(commands[i], "proclore", 8) == 0)
                {
                    char pid[256];
                    if (sscanf(commands[i], "proclore %s", pid) == 1)
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
                else if(strncmp(commands[i], "seek", 4) == 0) {
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
                    // printf("Cheeck  : %d %s %s %s %s",argc, argv[0],argv[1],argv[2],argv[3]);
                    discover(argc, argv);
                }
                else
                {
                    // Foreground command
                    // printf("Too Bad\n");
                    executeForeground(commands[i]);
                }
            }
        }

        // printf("\n");
        // printf("%s",initial_cwd);
        // printf("\n");
    }
    printf("\n");
    return 0;
}
