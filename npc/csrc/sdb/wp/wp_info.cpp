#include "wp.h"
#include <cstdio>

extern WP *head;

void info_wp() {
  if (head == NULL) {
    printf("No watchpoints.\n");
    return;
  }
  printf("Num\tExpr\t\t\tLast Value\n");
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    printf("%d\t%s\t\t0x%08x\n", wp->NO, wp->expr, wp->last_val);
  }
}