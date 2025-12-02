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

static WP wp_pool[NR_WP] = {};         // 32个监视点的静态数组
static WP *head = NULL, *free_ = NULL; // 已用链表 和 空闲链表 的 头指针

/* 初始化 */
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i; // 为每一个WP设置编号
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]); // 连接逻辑
  }

  head = NULL;     // 初始化的时候，没有已用监视点，故为NULL
  free_ = wp_pool; // 空闲链表的起点，指向监视点池的第一个元素
}

/* TODO: Implement the functionality of watchpoint */
/* 申请一个新的监视点 */
WP* new_wp() {
  if (free_ == NULL) { // 错误处理，判断 空闲链表 是否有 空闲的监视点
    printf("No free watchpoints available!\n");
    assert(0);
  }

  WP *wp = free_;      // 取走free_指向的WP
  free_ = free_->next; // 并且指向下一个WP为空闲链表的起点

  wp->next = head;     // 将取出的WP插入到 已用链表 的头部
  head = wp;           // 设置该WP为 已用链表 的起点

  return wp;
}

/* 释放一个监视点 */
void free_wp(WP *wp) {
  /* 没有已用监视点或者指定监视点不存在，直接返回，保证head指向不为空*/
  if (head == NULL || wp == NULL) return;
  /* 定义二级指针，指向 head 指针本身 */
  /* 指向（指向第一个节点的指针）的指针*/
  WP **indirect = &head;
  while (*indirect && *indirect != wp) { // 本质是对节点的地址进行操作比较
    /** 判断当前节点是否为空，且是否为要删除的节点
     *  遍历已用链表，取得（*indirect）->next的地址，二级指针操纵节点的next
     */
    indirect = &(*indirect)->next; //移动 indirect，直到指向要删除节点前一个的next指针
  }
  if (*indirect == wp) {  // 此时找到要删除的节点
    *indirect = wp->next; // 把 删除节点前一个的 next 指向 删除节点后一个，实现删除

    wp->next = free_;     // 把 删除节点WP 的next插入 空闲链表的起点
    free_ = wp;           // 并且设置 删除节点WP 为 空闲链表的新起点
  }
}

/* 打印所有监视点 */
void info_wp() {
  printf("Num\tExpression\t\tLast Value\n");
  for (WP *wp = head; wp != NULL; wp = wp->next) { // 遍历已用链表
    printf("%-4d %-20s 0x%016lx\n", wp->NO, wp->expr, wp->last_val);
  }
}

/* 检查监视点是否触发 */
WP* check_watchpoints() {
  for (WP *wp = head; wp != NULL; wp = wp->next) { // 遍历已用链表
    bool success = true;
    uint64_t new_val = expr(wp->expr, &success);   // 求新的值
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
  for (WP *wp = head; wp != NULL; wp = wp->next) { // 遍历已用链表
    if (wp->NO == no) return wp;
  }
  return NULL;
}

