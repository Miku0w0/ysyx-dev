#include "npc.h"
#include <cstdarg>

static bool trace_enable = false;
static FILE *trace_file = nullptr;

void trace_init() { trace_file = stdout; }

void trace_set_enable(bool enable) {
  trace_enable = enable;
  if (enable)
    printf("[NPC] Trace enabled\n");
}

static void trace_printf(const char *fmt, ...) {
  if (!trace_enable)
    return;

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  if (trace_file && trace_file != stdout) {
    va_start(args, fmt);
    vfprintf(trace_file, fmt, args);
    va_end(args);
  }
}

void trace_record() {
  if (!trace_enable)
    return;

  uint32_t pc = get_pc();
  uint32_t inst = get_inst();
  bool wen = get_wen();
  uint32_t waddr = get_waddr();
  uint32_t wdata = get_wdata();
  bool mem_we = get_mem_we();
  uint32_t mem_addr = get_mem_addr();
  uint32_t mem_wdata = get_mem_wdata();
  uint32_t mem_rdata = get_mem_rdata();
  bool is_load = get_is_load();

  // itrace
  trace_printf("[%04d] PC:%08x  INST:%08x ", get_inst_cnt(), pc, inst);

  // 写回
  if (wen && waddr != 0)
    trace_printf("WB: x%02d<-%08x  ", waddr, wdata);
  else
    trace_printf("WB: -------------  ");

  // 访存
  if (mem_we)
    trace_printf("MEMW: [%08x]<-%08x", mem_addr, mem_wdata);
  else if (is_load)
    trace_printf("MEMR: [%08x]->%08x", mem_addr, mem_rdata);
  else
    trace_printf("MEM: -----------");

  trace_printf("\n");
  fflush(stdout);
}

void trace_display() {
  if (!trace_enable)
    return;
  trace_printf("\n[NPC] Simulation finished\n");
  trace_printf("[NPC] Total instructions: %d\n", get_inst_cnt());
}