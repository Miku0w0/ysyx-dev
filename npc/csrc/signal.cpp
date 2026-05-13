#include "npc.h"

// ========== 从 RTL 获取信号值 ==========
// 注意：这些路径需要根据 Verilator 生成的实际名称调整

uint32_t get_pc() { return dut->pc; }

uint32_t get_inst() {
  // 通过 top 模块的 output 端口访问
  // 需要在 top.v 中添加 debug_inst 端口
  return dut->debug_inst;
}

uint32_t get_wdata() { return dut->debug_wdata; }

uint32_t get_waddr() { return dut->debug_waddr; }

bool get_wen() { return dut->debug_wen; }

uint32_t get_mem_addr() { return dut->debug_mem_addr; }

uint32_t get_mem_wdata() { return dut->debug_mem_wdata; }

uint32_t get_mem_rdata() { return dut->debug_mem_rdata; }

bool get_mem_we() { return dut->debug_mem_we; }

bool get_is_load() { return dut->debug_is_load; }

uint32_t get_a0() { return dut->a0; }

uint32_t get_inst_cnt() {
  return inst_cnt; // 在 sim.cpp 中定义
}