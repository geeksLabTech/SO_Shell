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
    int br = 0;
    int cont = 0;
    while (1) {
      // si se recibe esto no sucede nada
      if (cmdLine[0] == '\n') {
        cont = 1;
        break;
      }
      if (cmdLine[0] == '#') {
        cmd_info->runtime_error = 0;
        cont = 1;
        break;
      }

      // Cerrar Shell
      if (strncmp(cmdLine, "exit\n", 5) == 0) {
        updateHistory(originalCmd, cmd_info);
        writeHistory(cmd_info);
        free(originalCmd);
        br = 1;
        break;
      }

      // Cambiar de directorio
      if (strncmp(cmdLine, "cd", 2) == 0) {
        if (cmdLine[2] == ' ' || cmdLine[2] == '\n') {
          changeDir(cmdLine, cmd_info);
          cont = 1;
          break;
        }
      }

      // Mostrar history
      if (strncmp(cmdLine, "history\n", 8) == 0) {
        updateHistory(originalCmd, cmd_info);
        printHistory(cmd_info);
        cont = 1;
        break;
      }

      // Ejecutar último comando nuevamente
      if (strncmp(cmdLine, "again", 4) == 0) {
        originalCmd = runAgain(cmdLine, cmd_info);
        if (originalCmd == NULL) {
          printf("Numero de comando fuera de rango\n");
          cont = 1;
          break;
        }
        strcpy(cmdLine, originalCmd);
        continue;
      }

      if (strncmp(cmdLine, "jobs", 4) == 0) {
        print_jobs();
        cont = 1;
        break;
      }

      if (strncmp(cmdLine, "fg", 2) == 0) {
        char *pointer = strchr(cmdLine, ' ');
        if (pointer == NULL) {
          printf("No se especificó un PID\n");
          break;
        }
        int number = (int)strtol(pointer + 1, NULL, 10);
        wait_job(number);
        cont = 1;
        break;
      }
      break;
    }

    if (cont) {
      continue;
    }
    if (br) {
      break;
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
