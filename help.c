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
> >> > - Para ejecutarlos se realiza primero el analisis del comando y sus argumentos para poblar\n\
         el array de apuntadores que recibe el comando execvp, luego se recorre el resto de la linea\n\
         de comando y se puebla estructura con todos los elementos del redireccionamiento, en caso de existir,\n\
         y se hacen efectivos antes de ejecutar el execvp, es decir, se abren los ficheros segun sea el caso\n\
         y se utilizan las funciones close y dup para redireccionar.\n\
         Ejemplos: $ sort < lista.txt > listaOrdenada.txt\n\
                               ls >> salida.txt\n\
                               sort < lista.txt >> salida.txt\n\n\
     | - Indica un nuevo comando que puede tener argumentos y redireccionamiento (en este caso solo de salida), \n\
         se realiza el analisis sintactico al segundo comando igual que l primero, se crean 2 procesos hijos,\n\
         uno para cada comando y se realizan la apertura y cierre de los file descriptor de lectura y escritura\n\
         en dependencia del proceso activo.\n\
         Ejemplo: $ ls | wc\n\
                   $ ls | wc > salida.txt\n\n\
     # - Se ignora el resto de la linea cuando el comando empieza con #\n\n";

char help[] = "help - Ayuda del Shell\n\
    help sin argumento - Informacion general \n\
    help funcionalidad - Muestra ayuda de la funcionalidad\n\n\
    Para hacer pausa del texto mostrado, concatenar el comando help con el comando more.\n\n\
    Ejemplos: $ help | more\n\
              $ help help| more\n\
              $ help basic \n\n\
    Para su ejecucion se creo el ejecutable help (a partir de help.c),\n\
    se anadio el path a la cadena que contiene el comando a pasar al execvp\n\
    (argumento[0]=\"./help\") y se ejecuta como los comandos externos.\n\n";

char history[] = "history - Se divide en 3 fases:\n\
    -cargar las lineas de comandos guardadas en el fichero history.txt \n\
     (puede crearse .history para que quede oculto). Para ello se reserva memoria para 10 punteros a punteros de char\n\
    -guardar en la estructura (char **) las lineas de comando ejecutadas. Para saber si la ejecucion fue correcta,\n\
     recibo en la variable estado (parametro del wait), el valor enviado por el hijo al padre \n\
     a traves del  parametro del exit. Al llegar a 10 comandos se corren los comandos hacia atras.\n\
    -descargar los comandos en el fichero \n\n\
again - se valida primero si el numero esta en el rango de lineas de comandos almacenadas. De estar correcto\n\
        se accede al elemento <number-1> de estos comandos  y se ejecuta.\n\n";



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
