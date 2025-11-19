#ifndef __EXPR_H__
#define __EXPR_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t word_t;

word_t expr(char *e, bool *success);

extern bool expr_debug;   // 声明全局变量

#endif
