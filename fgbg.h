#ifndef _FGBG_H
#define _FGBG_H

#include "headers.h"

void handler(int sig);
void ctrlZHandler(int sig);
void interruptHandler(int sig);
void eofHandler(int sig);
void executeForeground(char *command);
void executeBackground(char *command);
void executeKill(int jobIndex);



#endif