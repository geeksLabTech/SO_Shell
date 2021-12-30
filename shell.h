
#define MAX 256

#ifndef CMDINFO
#define CMDINFO
struct cmdInfo {
  int count;
  char **firstCmd;
  int runtime_error;
};
#endif

#ifndef ARGS
#define ARGS
struct args {
  char *arg;
  struct args *next;
};
#endif

void insertNULL(char *string);
void execute(char line[], struct cmdInfo cmd_info);
void execSimple(char *line[], struct cmdInfo cmd_info);