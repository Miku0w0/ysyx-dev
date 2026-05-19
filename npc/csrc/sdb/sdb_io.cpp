#include "sdb.h"
#include <cstdio>
#include <cstdlib>
#include <readline/history.h>
#include <readline/readline.h>

static char *line_read = NULL;

char *sdb_get_input() {
  if (line_read) {
    free(line_read);
    line_read = NULL;
  }
  line_read = readline("\033[1;32m(npc)\033[0m ");
  if (line_read && *line_read) {
    add_history(line_read);
  }
  return line_read;
}