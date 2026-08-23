#ifndef TASK_H
#define TASK_H

#include <stddef.h>

typedef struct {
    char *name;
    char **argv;
    int argc;
} Task;

typedef struct {
    Task *tasks;
    int count;
    int capacity;
} TaskStore;

void taskStoreInit(TaskStore *store);
void taskStoreFree(TaskStore *store);
int taskAdd(TaskStore *store, const char *nome, char **argv, int argc);
Task *taskFind(TaskStore *store, const char *name);
int taskRemove(TaskStore *store, const char *name);
void taskListAll(TaskStore *store);

#endif