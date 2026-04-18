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

extern "C" const char *disassemble(int inst);
extern "C" void set_ebreak() {
  printf("\033[1;32m[NPC] HIT EBREAK AT PC = 0x%08x\033[0m\n", dut->pc);

  if (dut->a0 == 0) {
    printf("\033[1;32m[NPC] HIT GOOD TRAP!\033[0m\n");
  } else {
    printf("\033[1;31m[NPC] HIT BAD TRAP! (code: 0x%08x)\033[0m\n", dut->a0);
  }
  if (tfp)
    tfp->close();
  exit(0);
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
