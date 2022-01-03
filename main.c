#include "builtin.h"
#include "history.h"
#include "prompt.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  char cmdLine[MAX] = {0}; // Linea a ejecutar

  int setStdout =
      dup(1); // Definir entrada y salida estándar para empezar la ejecución
  int setStdin = dup(0);

  char *originalCmd =
      NULL; // Aquí se guardara la cadena recibida sin ningúna modificación
  struct cmdInfo *cmd_info = (struct cmdInfo *)malloc(
      sizeof(struct cmdInfo)); // Aquí se guardarán las informaciones
                               // relacionadas a la ejecución de la shell

  readHistory(cmd_info); // Cargar del archivo history.log el history

  while (1) {
    // Restablecer entrada y salida estándar
    close(1);
    dup(setStdout);
    close(0);
    dup(setStdin);

    // si el comando ejecutado terminó satisfactoriamente se añade a history
    if (cmd_info->runtime_error == 0 && originalCmd != NULL &&
        originalCmd[0] != ' ' && originalCmd[0] != '\n') {
      updateHistory(originalCmd, cmd_info);
    }

    // prompt y lee nuevo comando
    printPrompt();
    fgets(cmdLine, MAX, stdin);

    // guarda la cadena original
    originalCmd = (char *)malloc(strlen(cmdLine) + 1);
    strcpy(originalCmd, cmdLine);

    // si se recibe esto no sucede nada
    if (cmdLine[0] == '\n')
      continue;
    if (cmdLine[0] == '#') {
      cmd_info->runtime_error = 0;
      continue;
    }

    // Cerrar Shell
    if (strncmp(cmdLine, "exit\n", 5) == 0) {
      updateHistory(originalCmd, cmd_info);
      free(originalCmd);
      writeHistory(cmd_info);
      break;
    }

    // Cambiar de directorio
    if (strncmp(cmdLine, "cd", 2) == 0) {
      if (cmdLine[2] == ' ' || cmdLine[2] == '\n') {
        changeDir(cmdLine, cmd_info);
        continue;
      }
    }

    // Mostrar history
    if (strncmp(cmdLine, "history\n", 8) == 0) {
      updateHistory(originalCmd, cmd_info);
      printHistory(cmd_info);
      continue;
    }

    // Ejecutar último comando nuevamente
    if (strncmp(cmdLine, "again", 5) == 0) {
      originalCmd = runAgain(cmdLine, cmd_info);
      if (originalCmd == NULL) {
        printf("Numero de comando fuera de rango\n");
        continue;
      }
    }

    // En caso de q no se ejecute ningún comando built in, se pasará a la
    // ejecución del comando
    execute(cmdLine, cmd_info);
    continue;
  }

  // cuando termina la ejecución se exporta el history a history.log
  writeHistory(cmd_info);
  exit(EXIT_SUCCESS);
}
