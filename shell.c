#include "shell.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void execute(char line[], struct cmdInfo cmd_info) {}

void execSimple(char *line[], struct cmdInfo cmd_info) {
  int status = 0;
  pid_t son_id;

  son_id = fork();

  if (son_id == -1) {
    printf("Error creando el proceso");
  } else if (son_id == 0) {
    execvp(line[0], line);
    perror(line[0]);
    exit(2);
  } else {
    if (strcmp(line[sizeof(line) / sizeof(line[0]) - 1], "&\n") == 0) {
      wait(&status);
      cmd_info.runtime_error = status;
    }
  }
}