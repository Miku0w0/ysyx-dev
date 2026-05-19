#include <cstdio>
#include "wp.h"
#include "../expr/expr.h"

extern WP *head;

int check_wp() {
    int triggered = 0;
    for (WP *wp = head; wp != NULL; wp = wp->next) {
        bool success = true;
        word_t new_val = expr(wp->expr, &success);
        if (success && new_val != wp->last_val) {
            printf("\n[Watchpoint %d] %s\n", wp->NO, wp->expr);
            printf("  Old value = 0x%08x (%d)\n", wp->last_val, (int32_t)wp->last_val);
            printf("  New value = 0x%08x (%d)\n", new_val, (int32_t)new_val);
            wp->last_val = new_val;
            triggered++;
        }
    }
    return triggered;
}