#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "executor.h"
#include "redirect.h"

void runSequential(TaskStore *store, char **taskNames, int count){
    for (int i = 0; i < count; i++) {
        Task *task = taskFind(store, taskNames[i]);
        if (task == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada.\n", taskNames[i]);
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            // processo filho

            if (store->workdir != NULL) {
                if (chdir(store->workdir) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }


            if (applyRedirect(task) == -1) {
                exit(EXIT_FAILURE);
            }
            execvp(task->argv[0], task->argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // processo pai
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                fprintf(stderr, "Tarefa '%s' terminou com código %d.\n",
                        task->name, exit_code);
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Tarefa '%s' foi finalizada por sinal %d.\n",
                    task->name, WTERMSIG(status));
        }
    }

    
}

void runParallel(TaskStore *store, char **taskNames, int count) {
    pid_t *pids = malloc(count * sizeof(pid_t));
    if (pids == NULL) {
        perror("malloc");
        return;
    }

    for (int i = 0; i < count; i++) {
        Task *task = taskFind(store, taskNames[i]);
        if (task == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada.\n", taskNames[i]);
            pids[i] = -1;
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            pids[i] = -1;
            continue;
        }

        if (pid == 0) {
            // processo filho

            if (store->workdir != NULL) {
                if (chdir(store->workdir) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }

            if(applyRedirect(task) == -1){
                exit(EXIT_FAILURE);
            }
            execvp(task->argv[0], task->argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // processo pai
        pids[i] = pid;
    }


    for (int i = 0; i < count; i++) {
        if (pids[i] == -1) {
            continue;
        }

        int status;
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                fprintf(stderr, "Tarefa terminou com código %d.\n", exit_code);
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Tarefa foi finalizada por sinal %d.\n", WTERMSIG(status));
        }
    }

    free(pids);
}