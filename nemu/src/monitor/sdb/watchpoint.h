#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct watchpoint {
  int NO;                  // 监视点编号
  struct watchpoint *next; // 指向下一个监视点的指针

  char expr[256];          // 监视的表达式字符串
  uint64_t last_val;       // 监视点的最后一个值
} WP;

void init_wp_pool();         // 初始化监视点池
WP* new_wp();                // 创建一个监视点
void free_wp(WP *wp);        // 释放一个监视点
void info_wp();              // 列出所有监视点
WP* check_watchpoints(void); // 检查监视点是否触发
WP* get_wp(int no);          // 通过编号查找监视点

#endif
