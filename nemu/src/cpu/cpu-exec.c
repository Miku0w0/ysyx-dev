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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include "../../monitor/sdb/watchpoint.h"

#ifdef CONFIG_ITRACE
/* 缓冲区 */
#define IRINGBUF_SIZE 16
typedef struct {
  char log[128];
} ringbuf_entry;

ringbuf_entry iringbuf[IRINGBUF_SIZE];
int iring_ptr = 0;
bool iring_full = false;
#endif

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));

#ifdef CONFIG_WATCHPOINT
  if (check_watchpoints() > 0) // 多个
    nemu_state.state = NEMU_STOP;
#endif
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc; 
  s->snpc = pc; 
  isa_exec_once(s);  
  cpu.pc = s->dnpc; 
  char *p = s->logbuf; 
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc); 
  int ilen = s->snpc - s->pc; 
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst;
#ifdef CONFIG_ISA_x86
  for (i = 0; i < ilen; i ++) {
#else
  for (i = ilen - 1; i >= 0; i --) { 
#endif
    p += snprintf(p, 4, " %02x", inst[i]); 
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4); // x86 8 用RISC-V 4
  int space_len = ilen_max - ilen; // 指令长度和最大程度的差距
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1; // 计算填充总空格数 如'xx '
  memset(p, ' ', space_len); // 填充空格对其反汇编文本
  p += space_len; // 更新p指向的位置

  // 调用反汇编器 目标地址 剩余空间size计算 pc地址 指令机器码 指令长度
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst, ilen);
#ifdef CONFIG_ITRACE
  strcpy(iringbuf[iring_ptr].log, s->logbuf);
  iring_ptr = (iring_ptr + 1) % IRINGBUF_SIZE;
  if (iring_ptr == 0)
    iring_full = true;
#endif
  }


static void execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc); // 执行单条指令 里面的isa_exec_once为 取指 译码 执行
    g_nr_guest_inst ++;    // 指令计数器增加
    trace_and_difftest(&s, cpu.pc); // 调用指令追踪和差分测试 监控逻辑如下
    if (nemu_state.state != NEMU_RUNNING) break; // 状态发生变化 ebreak checkwp
    if (g_nr_guest_inst % 4096 == 0) {
      IFDEF(CONFIG_DEVICE, device_update());
    }
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void display_iringbuf() {
#ifdef CONFIG_ITRACE
  int n = iring_full ? IRINGBUF_SIZE : iring_ptr;
  int i = iring_full ? iring_ptr : 0;
  printf(ANSI_FMT("--- [ Instruction Ring Buffer ] ---\n", ANSI_FG_YELLOW));
  for (int j = 0; j < n; j++) {
    // 如果是最后执行的一条，打印 --> 符号
    if (i == (iring_ptr + IRINGBUF_SIZE - 1) % IRINGBUF_SIZE) {
      printf(ANSI_FMT(" --> %s\n", ANSI_FG_RED), iringbuf[i].log);
    } else {
      printf("     %s\n", iringbuf[i].log);
    }
    i = (i + 1) % IRINGBUF_SIZE;
  }
#endif
}

void assert_fail_msg() {
#ifdef CONFIG_ITRACE
  display_iringbuf();
#endif
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT); // 打印指令标志
  switch (nemu_state.state) { // 检查当前NEMU状态 3个 结束 异常退出 用户退出
    case NEMU_END: case NEMU_ABORT: case NEMU_QUIT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return; // 程序运行完了，回到调用cpuexec处
    default: nemu_state.state = NEMU_RUNNING; // 否则就设置为运行状态
  }

  uint64_t timer_start = get_time(); // 记录开始时间

  execute(n); // 执行循环

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start; // 把执行时间累加到总时间

  switch (nemu_state.state) { // 指令数执行完但是程序未终止 把状态设置为结束
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT: // 若程序已终止 异常退出
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc); // 没有case语句，会穿透到下一个case，fallthrough行为
      // fall through
    case NEMU_QUIT: statistic(); // 打印性能统计信息
  }
}
