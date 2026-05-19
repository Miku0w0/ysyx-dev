#include "expr.h"
#include <cstdio>
#include <cstdlib>

const Rule rules[] = {
    {" +", TK_NOTYPE},
    {"\\+", TK_ADD},
    {"==", TK_EQ},
    {"!=", TK_NEQ},
    {"&&", TK_AND},
    {"\\|\\|", TK_OR},
    {"-", TK_SUB},
    {"\\*", TK_MUL},
    {"/", TK_DIV},
    {"\\(", TK_LPAREN},
    {"\\)", TK_RPAREN},
    {"0[xX][0-9a-fA-F]+", TK_HEX},
    {"[0-9]+", TK_NUM},
    {"\\$[a-z0-9]+", TK_REG},
};

const int NR_REGEX = sizeof(rules) / sizeof(rules[0]);
regex_t re[NR_REGEX];

void init_regex() {
    for (int i = 0; i < NR_REGEX; i++) {
        int ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            char error_msg[128];
            regerror(ret, &re[i], error_msg, 128);
            printf("regex compilation failed: %s\n", error_msg);
            exit(1);
        }
    }
}