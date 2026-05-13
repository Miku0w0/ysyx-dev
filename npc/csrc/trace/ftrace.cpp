#include "../npc.h"

bool ftrace_enable = false;
static int call_depth = 0;

void ftrace_set_enable(bool enable) {
  ftrace_enable = enable;
  if (enable)
    printf("[NPC] Ftrace enabled\n");
}

extern bool is_call_inst(uint32_t inst, uint32_t pc, uint32_t &target);
extern bool is_ret_inst(uint32_t inst);

void ftrace_record(uint32_t pc, uint32_t inst) {
  if (!ftrace_enable)
    return;

  uint32_t target;
  if (is_call_inst(inst, pc, target)) {
    for (int i = 0; i < call_depth; i++)
      printf("| ");
    printf("-> 0x%08x\n", target);
    call_depth++;
  } else if (is_ret_inst(inst)) {
    if (call_depth > 0)
      call_depth--;
    for (int i = 0; i < call_depth; i++)
      printf("| ");
    printf("<-\n");
  }
}