#include "sdb.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

bool is_batch_mode = false;
void sdb_set_batch_mode() { is_batch_mode = true; }

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  const cmd_entry *cmd_table = get_cmd_table();

  char *str;
  while ((str = sdb_get_input()) != NULL) {
    char *str_end = str + strlen(str);

    char *cmd_name = strtok(str, " ");
    if (cmd_name == NULL)
      continue;

    char *args = cmd_name + strlen(cmd_name) + 1;
    if (args >= str_end)
      args = NULL;

    int i;
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd_name, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd_name);
    }
  }
  
}