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
  for (int i = 0; i < 32; i++) {
    printf("%-3s\t0x%08x\t%d\n", regs[i], cpu.gpr[i], cpu.gpr[i]);
  }
  printf("pc \t0x%08x\t%d\n", cpu.pc, cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success) {
    *success = true;
  if (strcmp(s, "zero") == 0 || strcmp(s, "0") == 0) return 0;
  if (strcmp(s, "ra") == 0) return cpu.gpr[1];
  if (strcmp(s, "sp") == 0) return cpu.gpr[2];
  if (strcmp(s, "gp") == 0) return cpu.gpr[3];
  if (strcmp(s, "tp") == 0) return cpu.gpr[4];
  if (strcmp(s, "t0") == 0) return cpu.gpr[5];
  if (strcmp(s, "t1") == 0) return cpu.gpr[6];
  if (strcmp(s, "t2") == 0) return cpu.gpr[7];
  if (strcmp(s, "s0") == 0) return cpu.gpr[8];
  if (strcmp(s, "s1") == 0) return cpu.gpr[9];
  if (strcmp(s, "a0") == 0) return cpu.gpr[10];
  if (strcmp(s, "a1") == 0) return cpu.gpr[11];
  if (strcmp(s, "a2") == 0) return cpu.gpr[12];
  if (strcmp(s, "a3") == 0) return cpu.gpr[13];
  if (strcmp(s, "a4") == 0) return cpu.gpr[14];
  if (strcmp(s, "a5") == 0) return cpu.gpr[15];
  if (strcmp(s, "a6") == 0) return cpu.gpr[16];
  if (strcmp(s, "a7") == 0) return cpu.gpr[17];
  if (strcmp(s, "s2") == 0) return cpu.gpr[18];
  if (strcmp(s, "s3") == 0) return cpu.gpr[19];
  if (strcmp(s, "s4") == 0) return cpu.gpr[20];
  if (strcmp(s, "s5") == 0) return cpu.gpr[21];
  if (strcmp(s, "s6") == 0) return cpu.gpr[22];
  if (strcmp(s, "s7") == 0) return cpu.gpr[23];
  if (strcmp(s, "s8") == 0) return cpu.gpr[24];
  if (strcmp(s, "s9") == 0) return cpu.gpr[25];
  if (strcmp(s, "s10") == 0) return cpu.gpr[26];
  if (strcmp(s, "s11") == 0) return cpu.gpr[27];
  if (strcmp(s, "t3") == 0) return cpu.gpr[28];
  if (strcmp(s, "t4") == 0) return cpu.gpr[29];
  if (strcmp(s, "t5") == 0) return cpu.gpr[30];
  if (strcmp(s, "t6") == 0) return cpu.gpr[31];

  *success = false;
  return 0;
}
