#include "shell.h"
#include "builtin.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void execute(char line[], struct cmdInfo *cmd_info) {
  /*
  Declaración de variables y reserva de memoria
  */
  char *cmd = line;
  struct args *cmd_args = (struct args *)malloc(sizeof(struct args));
  struct execDetails *exec_details =
      (struct execDetails *)malloc(sizeof(struct execDetails));
  int syntax_error = 0;

  /*
  Recolectar argumentos del comando
  */
  int argc = getArgs(cmd_args, cmd);
  char *first_arg[argc + 1];
  cmd = getArgsList(cmd_args, first_arg, argc);
  cleanExec(exec_details);

  // Si no hay argumentos ejecutar comando simple
  if (cmd == NULL) {
    execSimple(first_arg, argc, exec_details);
  } else if (findRedirections(cmd, exec_details) < 0) {
    syntax_error = 1; // si se encuentran errores en los argumentos se informa
                      // el error de sintaxis
  } else {
    // se toman todas las palabras del comando
    cmd = exec_details->command;
    if (cmd != NULL) {
      if (cmd[0] == '|') { // si comienza la línea con un operador de pipe
        cleanArgs(cmd_args);
        if (cmd[1] != '\0') // si no hay más nada en el comando se informa error
                            // de sintaxis
          syntax_error = 1;
        else {
          // si hay más argumentos se recolectan y se repite el proceso de
          // ideintificación de errores
          int argc = getArgs(cmd_args, cmd + 2);
          char *second_argument[argc + 1];
          cmd = getArgsList(cmd_args, second_argument, argc);
          if (findRedirections(cmd, exec_details) < 0)
            syntax_error = 1;
          else {
            // si todo ocurre de manera correcta se manda a ejecutar el pipe (el
            // par de comandos conectados)
            execPipe(first_arg, second_argument, exec_details);
          }
        }
      }
    } else {
      // si hay redirecciónes de entrada o salida se abren los ficheros y se
      // ejecuta el comando simple
      redirect(exec_details);
      execSimple(first_arg, argc, exec_details);
    }
  }
  // una vez ejectado todo se libera la memoria ocupada por los argumentos
  cleanArgs(cmd_args);
  free(cmd_args);
  // Se informa de errores de ejecución o se actualiza el estado
  if (syntax_error) {
    printf("Error: Syntax Error\n");
  } else {
    cmd_info->runtime_error = exec_details->runtime_error;
  }
}

void execSimple(char *line[], int argc, struct execDetails *exec_details) {
  // estado de la subtarea
  int status = 0;
  pid_t son_id; // id del hijo
  son_id = fork();
  int wait_signal = 0;
  // Si el comando tiene el operador & al final se corerá en segundo plano
  if (strcmp(line[argc - 1], "&") == 0) {
    // printf("0\n");
    line[argc - 1] = NULL;
    wait_signal = 1;
  }
  // SI hubo errores a la hora de crear el proceso hijo, informar al usuario
  if (son_id == -1) {
    printf("Error creando el proceso");
  } else if (son_id == 0 && line != NULL) {
    // Si todo ocurrió correctamente, ejecutar el comando e informar de posibles
    // errores en la ejecución del comando
    execvp(line[0], line);
    perror(line[0]);
    exit(2);
  } else {
    // Si el comando no debía ejecutarse en segundo plano, esperar a q termine y
    // guardar los detalles de la ejecución
    if (wait_signal != 1) {
      waitpid(son_id, &status, 0);
      exec_details->runtime_error = status;
    } else {
      // añadir el id a la lista de procesos en segundo plano
      printf("%d\n", son_id);
      add_job(son_id);
      // si debía ejecutarse en segundo plano, solo guardar los detalles
      exec_details->runtime_error = status;
    }
  }
}

void execPipe(char *args1[], char *args2[], struct execDetails *exec_details) {
  int fd[2]; // fd para el pipe
  int pid, status = 0;
  pipe(fd);
  pid = fork();
  if (pid == 0) {
    // cerrar el fd q no se va a utilizar
    close(fd[0]);
    // verificar que no haya redireccióne de entrada para este comando
    if (exec_details->in_file != NULL)
      redirect_input(exec_details->in_file);
    close(
        1); // retornar a la entrada estándar (en caso de q se hubiera abierto)
    // abrir el fd de escritura
    dup(fd[1]);
    close(fd[1]);
    // ejecutar el comando
    execvp(args1[0], args1);
    perror(args1[0]);
    exit(2);
  } else {
    close(fd[1]); // cerrar el fd q no se va a utilizar
    pid = fork(); // crear un nuevo subproceso para el segundo comando
    if (pid == 0) {
      close(fd[1]);
      // buscar redirecciones de salida
      if (exec_details->out_file != NULL) {
        if (exec_details->add == 1)
          redirect_output(exec_details->out_file, 1);
        else
          redirect_output(exec_details->out_file, 0);
      }
      // leer los datos de entrada envviados por el comando anterior
      close(0);
      dup(fd[0]);
      close(fd[0]);
      // ejecutar el comando
      execvp(args2[0], args2);
      perror(args2[0]);
      exit(2);
    } else
      close(fd[0]);
  }
  // esperar que termine la ejecución y guardar los detalles de esta
  waitpid(pid, &status, 0);
  exec_details->runtime_error = status;
  waitpid(pid, &status, 0);
  exec_details->runtime_error += status;
}

