#include "sdb.h"
#include <cstdio>
#include <cstring>
#include <readline/history.h>
#include <readline/readline.h>

bool is_batch_mode = false;

void sdb_set_batch_mode() { is_batch_mode = true; }

static char *rl_gets() {
  static char *line_read = NULL;
  if (line_read) {
    free(line_read);
    line_read = NULL;
  }
  line_read = readline("\033[1;32m(npc)\033[0m ");
  if (line_read && *line_read)
    add_history(line_read);
  return line_read;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  const cmd_entry *cmd_table = get_cmd_table();
  char *str;
  while ((str = rl_gets()) != NULL) {
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
        if (cmd_table[i].handler(args) < 0)
          return;
        break;
      }
    }
    if (i == NR_CMD)
      printf("Unknown command '%s'\n", cmd_name);
  }
}