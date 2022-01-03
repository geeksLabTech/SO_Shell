
#include "history.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readHistory(struct cmdInfo *cmd_info) {
  // define y reserva la memoria para las variables
  cmd_info->firstCmd = (char **)malloc(sizeof(char *) * HISTORYLEN);
  char line[MAX];
  char **commands = cmd_info->firstCmd;
  // Se abre el fichero history para leer
  FILE *fd = fopen(HISTORYFILE, "r");
  // si no ocurren problemas se lee línea por línea y se guardan en cmd_info
  if (fd) {
    int i = 0;
    while (fgets(line, MAX, fd)) {
      commands[i] = (char *)malloc(strlen(line + 1));
      strcpy(commands[i], line);
    }
    cmd_info->count = i;
    fclose(fd);
  }
}

void writeHistory(struct cmdInfo *cmd_info) {
  // Abre el ficherohostory para escribir.
  // Se recorren todos los comandos guardados en cmd_info y se imprimen en
  // history
  char **commands = cmd_info->firstCmd;
  FILE *fd = fopen(HISTORYFILE, "w");
  for (int i = 0; i < cmd_info->count; i++)
    fputs(commands[i], fd);
  fclose(fd);
}

void updateHistory(char *cmd, struct cmdInfo *cmd_info) {

  char **commands = cmd_info->firstCmd;
  char *line = (char *)malloc(strlen(cmd) + 1);
  strcpy(line, cmd);

  // La lista de comandos que representa el history se corre en una posición y
  // se agrega al final el nuevo comando
  int i = cmd_info->count;
  if (i == HISTORYLEN) {
    int i = 1;
    while (i < HISTORYLEN) {
      commands[i - 1] = commands[i];
      i++;
    }
  } else {
    i++;
    cmd_info->count++;
  }
  commands[i - 1] = line;
}

void printHistory(struct cmdInfo *cmd_info) {
  // Se recorren los comandos y se imprimen en pantalla
  char **commands = cmd_info->firstCmd;
  for (int i = 0; i < cmd_info->count; i++) {
    printf(" %d: %s", i + 1, commands[i]);
  }
  cmd_info->runtime_error = 1;
}

void clearHistory(struct cmdInfo *cmd_info) {
  // Se libera la memoria ocupada por history
  char **commands = cmd_info->firstCmd;
  for (int i = 0; i < cmd_info->count; i++)
    free(commands[i]);
  free(cmd_info);
}