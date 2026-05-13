#include "../npc.h"
#include <cstdint>

bool mtrace_enable = false;

void mtrace_set_enable(bool enable) {
  mtrace_enable = enable;
  if (enable) printf("[NPC] Mtrace enabled\n");
}

void mtrace_record() {
  if (!mtrace_enable) return;
  
  bool mem_we = get_mem_we();
  uint32_t mem_addr = get_mem_addr();
  uint32_t mem_wdata = get_mem_wdata();
  uint32_t mem_rdata = get_mem_rdata();
  bool is_load = get_is_load();

  if (mem_we) {
    printf("[MTRACE] W addr=%08x data=%08x\n", mem_addr, mem_wdata);
    fflush(stdout);
  } 
  else if (is_load) {
    printf("[MTRACE] R addr=%08x data=%08x\n", mem_addr, mem_rdata);
    fflush(stdout);
  }
}