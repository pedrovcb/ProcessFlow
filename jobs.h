#ifndef JOBS_H
#define JOBS_H

#include "task.h"
#include <sys/wait.h>

typedef struct {
    int id;
    pid_t pid;
    char *taskName;
    int running;
} Job;

typedef struct {
    Job *jobs;
    int count;
    int capacity;
    int nextId;
} JobTable;

void jobTableInit(JobTable *table);
void jobTableFree(JobTable *table);
int jobAdd(JobTable *table, pid_t pid, const char *taskName);
void jobList(JobTable *table);
int jobWait(JobTable *table, int jobId);
void jobUpdateStatus(JobTable *table);


#endif