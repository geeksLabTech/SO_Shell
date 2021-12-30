#include <stdio.h>
#include <unistd.h>

void printPrompt() {
  char da[80]; // directorio actual para ponerlo en el prompt*/
  printf("%s $ ", getcwd(da, sizeof(da)));
}