#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h> // for isspace() function
#include <limits.h>
#include <dirent.h>   // For DIR in ls
#include <sys/stat.h> // ls stats

#include <pwd.h>
#include <grp.h>
#include <time.h>

#include <string.h>
#include <signal.h>

#define MAX_COMMANDS 100
#define MAX_COMMAND_LENGTH 100
#define MAX_HISTORY_SIZE 15

extern int debug;

extern char commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
extern char command_args[MAX_COMMANDS][MAX_COMMAND_LENGTH];
extern char initial_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
extern char current_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
extern char last_cwd_path[MAX_COMMANDS][MAX_COMMAND_LENGTH];
extern int numCommands;
extern int numArgs;
extern int initial_cwd_path_dept;
extern int current_cwd_path_dept;
extern int last_cwd_path_dept;

extern char username[1024];
extern char hostname[1024];
extern char initial_cwd[1024];
extern char curr_cwd[1024];
extern char prev_cwd[1024];
extern char last_cwd[1024];
extern char temp_cwd[1024];

extern char commandHistory[MAX_HISTORY_SIZE][MAX_COMMAND_LENGTH];
extern int historyCount;

extern int cd_done;



// FGBG Starts
extern int fgrunning;
extern int fgstopped;
extern char fgname[256];
extern pid_t fgpid;

typedef struct Node {
    char command[256];
    pid_t pid;
    int status;       // 0 for running, 1 for stopped
    int isForeground; // 1 for foreground, 0 for background
    struct Node *next;
} JobNode;

extern JobNode *head;
extern int numJobs;

// FGBG Ends




// LS Starts
typedef struct FileInfo {
    char name[256];
    char permissions[11];
    nlink_t nlinks;
    char owner[256];
    char group[256];
    off_t size;
    char mod_time[20];
} FileInfo;

extern FileInfo myFileInfo;

// LS Ends


#endif