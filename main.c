#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "task.h"

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

        // espaço pra ter os comandos dps sla

        printf("Comando não implementado ainda.\n");

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

        // tbm espaço pra os comandos dps
        printf("Comando não implementado ainda.\n");

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