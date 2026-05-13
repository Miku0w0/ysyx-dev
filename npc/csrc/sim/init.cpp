#include "../npc.h"
#include <getopt.h>
#include <verilated.h>

static bool trace_enabled = false;
static bool ftrace_enabled = false;

void npc_init(int argc, char **argv) {
  Verilated::commandArgs(argc, argv);
  dut = new Vtop;
  Verilated::traceEverOn(true);
  tfp = new VerilatedVcdC;
  dut->trace(tfp, 99);
  tfp->open("build/obj_dir/Top.vcd");
  inst_cnt = 0;
}