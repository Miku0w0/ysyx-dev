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

#define NR_WP 32 // 32个监视点结构体
/*
 * 设置WP结构体
 */
typedef struct watchpoint {
  int NO;                  // 监视点编号 
  struct watchpoint *next; // 指向下一个WP的指针
  char expr[256];          // 监视的表达式字符串
  uint64_t last_val;       // 上次求值结果，判断监视点是否被触发
  /* TODO: Add more members if necessary */
  /* 待办：如需扩展，可添加更多成员。 */
} WP;

static WP wp_pool[NR_WP] = {}; // 32个监视点的静态数组
static WP *head = NULL, *free_ = NULL; // 已用链表和空闲链表的头指针

/* 初始化 */
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i; //创建链表
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]); // 连接
  }

  head = NULL; // 开始没有已用监视点
  free_ = wp_pool; // 空闲链表的起点，数组第一个元素
}

/* TODO: Implement the functionality of watchpoint */
/* 申请一个新的监视点 */
WP* new_wp() {
  if (free_ == NULL) { // 错误处理，判断资源是否耗尽
    printf("No free watchpoints available!\n");
    assert(0);
  }

  WP *wp = free_;      // 取走free_指向的WP
  free_ = free_->next; // 并且指向下一个WP

  wp->next = head; // 将取出的WP插入到已用链表的头部
  head = wp;

  return wp;
}

/* 释放一个监视点 */
void free_wp(WP *wp) {
  if (head == NULL || wp == NULL) return;

  WP **indirect = &head; // 用二级指针，相当于竖着的定位箭头，定位head位置
  while (*indirect && *indirect != wp) { // 非空且还不是我们要找的WP
    indirect = &(*indirect)->next; // 改变这个竖着的箭头的位置，直到找到WP
  }
  if (*indirect == wp) {
    *indirect = wp->next; // 从已用链表删除，竖着箭头指向下一个
    wp->next = free_; // 删除的WP插入空闲链表
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
    uint64_t new_val = expr(wp->expr, &success); // 求新的值
    if (!success) continue;

    if (new_val != wp->last_val) { // 比较
      printf("\n[Watchpoint %d triggered] %s\n", wp->NO, wp->expr);
      printf("    Old value = 0x%lx\n", wp->last_val);
      printf("    New value = 0x%lx\n\n", new_val);
      wp->last_val = new_val; // 更新旧值
      return wp; // 返回触发的监视点
    }
  }
  return NULL;
}

/* 根据编号获取监视点 */
WP* get_wp(int no) {
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    if (wp->NO == no) return wp;
  }
  return NULL;
}

