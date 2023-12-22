#ifndef __DISCOVER_H
#define __DISCOVER_H

#include "headers.h"

void printfilespresent(char *directory, int d, int f, char *file_to_be_found, char *path_to_home);
void discover_command(char *target_dir, int d, int f, char *file_name);
void discover(int argc, char *argv[]);


#endif