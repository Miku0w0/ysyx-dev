#include "expr.h"
#include <cstring>
#include <cstdio>

Token tokens[65536];
int nr_token = 0;
bool expr_debug = false;

bool make_token(char *e) {
    int position = 0;
    nr_token = 0;
    
    while (e[position] != '\0') {
        int i;
        for (i = 0; i < NR_REGEX; i++) {
            regmatch_t pmatch;
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
                int substr_len = pmatch.rm_eo;
                position += substr_len;
                
                if (rules[i].token_type == TK_NOTYPE) break;
                
                // 处理负号（一元 vs 二元）
                if (rules[i].token_type == TK_SUB) {
                    if (nr_token == 0 || (tokens[nr_token - 1].type != TK_NUM &&
                        tokens[nr_token - 1].type != TK_RPAREN &&
                        tokens[nr_token - 1].type != TK_HEX &&
                        tokens[nr_token - 1].type != TK_REG)) {
                        tokens[nr_token].type = TK_NEG;
                    } else {
                        tokens[nr_token].type = TK_SUB;
                    }
                    tokens[nr_token].str[0] = '-';
                    tokens[nr_token].str[1] = '\0';
                    nr_token++;
                    break;
                }
                
                // 处理乘号（解引用 vs 乘法）
                if (rules[i].token_type == TK_MUL) {
                    if (nr_token == 0 || (tokens[nr_token - 1].type != TK_NUM &&
                        tokens[nr_token - 1].type != TK_RPAREN &&
                        tokens[nr_token - 1].type != TK_HEX &&
                        tokens[nr_token - 1].type != TK_REG)) {
                        tokens[nr_token].type = TK_DETEF;
                    } else {
                        tokens[nr_token].type = TK_MUL;
                    }
                    tokens[nr_token].str[0] = '*';
                    tokens[nr_token].str[1] = '\0';
                    nr_token++;
                    break;
                }
                
                // 普通 token
                tokens[nr_token].type = rules[i].token_type;
                int len = substr_len;
                if (len >= (int)sizeof(tokens[nr_token].str)) {
                    len = sizeof(tokens[nr_token].str) - 1;
                }
                strncpy(tokens[nr_token].str, e + position - substr_len, len);
                tokens[nr_token].str[len] = '\0';
                nr_token++;
                break;
            }
        }
        if (i == NR_REGEX) return false;
    }
    return true;
}

void debug_tokens() {
    printf("=== Token List ===\n");
    for (int i = 0; i < nr_token; i++) {
        printf("%2d: type=%d, str='%s'\n", i, tokens[i].type, tokens[i].str);
    }
}