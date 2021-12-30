#include "shell.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void execute(char line[], struct cmdInfo cmd_info) {
  char *cmd = line;
  struct args *cmd_args = (struct args *)malloc(sizeof(struct args));
  struct execDetails *exec_details =
      (struct execDetails *)malloc(sizeof(struct execDetails));
  int syntax_error = 0;

  int argc = getArgs(cmd_args, cmd);
  char *first_arg[argc + 1];
  cmd = getArgsList(cmd_args, first_arg, argc);
  cleanExec(exec_details);
  if (cmd == NULL) {
    execSimple(first_arg, argc, exec_details);
  } else if (findRedirections(cmd, exec_details) < 0)
    syntax_error = 1;
  else {
    cmd = exec_details->command;
    if (cmd != NULL) {
      if (cmd[0] == '|') {
        cleanArgs(cmd_args);
        if (cmd[1] != '\0')
          syntax_error = 1;
        else {
          int argc = getArgs(cmd_args, cmd + 2);
          char *second_argument[argc + 1];
          cmd = getArgsList(cmd_args, second_argument, argc);
          if (findRedirections(cmd, exec_details) < 0)
            syntax_error = 1;
        }
      }
    } else {
      redirect(exec_details);
      execSimple(first_arg, argc, exec_details);
    }
  }
  cleanArgs(cmd_args);
  free(cmd_args);
  if (syntax_error) {
    printf("Error: Syntax Error\n");
  } else {
    cmd_info.runtime_error = exec_details->runtime_error;
  }
}

void execSimple(char *line[], int argc, struct execDetails *exec_details) {
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
    // valorar if aquí
    if (strcmp(line[argc], "&\n") == 0) {
      wait(&status);
      exec_details->runtime_error = status;
    }
  }
}

int getArgs(struct args *args, char *cmd) {}
char *getArgsList(struct args *args, char **first_arg, int argc) {}
void cleanExec(struct execDetails *exec_details) {}
int findRedirections(char *cmd, struct execDetails *exec_details) {}
void cleanArgs(struct args *args) {}
void redirect(struct execDetails *exec_details) {}