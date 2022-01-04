# SO SHELL 2021
## Integrantes:
- Javier A. Oramas López (C212)
- Daniel A. Cárdenas Cabrera (C213)

## Descripción:
Implementación en C de una Shell para el SO Linux

## Funcionalidades
- Funcionalidades Básicas (ejecutar comandos, cd, >, <, <<, | )
- history (útimos 10 comandos aunque se puede aumentar modificando el valor de HISTORYLEN en history.h)
- again <num> (ejecuta el elemento <num> en history si este existe)
- exit (cierra la Shell)
- spaces (Ejecuta el comando siempre que sea válido sin importar la cantidad de espacios que tenga el comando)
- & (ejecuta el comando en segundo plano)

## Instalación:
### Make:
`make build`
### Source:
`gcc main.c shell.c prompt.c builtin.c history.c -o main`

`gcc help.c -o help`

## Ejecución:
`./main`

## Desinstalar:
### Make:
`make clean`

### Source:
`rm -rf main help`
