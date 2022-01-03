#include "builtin.h"
#include "history.h"
#include "prompt.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  // printf("1\n");
  char cmdLine[MAX] = {0};

  int setStdout = dup(1);
  int setStdin = dup(0);

  char *originalCmd = NULL;
  struct cmdInfo *cmd_info = (struct cmdInfo *)malloc(sizeof(struct cmdInfo));

  readHistory(cmd_info);

  while (1) {
    // Restablecer entrada y salida estándar
    close(1);
    dup(setStdout);
    close(0);
    dup(setStdin);

    // printf("%d\n", cmd_info->count);
    // si el comando ejecutado terminó satisfactoriamente se añade a history
    if (cmd_info->runtime_error == 0 && originalCmd != NULL &&
        originalCmd[0] != ' ' && originalCmd[0] != '\n') {
      updateHistory(originalCmd, cmd_info);
    }

    // // prompt y lee nuevo comando
    printPrompt();
    fgets(cmdLine, MAX, stdin);

    // guarda la cadena original
    originalCmd = (char *)malloc(strlen(cmdLine) + 1);
    strcpy(originalCmd, cmdLine);

    if (cmdLine[0] == '\n')
      continue;
    if (cmdLine[0] == '#') {
      cmd_info->runtime_error = 0;
      continue;
    }
    if (strncmp(cmdLine, "exit\n", 5) == 0) {
      updateHistory(originalCmd, cmd_info);
      free(originalCmd);
      writeHistory(cmd_info);
      break;
    }
    if (strncmp(cmdLine, "cd", 2) == 0) {
      if (cmdLine[2] == ' ' || cmdLine[2] == '\n') {
        changeDir(cmdLine, cmd_info);
        continue;
      }
    }
    if (strncmp(cmdLine, "history\n", 8) == 0) {
      updateHistory(originalCmd, cmd_info);
      printHistory(cmd_info);
      continue;
    }
    if (strncmp(cmdLine, "again", 5) == 0) {
      originalCmd = runAgain(cmdLine, cmd_info);
      if (originalCmd == NULL) {
        printf("Numero de comando fuera de rango\n");
        continue;
      }
    }
    execute(cmdLine, cmd_info);
    continue;
  }
  writeHistory(cmd_info);
  exit(EXIT_SUCCESS);
}
