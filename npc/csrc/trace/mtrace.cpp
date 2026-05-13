#include "../npc.h"
#include <cstdint>

static bool mtrace_enable = false;

void mtrace_init() { mtrace_enable = false; }

void mtrace_set_enable(bool enable) {
  mtrace_enable = enable;
  if (enable)
    printf("[NPC] Mtrace enabled\n");
}

bool mtrace_is_enabled() { return mtrace_enable; }

void mtrace_record_read(uint32_t addr, uint32_t data, int mask) {
  if (!mtrace_enable)
    return;
  trace_printf("[MTRACE] R addr=%08x data=%08x mask=%02x\n", addr, data, mask);
}

void mtrace_record_write(uint32_t addr, uint32_t data, int mask) {
  if (!mtrace_enable)
    return;
  trace_printf("[MTRACE] W addr=%08x data=%08x mask=%02x\n", addr, data, mask);
}