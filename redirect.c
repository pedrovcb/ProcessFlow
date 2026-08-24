#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "redirect.h"

int applyRedirect(Task *task){
    //input
    if (task->inputFile != NULL){
        int fd = open(task->inputFile, O_RDONLY);
        if (fd<0){
            fprintf(stderr, "Erro: não foi possível abrir '%s' para leitura: %s\n", task->inputFile, strerror(errno));
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    //output
    if(task->outputFile != NULL){
        int flags = O_WRONLY | O_CREAT;
        flags |= task->appendMode ? O_APPEND : O_TRUNC;

        int fd = open(task->outputFile, flags, 0644);
        if (fd<0){
            fprintf(stderr, "Erro: não foi possível abrir '%s' para escrita: %s\n", task->outputFile, strerror(errno));
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    return 0;
}