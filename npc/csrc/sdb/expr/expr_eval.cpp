#include "expr.h"
#include <cstdio>
#include <cstdlib>

extern "C" {
    word_t isa_reg_str2val(const char *name, bool *success);
    word_t vaddr_read(vaddr_t addr, int len);
}
extern bool check_parentheses(int p, int q);
extern int find_main_op(int p, int q);

word_t eval(int p, int q) {
    if (p > q) return 0;
    // 单个 token
    if (p == q) {
        if (tokens[p].type == TK_NUM || tokens[p].type == TK_HEX) {
            return strtoul(tokens[p].str, NULL, 0);
        }
        if (tokens[p].type == TK_REG) {
            bool success = true;
            return isa_reg_str2val(tokens[p].str + 1, &success);
        }
        return 0;
    }
    // 去掉外层括号
    if (check_parentheses(p, q)) return eval(p + 1, q - 1);
    // 找主运算符
    int op = find_main_op(p, q);
    // 一元运算符
    if (op == -1) {
        if (tokens[p].type == TK_NEG) {
            return -eval(p + 1, q);
        }
        if (tokens[p].type == TK_DETEF) {
            word_t addr = eval(p + 1, q);
            return vaddr_read(addr, 4);
        }
        return 0;
    }
    // 二元运算
    word_t val1 = eval(p, op - 1);
    word_t val2 = eval(op + 1, q);
    
    switch (tokens[op].type) {
        case TK_OR:  return val1 || val2;
        case TK_AND: return val1 && val2;
        case TK_EQ:  return val1 == val2;
        case TK_NEQ: return val1 != val2;
        case TK_ADD: return val1 + val2;
        case TK_SUB: return val1 - val2;
        case TK_MUL: return val1 * val2;
        case TK_DIV: return val2 != 0 ? val1 / val2 : 0;
        default: return 0;
    }
}