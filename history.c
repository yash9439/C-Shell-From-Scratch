#include "history.h"

void readingHistory() {
    FILE *file;
    
    // Concatenate the hidden file name to the current directory
    char hiddenFilePath[1024];
    // printf("%s %s,\n",initial_cwd,hiddenFilePath);
    snprintf(hiddenFilePath, sizeof(hiddenFilePath), "%s/.hiddenfile", initial_cwd);
    // printf("%s %s,\n",initial_cwd,hiddenFilePath);
    // Open the file for reading
    file = fopen(hiddenFilePath, "r");
    if (file == NULL) {
        perror("Error opening the file");
        printf("%s\n",hiddenFilePath);
        return 1;
    }

    // Read lines from the file and store in the array
    char buffer[MAX_COMMAND_LENGTH];
    historyCount = 0;
    for (int i = 0; i < 15 && fgets(buffer, sizeof(buffer), file) != NULL; i++) {
        // Allocate memory for the line and copy the content
        char *line = strdup(buffer);
        if (line == NULL) {
            perror("Memory allocation error");
            break;
        }

        strcpy(commandHistory[historyCount], line);  // Copy the content
        strtrim(commandHistory[historyCount]);
        free(line);  // Free the allocated memory for the line

        historyCount++;
    }

    // Close the file
    fclose(file);
    return;
}

void updateHiddenFile(const char *filename, const char *input)
{
    FILE *file = fopen(filename, "a"); // Open the hidden file in append mode

    if (file == NULL)
    {
        printf("Failed to open the hidden file.\n");
        return;
    }

    fprintf(file, "%s\n", input); // Write the input to the hidden file
    fclose(file);
}

void printHiddenFile(const char *filename)
{
    FILE *file = fopen(filename, "r"); // Open the hidden file in read mode

    if (file == NULL)
    {
        printf("%s\n",filename);
        printf("Failed to open the hidden file.\n");
        return;
    }

    char line[MAX_COMMAND_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s", line); // Print each line from the hidden file
    }

    fclose(file);
}

void deleteFirstLine(const char *filename)
{
    FILE *file = fopen(filename, "r");

    const char *tempFileName = ".tempfile";

    // Construct the full path to the hidden file
    char full_path[strlen(initial_cwd) + strlen(tempFileName) + 2]; // +2 for the slash and null terminator
    snprintf(full_path, sizeof(full_path), "%s/%s", initial_cwd, tempFileName);

    // printf("Yo => %s\n",full_path);

    FILE *tempFile = fopen(full_path, "w");

    if (file == NULL || tempFile == NULL)
    {
        printf("Failed to open the file.\n");
        return;
    }

    char line[MAX_COMMAND_LENGTH];
    int lineCount = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        lineCount++;

        if (lineCount != 1)
        {
            fputs(line, tempFile); // Write lines except the first one to the temporary file
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(filename);                  // Remove the original file
    rename(full_path, filename); // Rename the temporary file to the original file
}

void removeNewline(char *str) {
    int length = strlen(str);
    if (length > 0 && str[length - 1] == '\n') {
        str[length - 1] = '\0'; // Replace newline with null terminator
    }
}

void historyStore(char *string)
{
    const char *hiddenFile = ".hiddenfile";

    // Construct the full path to the hidden file
    char full_path[strlen(initial_cwd) + strlen(hiddenFile) + 2]; // +2 for the slash and null terminator
    snprintf(full_path, sizeof(full_path), "%s/%s", initial_cwd, hiddenFile);
    if(debug) {
        printf("Path of Storing History: %s\n",full_path);
    }
    // printf("Path of Storing History: %s\n",full_path);
    // Open the file in read mode
    FILE *file = fopen(full_path, "r");
    historyCount = 0;
    if (file != NULL)
    {
        char line[MAX_COMMAND_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL)
        {
            strcpy(commandHistory[historyCount % MAX_HISTORY_SIZE], line);
            historyCount++;
        }

        fclose(file);
    }

    strtrim(commandHistory[historyCount-1]);

    if (strcmp(string, commandHistory[historyCount-1]) == 0)
    {
        return;
    }

    if (historyCount >= MAX_HISTORY_SIZE)
    {
        deleteFirstLine(full_path);
    }
    updateHiddenFile(full_path, string);
}