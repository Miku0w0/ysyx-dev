#ifndef __SDB_H__
#define __SDB_H__

#include "../npc.h"

// 命令入口
struct cmd_entry {
  const char *name;
  const char *description;
  int (*handler)(char *);
};

// 命令表
const cmd_entry *get_cmd_table();
extern const int NR_CMD;

// 命令函数
int cmd_help(char *args);
int cmd_c(char *args);
int cmd_q(char *args);
int cmd_si(char *args);
int cmd_info(char *args);
int cmd_x(char *args);

// 主循环
void sdb_mainloop();
void sdb_set_batch_mode();

// I/O 函数
char *sdb_get_input();

// 监视点
void init_wp_pool();
int check_watchpoints();
void info_wp();

#endif