int getArgs(struct args *args, char *cmd) {
  int i = 0;
  struct args *cmd_args = args;
  cmd = strtok(cmd, " ");
  // procesar la cadena recibida y dividirla en tokens separados por espacio
  while (cmd != NULL) {
    // Si hay operadores de pipe o redirección de entrada o salida terminar el
    // procesamiento pues ya no hay más argumentos del comando
    
    if (cmd[0] == '>' || cmd[0] == '<' || cmd[0] == '|') {
      break;
    }
    // añadir los comandos a cmd_args y crear un nuevo elemento para guardar el
    // próximo argumento
    cmd_args->arg = cmd;
    cmd_args->next = (struct args *)malloc(sizeof(struct args));
    cmd_args = cmd_args->next;
    cmd = strtok(NULL, " ");
    i++;
  }
  // añadir un último argumento null
  cmd_args->arg = cmd;
  cmd_args->next = NULL;
  // modificar el argumento help para ejecutar el binario de help.c
  if (strncmp(args->arg, "help", 4) == 0)
    args->arg = (char *)"./help";
  // retornar la cantidad de argumentos del comando
  return i;
}
char *getArgsList(struct args *args, char **first_arg, int argc) {
  // procesar todos los argumentos obtenidos en getArgs y añadirlos
  // a una lista de strings
  int i = 0;
  while (i < argc) {
    first_arg[i] = args->arg;
    args = args->next;
    i++;
  }
  first_arg[i] = NULL;
  insertNULL(first_arg[i - 1]);
  return args->arg;
}
void cleanExec(struct execDetails *exec_details) {
  // limpiar los detalles de la ejecución
  exec_details->add = 0;
  exec_details->command = NULL;
  exec_details->runtime_error = 0;
  exec_details->in_file = NULL;
  exec_details->out_file = NULL;
}
int findRedirections(char *cmd, struct execDetails *exec_details) {
  if (cmd != NULL) {
    // si hay redirección de entrada (<)
    if (cmd[0] == '<') {
      if (strlen(cmd) > 1) {
        return -1;
      }
      // procesar la cadena para obtener el archivo q se debe abrir
      cmd = strchr(cmd, ' ');
      cmd = strtok(NULL, " ");
      if (cmd == NULL)
        return -1;
      // añadir el fichero a exec details para cuando se vaya a ejecutar el
      // comando
      exec_details->in_file = cmd;
      insertNULL(cmd);
      cmd = strtok(NULL, " ");
    }
    // de igual manera se hace con los ficheros de salida
    else if (cmd[0] == '>') {
      if (cmd[1] == '>') {
        if (strlen(cmd) > 2) {
          return -1;
        }
        exec_details->add =
            1; // si se debe abrir el fichero para añadir datos y no
               // sobreescribir también se informa en el exec_details
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
void cleanArgs(struct args *args) {
  // liberar memoria ocupada por los argumentos
  struct args *arguments = args->next;
  while (arguments != NULL) {
    struct args *current_arg = arguments;
    arguments = arguments->next;
    free(current_arg);
  }
}
void redirect(struct execDetails *exec_details) {
  // abrir los respectivos ficheros de entrada y/o salida
  if (exec_details->in_file != NULL)
    redirect_input(exec_details->in_file);
  else if (exec_details->out_file != NULL) {
    redirect_output(exec_details->out_file, 1);
  } else
    redirect_output(exec_details->out_file, 0);
}
void redirect_input(char *file) {
  int fd = open(file, O_RDONLY);
  dup2(fd, 0); 
  close(fd);
}
void redirect_output(char *file, int append) {
  if (!append) {
    int fd;
    fd = creat(file, 0777); // crea y abre el fichero con todos los permisos
    close(1);               // cierro la entrada estandar
    dup(fd);
  } else {
    int fd;
    fd = open(file, O_CREAT | O_WRONLY | O_APPEND,
              0777); // creo el fichero o se abre si ya existe
    close(1);        // cierro la salida estandar
    dup(fd);
  }
}
