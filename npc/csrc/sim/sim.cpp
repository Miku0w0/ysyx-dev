#include "../npc.h"

void single_cycle() {
  dut->clk = 0;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);

  dut->clk = 1;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);

  if (!dut->reset) {
    inst_cnt++;
    trace_record();
  }
}

void reset(int n) {
  dut->reset = 1;
  while (n-- > 0)
    single_cycle();
  dut->reset = 0;
}