#include "../sdb.h"

static cmd_entry cmd_table[] = {
    {"help", "Display help", cmd_help},
    {"c", "Continue execution", cmd_c},
    {"q", "Exit NPC", cmd_q},
    {"si", "Step instructions", cmd_si},
    {"info", "Print info", cmd_info},
    {"x", "Examine memory", cmd_x},

};

const int NR_CMD = sizeof(cmd_table) / sizeof(cmd_table[0]);
const cmd_entry *get_cmd_table() { return cmd_table; }