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
          else {
            execPipe(first_arg, second_argument, exec_details);
          }
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

int getArgs(struct args *args, char *cmd) {
  int i = 0;
  struct args *cmd_args = args;
  while (cmd != NULL) {
    if (cmd[0] == '>' || cmd[0] == '<' || cmd[0] == '|') {
      break;
    }
    cmd_args->arg = cmd;
    cmd_args->next = (struct args *)malloc(sizeof(struct args));
    cmd_args = cmd_args->next;
    cmd = strtok(NULL, " ");
    i++;
  }
  cmd_args->arg = cmd;
  cmd_args->next = NULL;
  if (strncmp(args->arg, "help", 4) == 0)
    args->arg = (char *)"./help";
  return i;
}
char *getArgsList(struct args *args, char **first_arg, int argc) {}

void cleanExec(struct execDetails *exec_details) {
  exec_details->add = NULL;
  exec_details->command = NULL;
  exec_details->runtime_error = NULL;
  exec_details->in_file = NULL;
  exec_details->out_file = NULL;
}

int findRedirections(char *cmd, struct execDetails *exec_details) {
  if (cmd != NULL) {
    if (cmd[0] == '<') {
      if (strlen(cmd) > 1) {
        return -1;
      }
      cmd = strchr(cmd, ' ');
      cmd = strtok(NULL, " ");

      if (cmd == NULL)
        return -1;
      exec_details->in_file = cmd;
      insertNULL(cmd);
      cmd = strtok(NULL, " ");
    }

    if (cmd[0] == '>') {
      if (cmd[1] == '>') {
        if (strlen(cmd) > 2) {
          return -1;
        }
        exec_details->add = 1;
      } else if (strlen(cmd) > 1) {
        return -1;
      }
      cmd = strchr(cmd, ' ');
      cmd = strtok(NULL, " ");
      if (cmd == NULL)
        return -1;
      exec_details->out_file = cmd;
      insertNULL(cmd);
      cmd = strtok(NULL, " ");
    }
  }
  exec_details->command = cmd;
  return 0;
}

void cleanArgs(struct args *args) {}
void redirect(struct execDetails *exec_details) {
  if (exec_details->in_file != NULL)
    redirect_input(exec_details->in_file);
  if (exec_details->out_file != NULL) {
    redirect_output(exec_details->out_file, 1);
  } else
    redirect_output(exec_details->out_file, 0);
}
void redirect_input(char *file) {
  int fd;
  close(0);
  fd = open(file, O_RDONLY);
  dup(fd);
}

void redirect_output(char *file, int append) {
  if (!append) {
    int fd;
    fd = creat(file, 0777); // crea y abre el fichero con todos los permisos
    close(1);               // cierro la entrada estandar
    dup(fd);
  } else {
    int fd;
    fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0777); // creo el fichero
    close(1); // cierro la salida estandar
    dup(fd);
  }
}
void execPipe(char *args1, char *args2, struct execDetails *exec_details) {}