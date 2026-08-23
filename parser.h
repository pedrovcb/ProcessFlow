#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef enum {
    CMD_UNKNOWN,
    CMD_TASK,
    CMD_RUN,
    CMD_START,
    CMD_JOBS,
    CMD_WAIT,
    CMD_INPUT,
    CMD_OUTPUT,
    CMD_APPEND,
    CMD_WORKDIR,
    CMD_EXIT
} CommandType;

typedef struct {
    CommandType type;
    char **argv;
    int argc;
} Command;

void commandInit(Command *cmd);
void commandFree(Command *cmd);
int parseLine(const char *line, Command *cmd);
CommandType parseCommandType(const char *token);

#endif