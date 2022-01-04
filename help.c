/* Autores: Lia de la Concepcion Zerquera Ferrer (C212)
                        Kevin Talavera Diaz (C211)
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
&: permitir que se ejecuten comandos en segundo plano (0.5 puntos)\n\
help: ayuda (1 punto)\n\n\
Comandos built-in:\n\
cd: cambia de directorios\n\
exit: termina el Shell\n\
history: lista los ultimos 10 comandos\n\
again <number>: ejecuta el comando <number> de la lista de comandos\n\
help: ayuda\n\
Total: 5 puntos\n\n";

char basic[] = "Detalles de la implementacion\n\n\
El nombre del shell: \n\
prompt de la consola - \"keli $ \". El comando chpro muestra prompt del \"directorio actual $\" y viceversa\n\
Ejemplo: keli $ chpro\n\
         /home/pepe/so/shell $ chpro\n\
         keli $\n\n\
cd -     Para su ejecucion se utilizo la funcion chdir de unistd.h.\n\n\
exit -   La ejecucion del exit se realiza dentro de un ciclo while que solo se interrumpe con este comando \n\
         mediante un break.\n\n\
> >> > - Para ejecutarlos se realiza primero el analisis del comando y sus argumentos para poblar\n\
         el array de apuntadores que recibe el comando execvp, luego se recorre el resto de la linea\n\
         de comando y se puebla estructura con todos los elementos del redireccionamiento, en caso de existir,\n\
         y se hacen efectivos antes de ejecutar el execvp, es decir, se abren los ficheros segun sea el caso\n\
         y se utilizan las funciones close y dup para redireccionar.\n\
         Ejemplos: keli $ sort < lista.txt > listaOrdenada.txt\n\
                               ls >> salida.txt\n\
                               sort < lista.txt >> salida.txt\n\n\
     | - Indica un nuevo comando que puede tener argumentos y redireccionamiento (en este caso solo de salida), \n\
         se realiza el analisis sintactico al segundo comando igual que l primero, se crean 2 procesos hijos,\n\
         uno para cada comando y se realizan la apertura y cierre de los file descriptor de lectura y escritura\n\
         en dependencia del proceso activo.\n\
         Ejemplo1: keli $ ls | wc\n\
                   keli $ ls | wc > salida.txt\n\n\
     # - Se ignora el resto de la linea cuando el comando empieza con #\n\n";

char help[] = "help - Ayuda del Shell\n\
    Detalles de la implementacion\n\n\
    help [funcionalidad]\n\
    help sin argumento - Informacion general \n\
    help funcionalidad - Muestra ayuda de la funcionalidad\n\n\
    Para hacer pausa del texto mostrado, concatenar el comando help con el comando more.\n\n\
    Ejemplos: keli $ help | more\n\
              keli $ help help| more\n\
              keli $ help basic \n\n\
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

char spaces[] =
    "spaces - Para lograr entrada sin restricciones de espacio se utilizaron las funciones strchr y strtok con delimitador \" \"\n\
         Esta funcion ignora los espacios que es lo que se paso como parametro delimitador y va devolviendo las palabras\n\
         que va encontrando hasta que llegue al final de la linea de comando o se interrumpa el ciclo por alguna condicion.\n\n";

struct funct functions[] = {{(char *)"info", info},
                            {(char *)"basic", basic},
                            {(char *)"history", history},
                            {(char *)"help", help},
                            {(char *)"spaces", spaces}};

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
