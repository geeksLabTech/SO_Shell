#include "shell.h"
#include <fcntl.h>

void changeDir(char *cmdLine, struct cmdInfo *cmd_info);
char *runAgain(char *cmdLine, struct cmdInfo *cmd_info);

void add_job(pid_t pid);
void wait_job(pid_t pid);
void print_jobs();
