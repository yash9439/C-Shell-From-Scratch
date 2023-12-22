#include "ls.h"

void printColoredText(const char *text, const char *colorCode) {
    printf(" %s%s%s\n", colorCode, text, "\x1B[0m");
}

int compare_names(const void *a, const void *b)
{
    const struct FileInfo *file1 = (const struct FileInfo *)a;
    const struct FileInfo *file2 = (const struct FileInfo *)b;
    return strcasecmp(file1->name, file2->name);
}

void replaceSubstring(char *string, const char *substring, const char *replacement)
{
    char *match;

    while ((match = strstr(string, substring)) != NULL)
    {
        size_t substringLen = strlen(substring);
        size_t replacementLen = strlen(replacement);

        memmove(match, match + substringLen, strlen(match + substringLen) + 1);
        memmove(match + substringLen, match + substringLen + replacementLen, strlen(match + substringLen + replacementLen) + 1);
    }
}

void ls_get_args(const char *str)
{
    size_t strLen = strlen(str);
    char *substring = (char *)malloc((strLen + 1) * sizeof(char));
    size_t substringIndex = 0;
    int insideQuotes = 0;
    int escapeNextChar = 0;

    numArgs = 1;

    for (size_t i = 0; i < strLen; i++)
    {
        char currentChar = str[i];

        if (currentChar == '"' && !escapeNextChar)
        {
            insideQuotes = !insideQuotes;
        }

        if (currentChar == ' ' && !insideQuotes && !escapeNextChar)
        {
            if (substringIndex > 0)
            {
                substring[substringIndex] = '\0';
                if (debug)
                {
                    printf("LS Args => %s\n", substring);
                }
                strcpy(command_args[numArgs++], substring);
                substringIndex = 0;
            }
        }
        else
        {
            if (!insideQuotes && !escapeNextChar && currentChar == '\\')
            {
                // nothing
            }
            else if (currentChar == '\"' || currentChar == '\'')
            {
                // nothing
            }
            else
            {
                substring[substringIndex++] = currentChar;
            }
        }

        escapeNextChar = (currentChar == '\\' && !escapeNextChar);
    }

    if (substringIndex > 0)
    {
        substring[substringIndex] = '\0';
        strcpy(command_args[numArgs++], substring);
        if(debug) {
            printf("LS Args => %s\n", substring);
        }
    }

    free(substring);
}

// Function to compare two strings alphabetically
int compareStrings(const void *a, const void *b)
{
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    return strcasecmp(str1, str2);
}

void count_total(char *input, int *total)
{
    struct stat fileStat;
    stat(input, &fileStat);
    (*total) += fileStat.st_blocks;
}

void listFilesInDirectory(const char *path, int showHidden, int showDetails)
{
    DIR *dir = opendir(path);
    if (dir)
    {
        struct dirent *entry;
        char **names = NULL;
        int count = 0;
        int total = 0; // Total block count

        // Read all the directory entries and store their names
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_REG || entry->d_type == DT_DIR || (showHidden && entry->d_type == DT_UNKNOWN))
            {
                // Allocate memory for the name
                char *name = malloc(strlen(entry->d_name) + 1);
                strcpy(name, entry->d_name);

                // Add the name to the array
                names = realloc(names, (count + 1) * sizeof(char *));
                names[count] = name;
                count++;

                // Get the file/directory path
                char filePath[1024];
                sprintf(filePath, "%s/%s", path, name);

                // Update total block count
                count_total(filePath, &total);
            }
        }
        closedir(dir);

        // Sort the array of names alphabetically
        qsort(names, count, sizeof(char *), compareStrings);

        // Print the total block count
        if (showDetails)
            printf("Total: %d\n", total / 2);

        // Print the sorted names with or without details
        for (int i = 0; i < count; i++)
        {
            char *name = names[i];

            // Skip hidden files and directories if showHidden is not set
            if (!showHidden && name[0] == '.')
                continue;

            if (showDetails)
            {
                struct stat fileStat;
                char filePath[1024];
                sprintf(filePath, "%s/%s", path, name);

                if (stat(filePath, &fileStat) < 0)
                    continue;

                // Permissions
                printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

                // Number of links
                printf(" %ld", fileStat.st_nlink);

                // Owner and group
                struct passwd *pw = getpwuid(fileStat.st_uid);
                struct group *gr = getgrgid(fileStat.st_gid);
                printf(" %s %s", pw->pw_name, gr->gr_name);

                // File size
                printf(" %ld", fileStat.st_size);

                // Last modified time
                char *timeStr = ctime(&fileStat.st_mtime);
                timeStr[strlen(timeStr) - 1] = '\0'; // Remove newline character
                printf(" %s", timeStr);

                // File name
                if (S_ISDIR(fileStat.st_mode)) {
                    printColoredText(name, "\x1B[34m");  // Blue for directories
                }
                else if (fileStat.st_mode & S_IXUSR || fileStat.st_mode & S_IXGRP || fileStat.st_mode & S_IXOTH) {
                    printColoredText(name, "\x1B[32m");  // Green for executables
                }
                else {
                    printColoredText(name, "\x1B[37m");  // White for regular files
                }

                // printf(" %s\n", name);
            }
            else
            {
                struct stat fileStat;
                char filePath[1024];
                sprintf(filePath, "%s/%s", path, name);

                if (stat(filePath, &fileStat) < 0)
                    continue;
                if (S_ISDIR(fileStat.st_mode)) {
                    printColoredText(name, "\x1B[34m");  // Blue for directories
                }
                else if (fileStat.st_mode & S_IXUSR || fileStat.st_mode & S_IXGRP || fileStat.st_mode & S_IXOTH) {
                    printColoredText(name, "\x1B[32m");  // Green for executables
                }
                else {
                    printColoredText(name, "\x1B[37m");  // White for regular files
                }
            }

            // Free the memory allocated for each name
            free(names[i]);
        }

        // Free the memory allocated for the array
        free(names);
    }
    else
    {
        printf("Unable to open directory: %s\n", path);
    }
}

