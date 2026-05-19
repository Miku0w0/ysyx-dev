#include "../sdb.h"
#include <cstdio>

extern const cmd_entry *get_cmd_table();
extern const int NR_CMD;

int cmd_help(char *args) {
  const cmd_entry *cmd_table = get_cmd_table();
  printf("\nNPC Debugger Commands:\n");
  for (int i = 0; i < NR_CMD; i++) {
    printf("  %-8s - %s\n", cmd_table[i].name, cmd_table[i].description);
  }
  printf("\n");
  return 0;
}