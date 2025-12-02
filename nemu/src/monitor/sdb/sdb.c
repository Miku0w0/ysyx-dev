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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "expr.h"
#include "watchpoint.h"  
#include <memory/paddr.h>  

static int is_batch_mode = false;  // sdb模式参数，false为交互式，true批量式

void init_regex();   // 初始化 正则表达式引擎，与 表达式求值 有关
void init_wp_pool(); // 创建并初始化 WP结构体 数组，连接为空闲链表

/**  
 * readline输入封装，我们使用readline库来提供更灵活的标准输入（stdin）读取方式
 */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {  // readline在堆上分配内存，要释放，以防内存泄漏
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("\033[1;32m(nemu)\033[0m ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

/**********************************命令处理函数8个********************************/
static int cmd_help(char *args); // help在下面命令表定义之后实现

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) {
  int n = 1;  
  if (args != NULL) {
    sscanf(args, "%d", &n);
  }
  cpu_exec(n);
  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("Usage: x N EXPR\n");
    return 0;
  }

  int n;
  unsigned int addr;
  char expr[32];

  if (sscanf(args, "%d %s", &n, expr) != 2) {
    printf("Usage: x N EXPR\n");
    return 0;
  }

  sscanf(expr, "%x", &addr);  // 简化版，只支持十六进制数

  for (int i = 0; i < n; i++) {
    uint32_t data = paddr_read(addr + i * 4, 4);
    printf("0x%08x: 0x%08x\n", addr + i * 4, data);
  }

  return 0;
}

static int cmd_p(char *args) {
  expr_debug = true;       // 打开调试输出
  if (args == NULL) {
    printf("Usage: p EXPR\n");
    return 0;
  }
  bool success = true;
  word_t result = expr(args, &success);
  expr_debug = false;  // 关闭调试输出
  if (success) {
    printf("Result of '%s' = %d\n", args, (int32_t)result); // 只显示十进制
  } else {
    printf("Invalid expression: %s\n", args);
  }

  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    printf("Usage: info r Or info w\n");
    return 0;
  }

  if (strcmp(args, "r") == 0) {
    isa_reg_display();
  }
  else if (strcmp(args, "w") == 0) {
    info_wp();
  }
  else {
    printf("Unknown subcommand for info: %s\n", args);
  }
  return 0;
}

static int cmd_w(char *args) {
  if (args == NULL) {
    printf("Usage: w EXPR\n");
    return 0;
  }

  WP *wp = new_wp();
  strcpy(wp->expr, args);

  bool success = true;
  wp->last_val = expr(args, &success);
  if (!success) {
    printf("Invalid expression: %s\n", args);
    free_wp(wp);
    return 0;
  }

  printf("Set watchpoint %d: %s = 0x%lx\n", wp->NO, args, wp->last_val);
  return 0;
}

static int cmd_d(char *args) {
  if (args == NULL) {
    printf("Usage: d N\n");
    return 0;
  }

  int no = atoi(args);
  WP *wp = get_wp(no);
  if (wp == NULL) {
    printf("No watchpoint with number %d\n", no);
    return 0;
  }

  free_wp(wp);
  printf("Deleted watchpoint %d\n", no);
  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si",   "Step through N instructions", cmd_si },
  { "info", "Print register or watchpoint info", cmd_info },
  { "x",    "Examine memory", cmd_x },
  { "p",    "Evaluate the expression", cmd_p },
  { "w", "Set a watchpoint for an expression", cmd_w },
  { "d", "Delete a watchpoint by number", cmd_d },
  /* 待办：添加更多命令 */
};

#define NR_CMD ARRLEN(cmd_table) // 计算命令表的元素个数，赋值给NR_CMD

static int cmd_help(char *args) {
  /* 提取第一个参数 */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* 未提供参数 */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}
 /*****************************命令处理函数和命令表********************************/

 /**********************************sdb主循环***********************************/
void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) { // 交互式无限循环与输入
    char *str_end = str + strlen(str); // 输入字符串结束位置

    /** 
     * 提取第一个标记token作为命令 
     */
    char *cmd = strtok(str, " "); // 分割字符串,查找第一个分隔符
    /*x 10 0x80000000变为x\010 0x80000000*/

    /* 如果输入字符串只包含空格或为空，strtok() 返回 NULL。*/
    /* continue语句跳过本次循环的命令处理，重新调用rl_gets()等待用户输入下一条命令。*/
    if (cmd == NULL) { continue; } //

    /** 
     * 将剩余的字符串作为参数处理
     * 这些参数可能需要进一步解析
     */
    char *args = cmd + strlen(cmd) + 1; // 跳过x\0，即10 0x80000000
    if (args >= str_end) { // 说明命令后面没有其他内容
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) { // 遍历所有的已知命令
      if (strcmp(cmd, cmd_table[i].name) == 0) { //找到命令，handler执行
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); } // 错误处理
  }
}

void init_sdb() { // 初始化过程
  /* 编译正则表达式。 */
  init_regex();

  /* 初始化断点池。 */
  init_wp_pool();
}
 /*********************************sdb主循环**********************************/