#include "npc.h"
#include <verilated.h>

// ========== 全局变量定义 ==========
Vtop *dut = nullptr;
VerilatedVcdC *tfp = nullptr;
uint64_t sim_time = 0;
uint32_t inst_cnt = 0;

static bool trace_enabled = false;

// ========== 初始化 ==========
void npc_init(int argc, char **argv) {
  // 解析命令行参数
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--trace") == 0) {
      trace_enabled = true;
    }
    if (strcmp(argv[i], "--help") == 0) {
      printf("Usage: npc [--trace] <image.bin>\n");
      exit(0);
    }
  }

  Verilated::commandArgs(argc, argv);
  dut = new Vtop;
  Verilated::traceEverOn(true);
  tfp = new VerilatedVcdC;
  dut->trace(tfp, 99);
  tfp->open("build/obj_dir/Top.vcd");

  trace_init();
  trace_set_enable(trace_enabled);

  inst_cnt = 0;
}

// ========== 单周期执行 ==========
void single_cycle() {
  // 时钟低电平
  dut->clk = 0;
  dut->eval();
  if (tfp) tfp->dump(sim_time++);
  // 时钟高电平（上升沿）
  dut->clk = 1;
  dut->eval();
  if (tfp) tfp->dump(sim_time++);
  // 在时钟上升沿后记录
  if (!dut->reset) {
    inst_cnt++;
    trace_record();
  }
}

// ========== 复位 ==========
void reset(int n) {
  dut->reset = 1;
  while (n-- > 0) single_cycle();
  dut->reset = 0;
}

// ========== 退出 ==========
void npc_exit() {
  if (tfp) {
    tfp->flush();
    tfp->close();
  }
  dut->final();
  delete dut;
}