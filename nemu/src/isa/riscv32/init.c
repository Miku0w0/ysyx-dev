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
#include <memory/paddr.h>

/**
 * this is not consistent with uint8_t
 * but it is ok since we do not access the array directly
 * 我们用 32 位整数（uint32_t）来定义这些指令
 * 但这与内存的底层表示（8位字节流，uint8_t）并不完全一致
 * 没关系，因为我们没有直接用img[i]这种方式读取它的值
 * 我们只是用 memcpy将它当作一个原始的、连续的字节块拷贝到内存中
 * 
 * 只要 uint32_t 在宿主系统上是 4 字节，
 * 那么拷贝 5 个 uint32_t到内存中的效果=手动定义 20 个 uint8_t
 * 这样方便代码编写（使用 uint32_t）和符合内存规范（本质是 uint8_t 序列）
 */
static const uint32_t img [] = {
  0x00000297,  // auipc t0,0
  0x00028823,  // sb  zero,16(t0)
  0x0102c503,  // lbu a0,16(t0)
  0x00100073,  // ebreak (used as nemu_trap)
  0xdeadbeef,  // some data
};

static void restart() {
  /* Set the initial program counter. */
  // 设置pc（cpu的起点）为客户程序启动地址
  cpu.pc = RESET_VECTOR;

  /* The zero register is always 0. */
  // 清零寄存器x0，RISC-V规定
  cpu.gpr[0] = 0;
}

void init_isa() {
  /* Load built-in image. */
  // 将 NEMU 内置的测试指令序列 img
  // 完整地拷贝到 NEMU模拟内存的起始目标地址
  // 起始点是客户机cpu开始执行指令的地址 (RESET_VECTOR) 
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

  /* Initialize this virtual computer system. */
  // 设置 CPU 的初始运行状态
  // 使其能够从一个已知的、确定的位置开始执行客户代码
  restart();
}
