#include "shell.h"
#define HISTORYLEN 10
#define HISTORYFILE "history.log"
void readHistory(struct cmdInfo *cmd_info);
void writeHistory(struct cmdInfo *cmd_info);
void updateHistory(char *cmd, struct cmdInfo *cmd_info);
void printHistory(struct cmdInfo *cmd_info);
void clearHistory(struct cmdInfo *cmd_info);