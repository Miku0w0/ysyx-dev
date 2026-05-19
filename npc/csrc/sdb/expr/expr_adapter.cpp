#include "expr.h"
#include "../../npc.h"
#include <cstring>

extern "C" word_t isa_reg_str2val(const char *name, bool *success) {
    const char *reg_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };
    
    if (name[0] == '$') name++;
    
    for (int i = 0; i < 32; i++) {
        if (strcmp(name, reg_names[i]) == 0) {
            *success = true;
            return get_reg_value(i);
        }
    }
    *success = false;
    return 0;
}

extern "C" word_t vaddr_read(vaddr_t addr, int len) {
    return pmem_read(addr);
}