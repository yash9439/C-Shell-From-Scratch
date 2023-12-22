#include "prompt.h"

// Set the hostname, username and the curr dir in global var
int set_display()
{
    // Retrieve username
    if (getlogin_r(username, sizeof(username)) != 0)
    {
        perror("Error retrieving username");
        return 1;
    }

    // Retrieve hostname
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        perror("Error retrieving hostname");
        return 1;
    }

    // Retrieve working directory
    if (getcwd(initial_cwd, sizeof(initial_cwd)) == NULL)
    {
        perror("Error retrieving working directory");
        return 1;
    }

    // Retrieve working directory
    if (getcwd(curr_cwd, sizeof(initial_cwd)) == NULL)
    {
        perror("Error retrieving working directory");
        return 1;
    }

    // Split the initial cwd string
    splitString(initial_cwd, initial_cwd_path, &initial_cwd_path_dept);

    // Split the current cwd string
    splitString(curr_cwd, current_cwd_path, &current_cwd_path_dept);

    return 0;
}

// // Print before taking input from user
int display()
{
    // If i am in Home Directory
    if (strcmp(initial_cwd, curr_cwd) == 0)
    {
        printf("<%s@%s:~> ", username, hostname);
    }
    else if(strncmp(initial_cwd, curr_cwd, strlen(initial_cwd)) == 0) {
        printf("<%s@%s:~%s> ", username, hostname, curr_cwd + strlen(initial_cwd));
    }
    // If i am not in my home directory
    else
    {
        printf("<%s@%s:%s> ", username, hostname, curr_cwd);
    }
}