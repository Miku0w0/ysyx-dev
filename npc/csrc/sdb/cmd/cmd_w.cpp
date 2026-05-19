#include "../sdb.h"
#include "../wp/wp.h"
#include "../expr/expr.h"

int cmd_w(char *args) {
  if (args == NULL) {
    printf("Usage: w EXPR\n");
    return 0;
  }

  WP *wp = new_wp();
  if (wp == NULL) return 0;

  strcpy(wp->expr, args);

  bool success = true;
  wp->last_val = expr(args, &success);
  if (!success) {
    printf("Invalid expression: %s\n", args);
    free_wp(wp);
    return 0;
  }

  printf("Watchpoint %d: %s = 0x%08x (%d)\n", wp->NO, wp->expr, wp->last_val, (int32_t)wp->last_val);
  return 0;
}