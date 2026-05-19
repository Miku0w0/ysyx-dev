#include "wp.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
WP *head = NULL;
WP *free_ = NULL;

void init_wp_pool() {
  for (int i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  head = NULL;
  free_ = wp_pool;
}