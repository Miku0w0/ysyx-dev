#include "../sdb.h"
#include "../wp/wp.h"

int cmd_w(char *args) {
  if (args == NULL) {
    printf("Usage: w EXPR\n");
    return 0;
  }

  WP *wp = new_wp();
  if (wp == NULL)
    return 0;

  strcpy(wp->expr, args);
  wp->last_val = 0;

  printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
  return 0;
}