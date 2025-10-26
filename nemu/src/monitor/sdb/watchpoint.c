/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[256];   // 监视的表达式字符串
  uint64_t last_val; // 上次求值结果
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/* 申请一个新的监视点 */
WP* new_wp() {
  if (free_ == NULL) {
    printf("No free watchpoints available!\n");
    assert(0);
  }

  WP *wp = free_;
  free_ = free_->next;

  wp->next = head;
  head = wp;

  return wp;
}

/* 释放一个监视点 */
void free_wp(WP *wp) {
  if (head == NULL || wp == NULL) return;

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

/* 打印所有监视点 */
void info_wp() {
  printf("Num\tExpression\t\tLast Value\n");
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    printf("%-4d %-20s 0x%016lx\n", wp->NO, wp->expr, wp->last_val);
  }
}

/* 检查监视点是否触发 */
WP* check_watchpoints() {
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    bool success = true;
    uint64_t new_val = expr(wp->expr, &success);
    if (!success) continue;

    if (new_val != wp->last_val) {
      printf("\n[Watchpoint %d triggered] %s\n", wp->NO, wp->expr);
      printf("    Old value = 0x%lx\n", wp->last_val);
      printf("    New value = 0x%lx\n\n", new_val);
      wp->last_val = new_val;
      return wp; // 返回第一个触发的监视点
    }
  }
  return NULL;
}


WP* get_wp(int no) {
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    if (wp->NO == no) return wp;
  }
  return NULL;
}

