#include "pinfo.h"

void executePinfo(pid_t pid)
{
    JobNode *current = head;

    while (current != NULL)
    {
        if (current->pid == pid)
        {
            char statusString[10];

            if (current->status == 0)
            {
                strcpy(statusString, "R");
            }
            else
            {
                strcpy(statusString, "S");
            }

            // Check if the process is running in the foreground
            if (current->isForeground == 1)
            {
                strcat(statusString, "+");
            }

            char procPath[256];
            sprintf(procPath, "/proc/%d/status", pid);

            FILE *file = fopen(procPath, "r");
            if (file == NULL)
            {
                printf("Error: Process with pid %d not found\n", pid);
                return;
            }

            char line[256];
            char virtualMemory[256];
            char executablePath[256];

            while (fgets(line, sizeof(line), file) != NULL)
            {
                if (strncmp(line, "VmSize:", 7) == 0)
                {
                    sscanf(line, "VmSize:\t%s", virtualMemory);
                    break;
                }
            }

            fclose(file);

            char exePath[256];
            sprintf(exePath, "/proc/%d/exe", pid);
            ssize_t bytesRead = readlink(exePath, executablePath, sizeof(executablePath) - 1);
            if (bytesRead != -1)
            {
                executablePath[bytesRead] = '\0';
            }
            else
            {
                strcpy(executablePath, "Unknown");
            }

            printf("pid: %d\n", pid);
            printf("process status: %s\n", statusString);
            printf("memory: %s (Virtual Memory)\n", virtualMemory);
            printf("executable path: %s\n", executablePath);

            return;
        }

        current = current->next;
    }

    printf("Error: Process with pid %d not found in the job list\n", pid);
}

void shellPinfo() {
    pid_t currentPID = getpid();
    
    // You can print the current process information directly
    printf("pid: %d\n", currentPID);

    char statusString[10];
    strcpy(statusString, "R+");
    
    char procPath[256];
    sprintf(procPath, "/proc/%d/status", currentPID);
    
    FILE *file = fopen(procPath, "r");
    if (file != NULL)
    {
        char line[256];
        char virtualMemory[256];
        while (fgets(line, sizeof(line), file) != NULL)
        {
            if (strncmp(line, "VmSize:", 7) == 0)
            {
                sscanf(line, "VmSize:\t%s", virtualMemory);
                break;
            }
        }
        fclose(file);

        printf("Process Status: %s\n", statusString);
        printf("Virtual Memory: %s\n", virtualMemory);
    }
    else
    {
        printf("Error: Unable to access process status\n");
    }

    // Get the executable path
    char executablePath[256];
    ssize_t bytesRead = readlink("/proc/self/exe", executablePath, sizeof(executablePath) - 1);
    if (bytesRead != -1)
    {
        executablePath[bytesRead] = '\0';
        printf("Executable Path: %s\n", executablePath);
    }
    else
    {
        printf("Error: Unable to determine executable path\n");
    }
}