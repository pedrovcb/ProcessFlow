#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "jobs.h"

void jobTableInit(JobTable *table) {
    table->jobs = NULL;
    table->count = 0;
    table->capacity = 0;
    table->nextId = 1;
}

void jobTableFree(JobTable *table) {
    for (int i = 0; i < table->count; i++) {
        free(table->jobs[i].taskName);
    }
    free(table->jobs);
    jobTableInit(table);
}

int jobAdd(JobTable *table, pid_t pid, const char *taskName) {
    if (table->count >= table->capacity) {
        int newCapacity = (table->capacity == 0) ? 4 : table->capacity * 2;
        Job *newJobs = realloc(table->jobs, newCapacity * sizeof(Job));
        if (newJobs == NULL) {
            perror("realloc");
            return -1;
        }
        table->jobs = newJobs;
        table->capacity = newCapacity;
    }

    int id = table->nextId++;

    table->jobs[table->count].id = id;
    table->jobs[table->count].pid = pid;
    table->jobs[table->count].taskName = strdup(taskName);
    table->jobs[table->count].running = 1;

    table->count++;

    return id;
}

void jobList(JobTable *table) {
    if (table->count == 0) {
        printf("Nenhum job em background.\n");
        return;
    }

    for (int i = 0; i < table->count; i++) {
        Job *j = &table->jobs[i];
        char *status = j->running ? "Running" : "Done";
        printf("[%d] %d\t%s\t%s\n", j->id, j->pid, status, j->taskName);
    }
}

int jobWait(JobTable *table, int jobId) {
    Job *target = NULL;

    for (int i = 0; i < table->count; i++) {
        if (table->jobs[i].id == jobId) {
            target = &table->jobs[i];
            break;
        }
    }

    if (target == NULL) {
        fprintf(stderr, "Erro: job [%d] não encontrado.\n", jobId);
        return -1;
    }

    if (!target->running) {
        printf("Job [%d] já terminou.\n", jobId);
        return 0;
    }

    int status;
    waitpid(target->pid, &status, 0);
    target->running = 0;

    if (WIFEXITED(status)) {
        int exitCode = WEXITSTATUS(status);
        if (exitCode != 0) {
            fprintf(stderr, "Job [%d] terminou com código %d.\n", jobId, exitCode);
        }
    }

    return 0;
}

void jobUpdateStatus(JobTable *table) {
    for (int i = 0; i < table->count; i++) {
        if (table->jobs[i].running) {
            int status;
            pid_t result = waitpid(table->jobs[i].pid, &status, WNOHANG);
            if (result > 0) {
                table->jobs[i].running = 0;
            }
        }
    }
}