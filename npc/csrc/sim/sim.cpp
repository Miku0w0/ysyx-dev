#include "../npc.h"
#include "../sdb/wp/wp.h"
#include "../trace/trace.h"

static uint32_t last_pc = 0;
static uint32_t last_inst = 0;

void single_cycle() {
  dut->clk = 0;
  dut->eval();
  if (tfp) tfp->dump(sim_time++);
  dut->clk = 1;
  dut->eval();
  if (tfp) tfp->dump(sim_time++);

  if (!dut->reset) {
    inst_cnt++;
    if (check_wp(last_pc, last_inst) > 0) { wp_triggered = true; }
    uint32_t pc = get_pc();
    uint32_t inst = get_inst();
    if (itrace_enable) itrace_record(pc, inst);
    if (ftrace_enable) ftrace_record(pc, inst);
    if (mtrace_enable) mtrace_record();
    last_pc = pc;
    last_inst = inst;
  }
}

void reset(int n) {
  dut->reset = 1;
  while (n-- > 0)
    single_cycle();
  dut->reset = 0;
}