#include "../npc.h"
#include <cstdarg>

static bool trace_enable = false;
static FILE *trace_file = nullptr;

void trace_init() { 
  trace_file = stdout;
  ftrace_set_enable(false);
}

void trace_set_enable(bool enable) {
  trace_enable = enable;
  if (enable)
    printf("[NPC] Trace enabled\n");
}

bool trace_is_enabled() { return trace_enable; }

void trace_printf(const char *fmt, ...) {
  if (!trace_enable)
    return;

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void trace_display() {
  if (!trace_enable)
    return;
  trace_printf("\n[NPC] Simulation finished\n");
  trace_printf("[NPC] Total instructions: %d\n", get_inst_cnt());
}