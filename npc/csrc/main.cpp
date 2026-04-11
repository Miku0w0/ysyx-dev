#include "svdpi.h"
#include "verilated_vcd_c.h"
#include <Vtop.h>
#include <cstdio>
#include <cstdlib>
#include <verilated.h>

static Vtop *dut;
static VerilatedVcdC *tfp;
static uint64_t sim_time = 0;

static void single_cycle() {
  dut->clk = 0;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);

  dut->clk = 1;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);
}

static void reset(int n) {
  dut->reset = 1;
  while (n-- > 0)
    single_cycle();
  dut->reset = 0;
}

extern "C" void set_ebreak() {
  printf("\n---------- HIT EBREAK ----------\n");
  printf("NPC: Success! 程序执行完毕，正在退出仿真...\n");
  if (tfp)
    tfp->close();
  exit(0); // 直接退出整个仿真程序
}

int main(int argc, char **argv) {
  Verilated::commandArgs(argc, argv);
  dut = new Vtop;
  Verilated::traceEverOn(true); // 开启追踪
  tfp = new VerilatedVcdC;
  dut->trace(tfp, 99); // 追踪深度
  tfp->open("build/obj_dir/Top.vcd"); 

  reset(10);
  while (1) {
    single_cycle();
  }

  dut->final();
  tfp->close();
  return 0;
}
