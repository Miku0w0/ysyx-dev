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

#include <common.h>

extern uint64_t g_nr_guest_inst;

#ifndef CONFIG_TARGET_AM
FILE *log_fp = NULL;

void init_log(const char *log_file) {
  log_fp = stdout; // 日志文件指针初始化为stdout
  if (log_file != NULL) { // 处理指定的日志文件
    FILE *fp = fopen(log_file, "w"); // 写入模式
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp; // 日志文件指针指向这个新的文件句柄，写入该文件
  }
  Log("Log is written to %s", log_file ? log_file : "stdout"); // 打印日志信息
}

bool log_enable() { // 是否实际输出日志内容：指令跟踪 ITRACE
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
         (g_nr_guest_inst <= CONFIG_TRACE_END), false);
}
#endif
