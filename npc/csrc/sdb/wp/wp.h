#ifndef __WP_H__
#define __WP_H__

#include <cstddef>
#include <cstdint>

typedef struct wp {
  int NO;
  struct wp *next;
  char expr[256];
  uint32_t last_val;
} WP;

void init_wp_pool();
WP *new_wp();
void free_wp(WP *wp);
void info_wp();
int check_wp(uint32_t last_pc, uint32_t last_inst);
WP *get_wp(int no);

#endif