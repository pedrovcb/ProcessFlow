#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "task.h"
#include "executor.h"
#include "redirect.h"
#include "pipe.h"


void handleTask(TaskStore *store, Command *cmd) {
    if (cmd->argc < 2) {
        fprintf(stderr, "Uso: task <nome> <programa> [argumentos...]\n");
        return;
    }

    int result = taskAdd(store, cmd->argv[0], &cmd->argv[1], cmd->argc - 1);
    if (result == -1) {
        Task *existing = taskFind(store, cmd->argv[0]);
        if (existing != NULL) {
            fprintf(stderr, "Erro: tarefa '%s' já existe.\n", cmd->argv[0]);
        } else {
            fprintf(stderr, "Erro: falha ao cadastrar tarefa '%s'.\n", cmd->argv[0]);
        }
    }
}

void handleRun(TaskStore *store, Command *cmd) {
    if (cmd->argc < 2) {
        fprintf(stderr, "Uso: run <sequential|parallel|pipe> <tarefa1> [tarefa2...]\n");
        return;
    }

    char *mode = cmd->argv[0];
    char **taskNames = &cmd->argv[1];
    int taskCount = cmd->argc - 1;

    if (strcmp(mode, "sequential") == 0) {
        runSequential(store, taskNames, taskCount);
    } else if (strcmp(mode, "parallel") == 0) {
        runParallel(store, taskNames, taskCount);
    } else if (strcmp(mode, "pipe") == 0) {
        runPipe(store, taskNames, taskCount);
    } else {
        fprintf(stderr, "Uso: run <sequential|parallel|pipe> <tarefa1> [tarefa2...]\n");
    }
}

void handleInput(TaskStore *store, Command *cmd) {
    if (cmd->argc != 2) {
        fprintf(stderr, "Uso: input <tarefa> <arquivo>\n");
        return;
    }

    Task *task = taskFind(store, cmd->argv[0]);
    if (task == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada.\n", cmd->argv[0]);
        return;
    }

    free(task->inputFile);

    task->inputFile = strdup(cmd->argv[1]);
    if (task->inputFile == NULL) {
        perror("strdup");
    }
}

void handleOutput(TaskStore *store, Command *cmd) {
    if (cmd->argc != 2) {
        fprintf(stderr, "Uso: output <tarefa> <arquivo>\n");
        return;
    }

    Task *task = taskFind(store, cmd->argv[0]);
    if (task == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada.\n", cmd->argv[0]);
        return;
    }

    free(task->outputFile);

    task->outputFile = strdup(cmd->argv[1]);
    task->appendMode = 0;
    if (task->outputFile == NULL) {
        perror("strdup");
    }
}

void handleAppend(TaskStore *store, Command *cmd) {
    if (cmd->argc != 2) {
        fprintf(stderr, "Uso: append <tarefa> <arquivo>\n");
        return;
    }

    Task *task = taskFind(store, cmd->argv[0]);
    if (task == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada.\n", cmd->argv[0]);
        return;
    }

    free(task->outputFile);

    task->outputFile = strdup(cmd->argv[1]);
    task->appendMode = 1;
    if (task->outputFile == NULL) {
        perror("strdup");
    }
}

void loopInterativo(TaskStore *store) {
    char line[1024];

    while (1) {
        printf("processflow> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        Command cmd;
        commandInit(&cmd);

        if (parseLine(line, &cmd) == -1) {
            commandFree(&cmd);
            continue;
        }

        if (cmd.type == CMD_EXIT) {
            commandFree(&cmd);
            break;
        }

        switch (cmd.type) {
            case CMD_TASK:
                handleTask(store, &cmd);
                break;
            case CMD_RUN:
                handleRun(store, &cmd);
                break;
            case CMD_INPUT:
                handleInput(store, &cmd);
                break;
            case CMD_OUTPUT:
                handleOutput(store, &cmd);
                break;
            case CMD_APPEND:
                handleAppend(store, &cmd);
                break;
            default:
                fprintf(stderr, "Comando desconhecido: %s\n", cmd.argv[0]);
                break;
        }

        commandFree(&cmd);
    }
}

void loopWorkflow(TaskStore *store, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: não foi possível abrir o arquivo '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[1024];

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        Command cmd;
        commandInit(&cmd);

        if (parseLine(line, &cmd) == -1) {
            commandFree(&cmd);
            continue;
        }

        if (cmd.type == CMD_EXIT) {
            commandFree(&cmd);
            break;
        }

        switch (cmd.type) {
            case CMD_TASK:
                handleTask(store, &cmd);
                break;
            case CMD_RUN:
                handleRun(store, &cmd);
                break;
            case CMD_INPUT:
                handleInput(store, &cmd);
                break;
            case CMD_OUTPUT:
                handleOutput(store, &cmd);
                break;
            case CMD_APPEND:
                handleAppend(store, &cmd);
                break;
            default:
                fprintf(stderr, "Comando desconhecido: %s\n", cmd.argv[0]);
                break;
        }

        commandFree(&cmd);
    }

    fclose(fp);
}

int main(int argc, char *argv[]){
    TaskStore store;
    taskStoreInit(&store);

    if(argc == 1){
        loopInterativo(&store);
    } else if (argc == 2){
        loopWorkflow(&store, argv[1]);
    } else{
        fprintf(stderr, "Uso: %s [arquivo.pf]\n", argv[0]);
        taskStoreFree(&store);
        return EXIT_FAILURE;
    }

    taskStoreFree(&store);
    return EXIT_SUCCESS;
}