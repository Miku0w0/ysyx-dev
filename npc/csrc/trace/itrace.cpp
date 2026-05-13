#include "../npc.h"

void trace_record() {
  if (!trace_is_enabled())
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

  trace_printf("[%04d] PC:%08x  INST:%08x ", get_inst_cnt(), pc, inst);

  // 写回
  if (wen && waddr != 0) {
    trace_printf("WB: x%02d<-%08x  ", waddr, wdata);
  } else {
    trace_printf("WB: -------------  ");
  }

  // 访存
  if (mem_we) {
    trace_printf("MEMW: [%08x]<-%08x", mem_addr, mem_wdata);
    mtrace_record_write(mem_addr, mem_wdata, 0x0f);
  } else if (is_load) {
    trace_printf("MEMR: [%08x]->%08x", mem_addr, mem_rdata);
    mtrace_record_read(mem_addr, mem_rdata, 0x0f);
  } else {
    trace_printf("MEM: -----------");
  }

  trace_printf("\n");
  fflush(stdout);
}