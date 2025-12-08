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
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  int i;
  for (i = 0; i < 32; i++) {
    // 索引 i: 0=$0, 1=ra, 2=sp, 3=gp, 8=s0, 9=s1
    if (i == 1 || i == 2 || i == 3 || i == 8 || i == 9) {
      // ra, sp, gp, s0, s1 (地址/指针寄存器) -> 十六进制
      printf("%-10s\t0x%08x\t0x%08x\n", regs[i], cpu.gpr[i], cpu.gpr[i]);
    } else { 
      // 其他通用数据寄存器 -> 无符号十进制
      printf("%-10s\t0x%08x\t%u\n", regs[i], cpu.gpr[i], cpu.gpr[i]);
    }
  }

  // PC (地址寄存器) -> 十六进制
  printf("%-10s\t0x%08x\t0x%x\n", "pc", cpu.pc, cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;

  for (int i = 0;i < 32;i ++) {
    if (strcmp(s,regs[i]) == 0) {
      return cpu.gpr[i];
    }
  }

  if(strcmp(s,"pc") == 0) {
    return cpu.pc;
  }

  *success = false;
  return 0;
}
