#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

void commandInit(Command *cmd) {
    cmd->type = CMD_UNKNOWN;
    cmd->argv = NULL;
    cmd->argc = 0;
}

void commandFree(Command *cmd) {
    if (cmd->argv != NULL) {
        for (int i = 0; i < cmd->argc; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
    }
    commandInit(cmd);
}

CommandType parseCommandType(const char *token) {
    if (strcmp(token, "task") == 0)     return CMD_TASK;
    if (strcmp(token, "run") == 0)      return CMD_RUN;
    if (strcmp(token, "start") == 0)    return CMD_START;
    if (strcmp(token, "jobs") == 0)     return CMD_JOBS;
    if (strcmp(token, "wait") == 0)     return CMD_WAIT;
    if (strcmp(token, "input") == 0)    return CMD_INPUT;
    if (strcmp(token, "output") == 0)   return CMD_OUTPUT;
    if (strcmp(token, "append") == 0)   return CMD_APPEND;
    if (strcmp(token, "workdir") == 0)  return CMD_WORKDIR;
    if (strcmp(token, "exit") == 0)     return CMD_EXIT;
    return CMD_UNKNOWN;
}

int parseLine(const char *line, Command *cmd) {
    if (line == NULL) {
        return -1;
    }

    while (isspace((unsigned char)*line)) {
        line++;
    }

    if (*line == '\0' || *line == '\n') {
        return -1;
    }

    char *copy = strdup(line);
    if (copy == NULL) {
        return -1;
    }

    char *token = strtok(copy, " \t\n");
    if (token == NULL) {
        free(copy);
        return -1;
    }

    cmd->type = parseCommandType(token);

    int capacity = 4;
    cmd->argv = malloc(capacity * sizeof(char *));
    if (cmd->argv == NULL) {
        free(copy);
        return -1;
    }

    cmd->argc = 0;

    while ((token = strtok(NULL, " \t\n")) != NULL) {
        if (cmd->argc >= capacity) {
            capacity *= 2;
            char **new_argv = realloc(cmd->argv, capacity * sizeof(char *));
            if (new_argv == NULL) {
                free(copy);
                commandFree(cmd);
                return -1;
            }
            cmd->argv = new_argv;
        }

        cmd->argv[cmd->argc] = strdup(token);
        if (cmd->argv[cmd->argc] == NULL) {
            free(copy);
            commandFree(cmd);
            return -1;
        }
        cmd->argc++;
    }

    free(copy);
    return 0;
}