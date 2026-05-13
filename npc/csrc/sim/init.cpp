#include "../npc.h"
#include <verilated.h>

static bool trace_enabled = false;
static bool ftrace_enabled = false;

void npc_init(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--trace") == 0) {
      trace_enabled = true;
    }
    if (strcmp(argv[i], "--ftrace") == 0) {
      ftrace_enabled = true;
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
  ftrace_set_enable(ftrace_enabled);
  
  inst_cnt = 0;
}