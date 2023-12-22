#include "jobs.h"

void executeJobs(int flag_r, int flag_s)
{
    JobNode *current = head;
    int count = 1;

    while (current != NULL)
    {
        if(flag_r == 0 && current->status == 0) {
            current = current->next;
            continue;
        }
        if(flag_s == 0 && current->status == 1) {
            current = current->next;
            continue;
        }
        printf("[%d] %s [%d] %s\n", count, current->status == 0 ? "Running" : "Stopped", current->pid, current->command);
        current = current->next;
        count++;
    }
}


void executeSig(int jobIndex, int signalNum)
{
    JobNode *current = head;
    int count = 1;

    while (current != NULL)
    {
        if (count == jobIndex)
        {
            if (current->isForeground)
            {
                printf("Cannot send a signal to a foreground job\n");
            }
            else
            {
                kill(current->pid, signalNum);
                printf("Sent signal %d to process with pid = %d\n", signalNum, current->pid);
            }
            break;
        }

        current = current->next;
        count++;
    }
}


void executeFg(int jobIndex)
{
    JobNode *current = head;
    JobNode *previous = NULL;
    int count = 1;

    while (current != NULL)
    {
        if (count == jobIndex)
        {
            if (current->isForeground == 0)
            {
                // Background job
                printf("Bringing [%d] %s to the foreground\n", jobIndex, current->command);
                
                if (current->status == 1)
                {
                    // Stopped background job
                    printf("Resuming the stopped background job\n");
                    kill(current->pid, SIGCONT);
                    current->status = 0; // Mark the job as running
                }
                
                current->isForeground = 1; // Set as foreground job

                // Update the status of the foreground process
                JobNode *fgJob = head;
                while (fgJob != NULL)
                {
                    if (fgJob->isForeground == 1 && fgJob->pid != current->pid)
                    {
                        fgJob->isForeground = 0; // Set other foreground jobs as background
                        break;
                    }
                    fgJob = fgJob->next;
                }

                // Update the foreground process variables
                fgrunning = 1;
                fgstopped = 0;
                fgpid = current->pid;
                strcpy(fgname, current->command);

                // Wait for the foreground process to finish
                int status;
                waitpid(current->pid, &status, WUNTRACED);

                // Check if the foreground process was stopped by a signal
                if (WIFSTOPPED(status))
                {
                    printf("Foreground job [%d] %s stopped\n", jobIndex, current->command);
                    current->status = 1; // Mark the job as stopped
                    fgstopped = 1;
                }
                else
                {
                    // The foreground process has finished
                    printf("Foreground job [%d] %s finished\n", jobIndex, current->command);
                    current->status = 2; // Mark the job as finished
                    fgrunning = 0;

                    // Remove the finished job from the list
                    if (previous == NULL)
                    {
                        // Removing the head node
                        head = current->next;
                    }
                    else
                    {
                        previous->next = current->next;
                    }

                    free(current);
                }
            }
            else
            {
                printf("Job [%d] %s is already in the foreground\n", jobIndex, current->command);
            }
            break;
        }

        previous = current;
        current = current->next;
        count++;
    }

    if (current == NULL)
    {
        printf("Job [%d] not found\n", jobIndex);
    }
}

void executeBg(int jobIndex)
{
    JobNode *current = head;
    int count = 1;

    while (current != NULL)
    {
        if (count == jobIndex)
        {
            if (current->isForeground == 0)
            {
                // Background job
                if (current->status == 1)
                {
                    // Stopped background job
                    printf("Resuming the stopped background job [%d] %s\n", jobIndex, current->command);
                    kill(current->pid, SIGCONT);
                    current->status = 0; // Mark the job as running
                }
                else
                {
                    printf("Job [%d] %s is already running in the background\n", jobIndex, current->command);
                }
            }
            else
            {
                printf("Job [%d] %s is a foreground job, cannot bring it to the background\n", jobIndex, current->command);
            }
            break;
        }

        current = current->next;
        count++;
    }

    if (current == NULL)
    {
        printf("Job [%d] not found\n", jobIndex);
    }
}
