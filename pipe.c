#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "pipe.h"
#include "redirect.h"

void runPipe(TaskStore *store, char **taskNames, int count) {
    if (count < 2) {
        fprintf(stderr, "Erro: pipe precisa de pelo menos 2 tarefas.\n");
        return;
    }

    int (*pipes)[2] = malloc((count - 1) * sizeof(int[2]));
    if (pipes == NULL) {
        perror("malloc");
        return;
    }

    for (int i = 0; i < count - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            for (int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            free(pipes);
            return;
        }
    }

    pid_t *pids = malloc(count * sizeof(pid_t));
    if (pids == NULL) {
        perror("malloc");
        for (int i = 0; i < count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        free(pipes);
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
            for (int j = 0; j < count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (store->workdir != NULL) {
                if (chdir(store->workdir) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }


            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            if (i == 0 && task->inputFile != NULL) {
                int fd = open(task->inputFile, O_RDONLY);
                if (fd >= 0) {
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
            }

            if (i == count - 1 && task->outputFile != NULL) {
                int flags = O_WRONLY | O_CREAT;
                flags |= task->appendMode ? O_APPEND : O_TRUNC;
                int fd = open(task->outputFile, flags, 0644);
                if (fd >= 0) {
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
            }

            execvp(task->argv[0], task->argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        pids[i] = pid;
    }

    // processo pai (pra fechar os pipes)
    for (int i = 0; i < count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < count; i++) {
        if (pids[i] == -1) continue;

        int status;
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode != 0) {
                fprintf(stderr, "Tarefa '%s' terminou com código %d.\n",
                        taskNames[i], exitCode);
            }
        }
    }

    free(pipes);
    free(pids);
}