#include "wp.h"
#include <cstdio>

extern WP *head;
extern WP *free_;

WP *new_wp() {
  if (free_ == NULL) {
    printf("No free watchpoints available!\n");
    return NULL;
  }
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  return wp;
}

void free_wp(WP *wp) {
  if (head == NULL || wp == NULL)
    return;

  WP **indirect = &head;
  while (*indirect && *indirect != wp) {
    indirect = &(*indirect)->next;
  }
  if (*indirect == wp) {
    *indirect = wp->next;
    wp->next = free_;
    free_ = wp;
  }
}

WP *get_wp(int no) {
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    if (wp->NO == no)
      return wp;
  }
  return NULL;
}