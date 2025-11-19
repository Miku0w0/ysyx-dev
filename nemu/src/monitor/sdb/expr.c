/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,    // ==
  TK_NEQ,                    // !=
  TK_AND,                    // &&
  TK_OR,                     // ||
  TK_NUM,                    // 数字
  TK_SUB,                    // 减号
  TK_MUL,                    // 乘号
  TK_DIV,                    // 除号
  TK_LPAREN,                 // 左括号
  TK_RPAREN,                 // 右括号
  TK_NEG = 270,              // 负号
  TK_HEX,                    // 十六进制
  TK_REG                     // 寄存器 
    /* TODO: Add more token types */
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},               // spaces
  {"\\+", '+'},                    // plus
  {"==", TK_EQ},                   // equal
  {"!=", TK_NEQ},                  // not equal
  {"&&", TK_AND},                  // logical and
  {"\\|\\|", TK_OR},               // logical or
  {"-", TK_SUB},                   // 减号
  {"\\*", TK_MUL},                 // 乘号
  {"/", TK_DIV},                   // 除号
  {"\\(", TK_LPAREN},              // 左括号
  {"\\)", TK_RPAREN},              // 右括号
  {"0[xX][0-9a-fA-F]+", TK_HEX},   // 十六进制数
  {"[0-9]+", TK_NUM},              // 数字
  { "\\$[a-z0-9]+", TK_REG },      //寄存器
    /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          /* default: TODO(); */
          case TK_NOTYPE:
          break;  // 忽略空格

          case TK_SUB:
            // 判断是负号还是减号
            printf("Processing '-', TK_NEG=%d, TK_SUB=%d\n", TK_NEG, TK_SUB); // 添加调试
            if (nr_token == 0 ||
                (tokens[nr_token - 1].type != TK_NUM &&
                tokens[nr_token - 1].type != TK_RPAREN &&
                tokens[nr_token - 1].type != TK_HEX)) {
              tokens[nr_token].type = TK_NEG;  // 一元负号
              printf("Set to TK_NEG: %d\n", TK_NEG); // 添加调试
            } else {
              tokens[nr_token].type = TK_SUB;  // 二元减号
              printf("Set to TK_SUB: %d\n", TK_SUB); // 添加调试
            }
              strncpy(tokens[nr_token].str, substr_start, 1);
              tokens[nr_token].str[1] = '\0';
              nr_token++;
              break;   // <- 必须加 break

          default:
            tokens[nr_token].type = rules[i].token_type;
            int len = substr_len;
            if (len >= sizeof(tokens[nr_token].str))
              len = sizeof(tokens[nr_token].str) - 1;
            strncpy(tokens[nr_token].str, substr_start, len);
            tokens[nr_token].str[len] = '\0';
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static void debug_tokens() {
  printf("=== Token List ===\n");
  for (int i = 0; i < nr_token; i++) {
    printf("%2d: type=%d, str='%s'\n", i, tokens[i].type, tokens[i].str);
  }
}

// ===== 新增代码开始 =====

// 判断表达式两端是否是成对括号
static bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN)
    return false;

  int count = 0;
  for (int i = p + 1; i < q; i++) {
    if (tokens[i].type == TK_LPAREN) count++;
    else if (tokens[i].type == TK_RPAREN) {
      if (count == 0) return false; // 内部括号提前闭合，说明不是最外层括号
      count--;
    }
  }

  return count == 0;
}

// 找主运算符（最低优先级）- 只找二元运算符
static int find_main_op(int p, int q) {
    int pos = -1;
    int min_pri = 100;
    int paren = 0;

    for (int i = p; i <= q; i++) {
        int type = tokens[i].type;

        if (type == TK_LPAREN) { paren++; continue; }
        if (type == TK_RPAREN) { paren--; continue; }
        if (paren > 0) continue;

        // 判断是否是一元负号（在开头或前面是运算符/左括号）
        if (type == TK_SUB) {
            if (i == p || 
                tokens[i-1].type == TK_LPAREN ||
                tokens[i-1].type == '+' ||
                tokens[i-1].type == TK_SUB || 
                tokens[i-1].type == TK_MUL ||
                tokens[i-1].type == TK_DIV) {
                continue; // 这是一元负号，跳过
            }
        }

        int pri;
        switch (type) {
            case TK_OR:  pri = 0; break;// 最低优先级
            case TK_AND: pri = 1; break;
            case TK_EQ:
            case TK_NEQ: pri = 2; break;
            case '+':
            case TK_SUB: pri = 3; break;
            case TK_MUL:
            case TK_DIV: pri = 4; break;
            default: continue;
        }

        if (pri <= min_pri) {
            min_pri = pri;
            pos = i;
        }
    }

    return pos;
}

// 递归计算表达式
static word_t eval(int p, int q) {
    if (p > q) {
        panic("Bad expression: empty subexpression between %d and %d", p, q);
    }

    // 单个 token
    if (p == q) {
        if (tokens[p].type == TK_NUM) return atoi(tokens[p].str);
        if (tokens[p].type == TK_HEX) return strtoul(tokens[p].str, NULL, 16);
        if (tokens[p].type == TK_REG) {
            char reg_name[32];
            strcpy(reg_name, tokens[p].str + 1);
            bool success = true;
            word_t val = isa_reg_str2val(reg_name, &success);
            if (!success) {
                printf("Invalid register name: %s\n", reg_name);
                assert(0);
            }
            return val;
        }
        panic("Unexpected single token at %d: type=%d, str='%s'", p, tokens[p].type, tokens[p].str);
    }

    // 括号包围整个表达式
    if (check_parentheses(p, q)) {
        return eval(p + 1, q - 1);
    }

    // 一元负号（通过位置判断）
    /*    if (tokens[p].type == TK_SUB && 
        (p == 0 || 
         tokens[p-1].type == TK_LPAREN ||
         tokens[p-1].type == '+' ||
         tokens[p-1].type == TK_SUB || 
         tokens[p-1].type == TK_MUL ||
         tokens[p-1].type == TK_DIV)) {
        return -eval(p + 1, q);
    }*/


    // 找主运算符
    int op = find_main_op(p, q);

    if (op == -1) {
          if (tokens[p].type == TK_NEG) { 
            return -eval(p + 1, q);
        }
        panic("No operator found but not a number or unary minus at %d..%d", p, q);
    }

    word_t val1 = eval(p, op - 1);
    word_t val2 = eval(op + 1, q);

    switch (tokens[op].type) {
        case TK_OR:  return val1 || val2;
        case TK_AND: return val1 && val2;
        case TK_EQ:  return val1 == val2;
        case TK_NEQ: return val1 != val2;
        case '+': return val1 + val2;
        case TK_SUB: return val1 - val2;
        case TK_MUL: return val1 * val2;
        case TK_DIV: 
            if (val2 == 0) panic("Division by zero");
            return val1 / val2;
        default: 
            panic("Unknown operator at %d: type=%d, str='%s'", op, tokens[op].type, tokens[op].str);
    }
}


// ===== 新增代码结束 =====

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  debug_tokens(); // 调试输出

  /* TODO: Insert codes to evaluate the expression. */
  /* TODO(); */
  *success = true;
  return eval(0, nr_token - 1);

  /* return 0; */
}
