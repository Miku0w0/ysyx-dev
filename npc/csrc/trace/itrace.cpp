#include "../npc.h"

bool itrace_enable = false;

void itrace_set_enable(bool enable) {
  itrace_enable = enable;
  if (enable) printf("[NPC] Itrace enabled\n");
}

void itrace_record(uint32_t pc, uint32_t inst) {
  if (!itrace_enable) return;
  
  bool wen = get_wen();
  uint32_t waddr = get_waddr();
  uint32_t wdata = get_wdata();
  bool mem_we = get_mem_we();
  bool is_load = get_is_load();
  uint32_t mem_addr = get_mem_addr();
  uint32_t mem_wdata = get_mem_wdata();
  uint32_t mem_rdata = get_mem_rdata();

  printf("[%04d] PC:%08x  INST:%08x ", get_inst_cnt(), pc, inst);
  
  // 写回
  if (wen && waddr != 0) printf("WB: x%02d<-%08x  ", waddr, wdata);
  else printf("WB: -------------  ");

  // 访存
  if (mem_we) printf("MEMW: [%08x]<-%08x", mem_addr, mem_wdata);
  else if (is_load) printf("MEMR: [%08x]->%08x", mem_addr, mem_rdata);
  else printf("MEM: -----------");
  
  
  printf("\n");
  fflush(stdout);
}