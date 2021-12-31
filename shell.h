
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

#ifndef EXDET
#define EXDET
struct execDetails {

  char *in_file;
  char *out_file;
  int add;
  char *command;
  int runtime_error;
};
#endif

void execute(char line[], struct cmdInfo cmd_info);
void execSimple(char *line[], int argc, struct execDetails *exec_details);
void execPipe(char *args1, char *args2, struct execDetails *exec_details);

void insertNULL(char *string);
int getArgs(struct args *args, char *cmd);
char *getArgsList(struct args *args, char **first_arg, int argc);
int findRedirections(char *cmd, struct execDetails *exec_details);
void redirect(struct execDetails *exec_details);
void redirect_input(char *fd);
void redirect_output(char *fd, int append);

void cleanExec(struct execDetails *exec_details);
void cleanArgs(struct args *args);