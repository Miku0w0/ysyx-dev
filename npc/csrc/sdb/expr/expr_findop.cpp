#include "expr.h"

int find_main_op(int p, int q) {
    int pos = -1;
    int min_pri = 100;
    int paren = 0;
    
    for (int i = p; i <= q; i++) {
        int type = tokens[i].type;
        
        if (type == TK_LPAREN) {
            paren++;
            continue;
        }
        if (type == TK_RPAREN) {
            paren--;
            continue;
        }
        if (paren > 0) continue;
        
        // 跳过一元负号（不是二元运算符）
        if (type == TK_SUB) {
            if (i == p || tokens[i-1].type == TK_LPAREN ||
                tokens[i-1].type == TK_ADD || tokens[i-1].type == TK_SUB ||
                tokens[i-1].type == TK_MUL || tokens[i-1].type == TK_DIV) {
                continue;
            }
        }
        
        int pri;
        switch (type) {
            case TK_OR:  pri = 0; break;
            case TK_AND: pri = 1; break;
            case TK_EQ: case TK_NEQ: pri = 2; break;
            case TK_ADD: case TK_SUB: pri = 3; break;
            case TK_MUL: case TK_DIV: pri = 4; break;
            default: continue;
        }
        
        if (pri <= min_pri) {
            min_pri = pri;
            pos = i;
        }
    }
    return pos;
}