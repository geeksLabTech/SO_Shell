#include "shell.h"
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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