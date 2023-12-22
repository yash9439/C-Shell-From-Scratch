#ifndef _JOBS_H
#define _JOBS_H

#include "headers.h"

void executeJobs(int flag_r, int flag_s);
void executeSig(int jobIndex, int signalNum);
void executeFg(int jobIndex);
void executeBg(int jobIndex);


#endif