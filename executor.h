#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "task.h"

void runSequential(TaskStore *store, char **taskNames, int count);
void runParallel(TaskStore *store, char **taskNames, int count);

#endif