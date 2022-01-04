#include "shell.h"
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void changeDir(char *cmdLine, struct cmdInfo *cmd_info) {
  if (cmdLine[2] == '\n') {
    struct passwd *pw =
        getpwuid(getuid()); // Si no se especifica directorio se deberá cambiar
                            // al home del usuario
    const char *homedir = pw->pw_dir;
    chdir(homedir);
    // printf("moving to %s", homedir);
  } else {
    // Si se especifíca un directorio se cambia a ese directorio
    char *pointer = strtok(cmdLine + 3, " ");
    if (pointer != NULL) {
      insertNULL(pointer);
      if (chdir(pointer) < 0) {
        printf("cd: %s %s\n", pointer, strerror(errno));
        cmd_info->runtime_error = 1;
      }
    }
  }
}

char *runAgain(char *cmdLine, struct cmdInfo *cmd_info) {
  // Busca el comando deseado en history y se retorna, en caso de no ser válida
  // la selección, se retorna null
  char *pointer = strchr(cmdLine, ' ');
  if (pointer == NULL)
    return NULL;
  int number = (int)strtol(pointer + 1, NULL, 10);
  if (number < 1 || number > cmd_info->count)
    return NULL;
  char **commands = cmd_info->firstCmd;
  return commands[number - 1];
}

void insertNULL(char *string) {
  // Sustituye los saltos de línea por null
  int len = strlen(string);
  if (string[len - 1] == '\n')
    string[len - 1] = '\0';
}

void add_job(pid_t pid, struct cmdInfo *cmd_info) {
  printf("1/n");
  int newcount = 0;
  int jobs[cmd_info->job_count];
  for (int i = 0; i < cmd_info->job_count; i++) {
    printf("2/n");
    if (getpgid(cmd_info->jobs[i]) >= 0 && kill(cmd_info->jobs[i], 0) == -1 &&
        errno == ESRCH) {
      continue;
    } else {
      jobs[newcount++] = cmd_info->jobs[i];
    }
  }
  printf("3/n");
  if (!(getpgid(pid) >= 0 && kill(pid, 0) == -1 && errno == ESRCH)) {
    jobs[newcount++] = pid;
  }

  cmd_info->jobs = (int *)malloc(sizeof(int) * newcount);
  for (int i = 0; i < newcount; i++) {
    cmd_info->jobs[i] = jobs[i];
  }
}

void print_jobs(struct cmdInfo *cmd_info) {
  int newcount = 0;
  int jobs[cmd_info->job_count];

  for (int i = 0; i < cmd_info->job_count; i++) {
    if (getpgid(cmd_info->jobs[i]) >= 0 && kill(cmd_info->jobs[i], 0) == -1 &&
        errno == ESRCH) {
      continue;
    } else {
      jobs[newcount++] = cmd_info->jobs[i];
      printf("%d:[%d]\n", i, cmd_info->jobs[i]);
    }
  }

  cmd_info->jobs = (int *)malloc(sizeof(int) * newcount);
  for (int i = 0; i < newcount; i++) {
    cmd_info->jobs[i] = jobs[i];
  }
}