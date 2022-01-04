build:
	gcc main.c shell.c prompt.c builtin.c history.c -o main
	gcc help.c -o help
clean:
	rm -rf main help
