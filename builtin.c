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

void add_job(pid_t pid) {
  int newcount = 0;
  FILE *fd = fopen("jobs.log", "r");
  char *count = (char *)malloc(sizeof(char) * MAX);
  char *endptr = (char *)malloc(sizeof(char) * MAX);

  int jobcount = 0;
  if (fd) {
    fgets(count, MAX, fd);
    jobcount = strtol(count, &endptr, 10);
    int jobs[jobcount];
    char *temp_char = (char *)malloc(sizeof(char) * MAX);
    for (int i = 0; i < jobcount; i++) {
      fgets(temp_char, MAX, fd);
      printf("%s\n", temp_char);
      jobs[i] = (int)strtol(temp_char, &endptr, 10);
      if (getpgid(jobs[i]) >= 0 && kill(jobs[i], 0) == -1 && errno == ESRCH) {
        continue;
      } else {
        jobs[newcount++] = jobs[i];
      }
    }
    fclose(fd);
    if (!(getpgid(pid) >= 0 && kill(pid, 0) == -1 && errno == ESRCH)) {
      jobs[newcount++] = pid;
    }

    FILE *fd = fopen("jobs.log", "w");
    char *word = (char *)malloc(sizeof(char) * MAX);
    sprintf(word, "%d", newcount - 1);
    fputs(word, fd);
    fputs("\n", fd);
    for (int i = 0; i < newcount; i++) {
      sprintf(word, "%d", jobs[i]);
      fputs(word, fd);
      fputs("\n", fd);
    }
    fclose(fd);
  }
  if (!(getpgid(pid) >= 0 && kill(pid, 0) == -1 && errno == ESRCH)) {

    FILE *fd = fopen("jobs.log", "w");
    char *word = (char *)malloc(sizeof(char) * MAX);
    fputs("1\n", fd);
    sprintf(word, "%d", pid);
    fputs(word, fd);
    fputs("\n", fd);
    fclose(fd);
  }
}

void print_jobs() {
  // printf("1\n");
  int newcount = 0;
  FILE *fd = fopen("jobs.log", "r");
  char *count = (char *)malloc(sizeof(char) * MAX);
  char *endptr = (char *)malloc(sizeof(char) * MAX);

  int jobcount = 0;
  fgets(count, MAX, fd);
  jobcount = strtol(count, &endptr, 10);
  int jobs[jobcount];
  char *temp_char = (char *)malloc(sizeof(char) * MAX);
  for (int i = 0; i < jobcount; i++) {
    fgets(temp_char, MAX, fd);
    jobs[i] = (int)strtol(temp_char, &endptr, 10);
    if (getpgid(jobs[i]) >= 0 && kill(jobs[i], 0) == -1 && errno == ESRCH) {
      continue;
    } else {
      jobs[newcount++] = jobs[i];
    }
  }
  fclose(fd);
  for (int i = 0; i < newcount; i++) {
    printf("%d:[%d]\n", i, jobs[i]);
  }

  fd = fopen("jobs.log", "w");
  char *word = (char *)malloc(sizeof(char) * MAX);
  sprintf(word, "%d", newcount - 1);
  fputs(word, fd);
  fputs("\n", fd);
  for (int i = 0; i < newcount; i++) {
    sprintf(word, "%d", jobs[i]);
    fputs(word, fd);
    fputs("\n", fd);
  }
  fclose(fd);
}

void wait_job(pid_t pid) {
  int *stat = (int *)malloc(sizeof(int));
  waitpid(pid, stat, 0);
}