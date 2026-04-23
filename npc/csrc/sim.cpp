#include "npc.h"
#include <verilated.h>

void npc_init(int argc, char **argv) {
  Verilated::commandArgs(argc, argv);
  dut = new Vtop;
  Verilated::traceEverOn(true);
  tfp = new VerilatedVcdC;
  dut->trace(tfp, 99);
  tfp->open("build/obj_dir/Top.vcd");
}

void single_cycle() {
  dut->clk = 0;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);
  dut->clk = 1;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);
}

void reset(int n) {
  dut->reset = 1;
  while (n-- > 0)
    single_cycle();
  dut->reset = 0;
}

void npc_exit() {
  if (tfp) {
    tfp->flush();
    tfp->close();
  }
  dut->final();
  delete dut;
}