#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char expr[256];
  uint64_t last_val;
} WP;

void init_wp_pool();
WP* new_wp();
void free_wp(WP *wp);
void info_wp();
WP* check_watchpoints(void);
WP* get_wp(int no);

#endif
