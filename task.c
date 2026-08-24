#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

void taskStoreInit(TaskStore *store){
    store->tasks = NULL;
    store->count = 0;
    store->capacity = 0;
}

static void taskFreeOne(Task *task) {
    free(task->name);
    free(task->inputFile);
    free(task->outputFile);
    if (task->argv != NULL) {
        for (int i = 0; i < task->argc; i++) {
            free(task->argv[i]);
        }
        free(task->argv);
    }
}

void taskStoreFree(TaskStore *store) {
    for (int i = 0; i < store->count; i++) {
        taskFreeOne(&store->tasks[i]);
    }
    free(store->tasks);
    taskStoreInit(store);
}

int taskAdd(TaskStore *store, const char *name, char **argv, int argc) {
    if (name == NULL || argv == NULL) {
        return -1;
    }

    if (taskFind(store, name) != NULL) {
        return -1;
    }

    if (store->count >= store->capacity) {
        int new_capacity = (store->capacity == 0) ? 4 : store->capacity * 2;
        Task *new_tasks = realloc(store->tasks, new_capacity * sizeof(Task));
        if (new_tasks == NULL) {
            return -1;
        }
        store->tasks = new_tasks;
        store->capacity = new_capacity;
    }

    Task *task = &store->tasks[store->count];

    task->name = strdup(name);
    if (task->name == NULL) {
        return -1;
    }

    task->argc = argc;
    task->argv = malloc((argc + 1) * sizeof(char *));
    if (task->argv == NULL) {
        free(task->name);
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        task->argv[i] = strdup(argv[i]);
        if (task->argv[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(task->argv[j]);
            }
            free(task->argv);
            free(task->name);
            return -1;
        }
    }
    task->argv[argc] = NULL;
    task->inputFile = NULL;
    task->outputFile = NULL;
    task->appendMode = 0;

    store->count++;
    return 0;
}

Task *taskFind(TaskStore *store, const char *name) {
    if (name == NULL) {
        return NULL;
    }

    for (int i = 0; i < store->count; i++) {
        if (strcmp(store->tasks[i].name, name) == 0) {
            return &store->tasks[i];
        }
    }
    return NULL;
}

int taskRemove(TaskStore *store, const char *name) {
    if (name == NULL) {
        return -1;
    }

    for (int i = 0; i < store->count; i++) {
        if (strcmp(store->tasks[i].name, name) == 0) {
            taskFreeOne(&store->tasks[i]);

            for (int j = i; j < store->count - 1; j++) {
                store->tasks[j] = store->tasks[j + 1];
            }

            store->count--;
            return 0;
        }
    }
    return -1;
}

void taskListAll(TaskStore *store) {
    if (store->count == 0) {
        printf("Nenhuma tarefa cadastrada.\n");
        return;
    }

    for (int i = 0; i < store->count; i++) {
        Task *t = &store->tasks[i];
        printf("  %s:", t->name);
        for (int j = 0; j < t->argc; j++) {
            printf(" %s", t->argv[j]);
        }
        printf("\n");
    }
}