void ls_tokenizeCommand(char *string, char (*command_args)[MAX_COMMAND_LENGTH], int *num_tokens)
{
    strtrim(string);
    char *tokens[100]; // Array to store token pointers
    char *token = strtok(string, " "); // Split string by space

    int count = 0;
    int firstToken = 1;
    while (token != NULL) {
        if ( (strcmp(token,"ls") != 0 && strcmp(token,"peek") != 0 && firstToken == 1) && token[0] != '-' || token[1] == '\0') {
            firstToken = 0;
            tokens[count++] = token; // Store non-hyphenated tokens
        }
        token = strtok(NULL, " ");
    }

    // Reconstruct the string from non-hyphenated tokens
    char output[1000] = "";
    for (int i = 0; i < count; i++) {
        strcat(output, tokens[i]);
        if (i < count - 1) {
            strcat(output, " ");
        }
    }
    strtrim(output);

    // Seperate all the files directories and file paths and store them in a array.
    ls_get_args(output);

    return;
}

void ls_call_V2(int flag_a, int flag_l)
{
    // printf("%d %d %d %s\n",flag_a,flag_l,numArgs,command_args[0]);
    if (numArgs == 1)
    {
        strcpy(command_args[1], curr_cwd);
        numArgs++;
    }
    // Executing ls at each location
    for (int i = 1; i < numArgs; i++)
    {
        printf("%s :\n", command_args[i]);
        // if the path is a direct Path
        listFilesInDirectory(command_args[i], flag_a, flag_l);
        printf("\n\n");
        // Or its a Relative path
    }

    return;
}

void ls_call()
{
    int flag_a = 0;
    int flag_l = 0;

    for (int i = 0; i < numArgs; i++)
    {
        if (strcmp(command_args[i], "-l") == 0)
        {
            flag_l = 1;
        }
        else if (strcmp(command_args[i], "-a") == 0)
        {
            flag_a = 1;
        }
        else if (strcmp(command_args[i], "-la") == 0)
        {
            flag_a = 1;
            flag_l = 1;
        }
        else if (strcmp(command_args[i], "-al") == 0)
        {
            flag_a = 1;
            flag_l = 1;
        }
    }

    for (int i = 0; i < numArgs; i++)
    {
        // if (command_args[i][0] == '-')
        // {
        //     continue;
        // }
        // if its not a valid directory or file

        // either ../ or ./ or / or Direct (Directory or file)

        // .. ==> New Path => Check       File or Directory
        // ./ ==> New Path => Check       File or Directory
        // Direct                         File or Directory
        // Find a new absolute path for all the three cases

        if (chdir(command_args[i]) == 0)
        {
            printf("correct = %s\n", command_args[i]);
        }
        else
        {
            printf("Incorrect = %s\n", command_args[i]);
        }
        // chk dir
    }

    DIR *dir = opendir(curr_cwd);
    if (dir == NULL)
    {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    // Calculate total file size for -l flag
    off_t total_size = 0;

    // Array to store file details
    const int MAX_FILES = 1024;
    struct FileInfo file_details[MAX_FILES];
    int num_files = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        // Exclude hidden files starting with "."
        if (entry->d_name[0] == '.' && flag_a == 0)
        {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s", entry->d_name);

        stat(path, &file_stat);

        // Get file permissions
        char permissions[11];
        permissions[0] = (S_ISDIR(file_stat.st_mode)) ? 'd' : '-';
        permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
        permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
        permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
        permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
        permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
        permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
        permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
        permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
        permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
        permissions[10] = '\0';

        // Get owner and group names
        struct passwd *owner = getpwuid(file_stat.st_uid);
        struct group *group = getgrgid(file_stat.st_gid);

        // Get last modified time
        char mod_time[20];
        strftime(mod_time, sizeof(mod_time), "%b %d %H:%M", localtime(&file_stat.st_mtime));

        // Store file details in the structure
        struct FileInfo file_info;
        strncpy(file_info.name, entry->d_name, sizeof(file_info.name));
        strncpy(file_info.permissions, permissions, sizeof(file_info.permissions));
        file_info.nlinks = file_stat.st_nlink;
        strncpy(file_info.owner, owner->pw_name, sizeof(file_info.owner));
        strncpy(file_info.group, group->gr_name, sizeof(file_info.group));
        file_info.size = file_stat.st_size;
        strncpy(file_info.mod_time, mod_time, sizeof(file_info.mod_time));

        // Calculate total file size
        total_size += file_stat.st_size;

        // Add file details to the array
        file_details[num_files] = file_info;
        num_files++;
    }

    if (flag_l != 0)
    {
        printf("total %ld\n", total_size / 1024);
    }

    // Sort file details in alphabetical order
    qsort(file_details, num_files, sizeof(struct FileInfo), compare_names);

    // Print sorted file details
    for (int i = 0; i < num_files; i++)
    {
        if (flag_l == 0)
        {
            printf("%s\t", file_details[i].name);
        }
        else
        {
            printf("%s\t%ld\t%s\t%s\t%ld\t%s\t%s\n", file_details[i].permissions, file_details[i].nlinks,
                   file_details[i].owner, file_details[i].group, file_details[i].size,
                   file_details[i].mod_time, file_details[i].name);
        }
    }
    printf("\n");

    closedir(dir);
}