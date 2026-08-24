#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "executor.h"

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
    //placeholder pra adicionar o runParallel dps
}