#include "discover.h"
int FilesFound = 0;
char FilePath[1024];


void printfilespresent(char *directory, int d, int f, char *file_to_be_found, char *path_to_home) {
    struct dirent *dirent_pointer;
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    DIR *dh = opendir(directory);
    if (dh == NULL) {
        return;
    }
    dirent_pointer = readdir(dh);
    while (dirent_pointer != NULL) {
        if (strcmp(dirent_pointer->d_name, ".") == 0 || strcmp(dirent_pointer->d_name, "..") == 0) {
            dirent_pointer = readdir(dh);
            continue;
        }
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", directory, dirent_pointer->d_name);
        

        struct stat fileStat;
        if (stat(full_path, &fileStat) == -1) {
            dirent_pointer = readdir(dh);
            continue;
        }

        if (S_ISDIR(fileStat.st_mode)) {
            // if (d) {
            //     printf("%s\n", full_path);
            // }
            // if (!f) {
            //     printfilespresent(full_path, d, f, file_to_be_found, path_to_home);
            // } else {
            //     printfilespresent(full_path, d, f, file_to_be_found, path_to_home); // Include recursive call for -f flag
            // }
            if (d && (strcmp(file_to_be_found, "") == 0 || strcmp(file_to_be_found, dirent_pointer->d_name) == 0)) {
                // printf("%s\n", full_path);
                strcpy(FilePath,full_path);
                FilesFound += 1;
                if (strncmp(full_path, path_to_home, strlen(path_to_home)) != 0) {
                    printf("%s\n", full_path);
                } else {
                    printf(".%s\n", &full_path[strlen(path_to_home)]);
                }
            }
            if (!f) {
                printfilespresent(full_path, d, f, file_to_be_found, path_to_home);
            } else {
                printfilespresent(full_path, d, f, file_to_be_found, path_to_home); // Include recursive call for -f flag
            }
        } else {
            // if (f) {
            //     if (strcmp(file_to_be_found, "") == 0 || strcmp(file_to_be_found, dirent_pointer->d_name) == 0) {
            //         if (strncmp(full_path, path_to_home, strlen(path_to_home)) != 0) {
            //             printf("%s\n", full_path);
            //         } else {
            //             printf(".%s\n", &full_path[strlen(path_to_home)]);
            //         }
            //     }
            // }
            if (f && (strcmp(file_to_be_found, "") == 0 || strcmp(file_to_be_found, dirent_pointer->d_name) == 0)) {
                // printf("%s\n", full_path);
                strcpy(FilePath,full_path);
                FilesFound += 1;
                if (strncmp(full_path, path_to_home, strlen(path_to_home)) != 0) {
                    printf("%s\n", full_path);
                } else {
                    printf(".%s\n", &full_path[strlen(path_to_home)]);
                }
            }
        }

        dirent_pointer = readdir(dh);
    }
    closedir(dh);
}

void discover_command(char *target_dir, int d, int f, char *file_name) {
    char path_to_home[PATH_MAX];
    getcwd(path_to_home, sizeof(path_to_home));

    char resolved_path[PATH_MAX * 2];
    if(debug) {
        printf("Herer=>%s\n",path_to_home);
    }

    if (strcmp(target_dir, ".") == 0 || strcmp(target_dir, "..") == 0 || strcmp(target_dir, "~") == 0) {
        snprintf(resolved_path, sizeof(resolved_path), "%s/%s", path_to_home, target_dir);
    } else {
        realpath(target_dir, resolved_path);
    }

    resolved_path[strlen(resolved_path)] = '\0';
    if(debug) {
        printf("Herer=>%s\n",resolved_path);
    }

    size_t length = strlen(resolved_path);

    if (length > 0) {
        char lastChar = resolved_path[length - 1];

        if (lastChar == '/') {
            resolved_path[length - 1] = '\0'; // Remove the last character by adding a null terminator
        }
    }

    if(debug) {
        printf("%s\n%d\n", resolved_path,strlen(resolved_path));
        printf("%s\n", file_name);
        printf("%s\n", path_to_home);
        printf("%d\n",d);
        printf("%d\n",f);
    }

    printfilespresent(resolved_path, d, f, file_name, path_to_home);
}

void discover(int argc, char *argv[]) {
    int d = 0, f = 0, e = 0;
    char target_dir[1024];
    char target_dir_byUser[1024];
    char file_name[1024] = "";

    strcpy(target_dir,curr_cwd);

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            d = 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            f = 1;
        } else if (strcmp(argv[i], "-e") == 0) {
            e = 1;
        } else {
            if (strcmp(file_name, "") == 0) {
                strcpy(file_name,argv[i]);
            } else if (strcmp(target_dir,curr_cwd) == 0) {
                strcpy(target_dir_byUser,argv[i]);
            } else {
                printf("Invalid command\n");
                return;
            }
        }
    }
    if(d == 1 && f == 1) {
        printf("Invalid flags!\n");
        return;
    }
    if(d == 0 && f == 0) {
        d = 1;
        f = 1;
    }
    if(strcmp(target_dir,".") == 0) {
        strcpy(target_dir,curr_cwd);
    }
    if(strcmp(target_dir,"..") == 0) {
        strcpy(target_dir,curr_cwd);
        strcat(target_dir,"/../");
    }
    if(target_dir_byUser[0] == '/') {
        strcpy(target_dir,target_dir_byUser);
    }
    else {
        strcat(target_dir,"/");
        strcat(target_dir,target_dir_byUser);
    }
    FilesFound = 0;
    discover_command(target_dir, d, f, file_name);
    if(FilesFound == 0) {
        printf("No Match Found\n");
    }
    if(FilesFound == 1) {
        if(debug) {
            printf("=>%s",FilePath);
        }
        if(d == 1 && e == 1) {
            strcpy(prev_cwd, curr_cwd);
            strcpy(curr_cwd, FilePath);
        }
        else if (f == 1 && e == 1) {
            // Open the file in read mode
            FILE *file = fopen(FilePath, "r");

            if (file == NULL) {
                perror("Failed to open the file");
                return 1;
            }

            // Read and print the file content
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                printf("%s", buffer);
            }

            // Close the file
            fclose(file);
        }
    }
}
