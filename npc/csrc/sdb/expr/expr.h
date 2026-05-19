#ifndef __EXPR_H__
#define __EXPR_H__

#include <cstdint>
#include <regex.h>

// ========== 类型定义 ==========
typedef uint32_t word_t;
typedef int32_t sword_t;
typedef uint32_t vaddr_t;

// ========== Token 类型枚举 ==========
enum {
    TK_NOTYPE = 256,
    TK_EQ, TK_NEQ, TK_AND, TK_OR,
    TK_NUM, TK_ADD, TK_SUB, TK_MUL, TK_DIV,
    TK_LPAREN, TK_RPAREN, TK_NEG, TK_HEX, TK_REG, TK_DETEF
};

// ========== Token 结构 ==========
struct Token {
    int type;
    char str[32];
};

// ========== 规则结构 ==========
struct Rule {
    const char *regex;
    int token_type;
};

// ========== 全局规则数据 ==========
extern const Rule rules[];
extern const int NR_REGEX;

// ========== 正则表达式存储 ==========
extern regex_t re[];

// ========== 词法分析全局变量 ==========
extern Token tokens[65536];
extern int nr_token;
extern bool expr_debug;

// ========== 函数声明 ==========
void init_regex();
bool make_token(char *e);
void debug_tokens();
word_t expr(char *e, bool *success);

#endif