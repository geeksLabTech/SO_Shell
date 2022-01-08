/* Autores: Daniel Alejandro Cárdenas Cabrera(C213)
            Javier Alejandro Oramas López(C212)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* declaraciones que se utilizan para la ayuda */
struct funct {
  char *func;
  char *desc;
};

char info[] = "\nIntegrantes: Javier Alejandro Oramas López(C212)\n\
                Daniel Alejandro Cárdenas Cabrera (C213)\n\n\
Funcionalidades implementadas:\n\
basic: funcionalidades basicas (3 puntos)\n\
history: lista ultimos 10 comandos (0.5 puntos)\n\
spaces: permitir cualquier cantidad de espacios entre los comandos y parametros (0.5 puntos)\n\
&,jobs fg: permitir que se ejecuten comandos en segundo plano, mostrar los procesos ejecutandose y esperar por un proceso (0.5 puntos)\n\
help: ayuda (1 punto)\n\n\
Comandos built-in:\n\
cd: cambia de directorios\n\
exit: termina el Shell\n\
history: lista los últimos 10 comandos\n\
again <number>: ejecuta el comando <number> de la lista de comandos\n\
help: ayuda\n\
Total: 5.5 puntos\n\n";

char basic[] = "Detalles de la implementación\n\n\
El nombre del shell: \n\
prompt de la consola - \"<dir> $ \n\
cd -     Si no se especifica directorio se obtiene el directorio home mediante.\n\
         las funciones getpwuid y getuidn, el directorio se cambia mediante\n\
         la funcion chdir\n\n\
exit -   Cuando se detecta el comando exit se ejecuta un break dentro del while que\n\
         mantiene vivo el shell.\n\n\
> >> > - Se revisa y prepara el comando a ejecutar y luego se revisa la existencia de redireccionamientos, \
        se abren los ficheros correspondientes y se ejecuta\n\n\
     | - Recibe un comando que estará formado por 2 comandos, y posiblemente una redirección de salida\
        Se procesan ambos comandos y se extraen los argumentos, se abre un pipe para enviar información entre los procesos\
        Se crean 2 procesos hijos, uno para cada comando y si hay redirección se abre en el último\n\
     # - Se ignora el resto de la linea cuando aparece un caracter #\n\n\
     & - Al colocar este caracter al final de un comando se le dice a la shell que no espere que termine la ejecución para devolver el prompt\n\n\
  jobs - Devuelve una lista con todos los pid de los procesos que se están ejecutando en segundo plano\n\n\
fg <pid> - realiza un waitpid del pid dado, en caso de que sea válido. Luego de esto devuelve el prompt\n\
    ";

char help[] = "help:\n\
    help - Informacion general \n\
    help <argumento> - Muestra ayuda de la funcionalidad\n\n\
    Se puede crear un pipe con el programa more para una lectura más amigable. (help | more)\n\n";
char history[] = "history:\n\
    Lee los comandos ejecutados previamente de history.log, los añade a una estructura  que se encargará tanto\
de actualizarlos como de dar una manera rápida de acceder a ellos tanto para mostrarlos como para ejecutarlos con again\n\n\
again - Verifica la validez del índice del comando a ejecutar y de ser correcto ejecuta nuevamente el comando.\n\n";

struct funct functions[] = {{(char *)"info", info},
                            {(char *)"basic", basic},
                            {(char *)"history", history},
                            {(char *)"help", help}};

int main(int argc, char **argv) {
  if (argc == 1)
    printf("%s", info);
  else
    for (int i = 0; i <= 8; i++) {
      if (strcmp(functions[i].func, argv[1]) == 0) {
        printf("%s", functions[i].desc);
        break;
      }
    }

  return 0;
}
