#include "fgbg.h"

void handler(int sig)
{
    pid_t chpid;
    int status;

    if (sig == SIGCHLD)
    {
        while ((chpid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            // Check if the child process is a background or foreground job
            JobNode *current = head;
            JobNode *previous = NULL;

            while (current != NULL)
            {
                if (current->pid == chpid)
                {
                    if (WIFEXITED(status))
                    {
                        if (WEXITSTATUS(status) == 0)
                        {
                            printf("%s with pid = %d exited normally\n", current->command, current->pid);
                        }
                        else
                        {
                            printf("%s with pid = %d exited with an error\n", current->command, current->pid);
                        }
                        if (previous != NULL)
                        {
                            previous->next = current->next;
                        }
                        else
                        {
                            head = current->next;
                        }
                        free(current); // Free the memory of the job node
                        numJobs--;
                    }
                    else if (WIFSTOPPED(status))
                    {
                        printf("[%d] Stopped %s [%d]\n", numJobs, current->command, current->pid);
                        current->status = 1; // Mark the job as stopped
                    }
                    else if (WIFSIGNALED(status))
                    {
                        printf("%s with pid = %d terminated by signal\n", current->command, current->pid);
                        if (previous != NULL)
                        {
                            previous->next = current->next;
                        }
                        else
                        {
                            head = current->next;
                        }
                        free(current); // Free the memory of the job node
                        numJobs--;
                    }
                    fflush(stdout);
                    break;
                }

                previous = current;
                current = current->next;
            }

            if (fgrunning != 0 && chpid == fgpid)
            {
                fgrunning = 0;
                fgstopped = 1;
                strcpy(fgname, "");
            }
        }
    }
}

void ctrlZHandler(int sig)
{
    if (fgrunning != 0)
    {
        printf("\nForeground process stopped\n");
        kill(fgpid, SIGTSTP);
        fgrunning = 0;
        fgstopped = 1;
        strcpy(fgname, "");

        // Update the status of the foreground process
        JobNode *current = head;
        while (current != NULL)
        {
            if (current->pid == fgpid)
            {
                current->status = 1; // Mark the job as stopped
                break;
            }
            current = current->next;
        }

        // Add the stopped process to the job list
        JobNode *newJob = (JobNode *)malloc(sizeof(JobNode));
        strcpy(newJob->command, fgname);
        newJob->pid = fgpid;
        newJob->status = 1;       // Stopped
        newJob->isForeground = 1; // Foreground process
        newJob->next = NULL;

        if (head == NULL)
        {
            head = newJob;
        }
        else
        {
            JobNode *job = head;
            while (job->next != NULL)
            {
                job = job->next;
            }
            job->next = newJob;
        }

        numJobs++;
    }
    else
    {
        // Ignore the SIGTSTP signal for background processes
    }
}

void interruptHandler(int sig)
{
    // Ignore SIGINT signal if there is no foreground process running
    if (fgrunning != 0)
    {
        // Send SIGINT signal to the foreground process
        printf("\nForeground process interrupted\n");
        kill(fgpid, SIGINT);
    }
}

void eofHandler(int sig)
{
    // Check if the program is running in an interactive terminal
    if (isatty(fileno(stdin)))
    {
        printf("Logging out...\n");
        exit(0);  // Exit the program gracefully
    }
    else
    {
        // Ignore the EOF signal if not running in an interactive terminal
        // or perform any other desired action
    }
}

void executeForeground(char *command)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process

        // Set up signal handling for child process
        signal(SIGTSTP, SIG_DFL);
        signal(SIGINT, SIG_DFL);

        char *args[256];
        int i = 0;

        args[i] = strtok(command, " \t\n");
        while (args[i] != NULL)
        {
            i++;
            args[i] = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1)
        {
            printf("Command not found or execution error\n");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        // Parent process
        fgrunning = 1;
        fgstopped = 0;
        fgpid = pid;
        strcpy(fgname, command);

        int status;

        // Wait for the foreground process to finish
        waitpid(pid, &status, WUNTRACED);

        if (WIFSTOPPED(status))
        {
            printf("\nForeground process stopped\n");
            fgrunning = 0;
            fgstopped = 1;
            strcpy(fgname, "");

            // Update the status of the foreground process
            JobNode *current = head;
            while (current != NULL)
            {
                if (current->pid == pid)
                {
                    current->status = 1; // Mark the job as stopped
                    break;
                }
                current = current->next;
            }
        }
        else
        {
            fgrunning = 0;
            fgstopped = 0;
            strcpy(fgname, "");

            // Update the status of the foreground process
            JobNode *current = head;
            JobNode *previous = NULL;
            while (current != NULL)
            {
                if (current->pid == pid)
                {
                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                    {
                        printf("%s with pid = %d exited normally\n", current->command, current->pid);
                    }
                    else
                    {
                        printf("%s with pid = %d exited with an error\n", current->command, current->pid);
                    }
                    if (previous != NULL)
                    {
                        previous->next = current->next;
                    }
                    else
                    {
                        head = current->next;
                    }
                    free(current); // Free the memory of the job node
                    numJobs--;
                    break;
                }
                previous = current;
                current = current->next;
            }
        }
    }
    else
    {
        // Fork error
        printf("Failed to create child process\n");
    }
}

void executeBackground(char *command)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process

        // Set up signal handling for child process
        signal(SIGTSTP, SIG_IGN);
        signal(SIGINT, SIG_IGN);

        char *args[256];
        int i = 0;

        args[i] = strtok(command, " \t\n");
        while (args[i] != NULL)
        {
            i++;
            args[i] = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1)
        {
            printf("Command not found or execution error\n");
            exit(1);
        }
    }
    else if (pid > 0)
    {
        // Parent process
        printf("Background process with pid = %d started\n", pid);

        // Add the background job to the job list
        JobNode *newJob = (JobNode *)malloc(sizeof(JobNode));
        strcpy(newJob->command, command);
        newJob->pid = pid;
        newJob->status = 0;
        newJob->isForeground = 0;
        newJob->next = NULL;

        if (head == NULL)
        {
            head = newJob;
        }
        else
        {
            JobNode *current = head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = newJob;
        }

        numJobs++;
    }
    else
    {
        // Fork error
        printf("Failed to create child process\n");
    }
}

void executeKill(int jobIndex)
{
    JobNode *current = head;
    int count = 1;

    while (current != NULL)
    {
        if (count == jobIndex)
        {
            if (current->status == 0)
            {
                kill(current->pid, SIGKILL);
                printf("Killed process with pid = %d\n", current->pid);
                if (current->isForeground == 1)
                {
                    fgrunning = 0;
                    fgstopped = 0;
                    strcpy(fgname, "");
                }

                if (current == head)
                {
                    head = current->next;
                }
                else
                {
                    JobNode *previous = head;
                    while (previous->next != current)
                    {
                        previous = previous->next;
                    }
                    previous->next = current->next;
                }

                free(current);
                numJobs--;
            }
            else
            {
                printf("Cannot kill a stopped job\n");
            }
            break;
        }

        current = current->next;
        count++;
    }
}