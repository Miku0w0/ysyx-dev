/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "expr.h"
#include <isa.h>
#include <memory/vaddr.h>
#include <regex.h>
#include "expr.h"

/**
 * 我们使用 POSIX 正则表达式函数来处理正则表达式
 * 输入 man regex 命令可获取关于 POSIX 正则表达式函数的更多信息
 */

bool expr_debug = false; // 打印 token 调试信息

enum {             // token_type，与标准ascll 隔离（0~255）
  TK_NOTYPE = 256, // 空格
  TK_EQ,           // ==
  TK_NEQ,          // !=
  TK_AND,          // &&
  TK_OR,           // ||
  TK_NUM,          // 数字
  TK_ADD,          // 加号
  TK_SUB,          // 减号
  TK_MUL,          // 乘号
  TK_DIV,          // 除号
  TK_LPAREN,       // 左括号
  TK_RPAREN,       // 右括号
  TK_NEG = 270,    // 负号
  TK_HEX,          // 十六进制
  TK_REG,          // 寄存器
  TK_DETEF         // 解引用/取值运算符
};

static struct rule { // 正则表达式与token类型的映射表
  const char *regex; // + * ? . ^ $ ( ) [ ] { } \ |
  int token_type;
} rules[] = {
    {" +", TK_NOTYPE},             // spaces " +":匹配一个或多个连续的空格
    {"\\+", TK_ADD},               // plus
    {"==", TK_EQ},                 // equal
    {"!=", TK_NEQ},                // not equal
    {"&&", TK_AND},                // logical and
    {"\\|\\|", TK_OR},             // logical or
    {"-", TK_SUB},                 // 减号
    {"\\*", TK_MUL},               // 乘号
    {"/", TK_DIV},                 // 除号
    {"\\(", TK_LPAREN},            // 左括号
    {"\\)", TK_RPAREN},            // 右括号
    {"0[xX][0-9a-fA-F]+", TK_HEX}, // 十六进制数
    {"[0-9]+", TK_NUM},            // 数字
    {"\\$[a-z0-9]+", TK_REG},      // 寄存器
                                   /* 注意不同规则的优先级顺序 */
};

#define NR_REGEX ARRLEN(rules)    // rules大小计算得NR_REGEX
static regex_t re[NR_REGEX] = {}; // 引用自regex.h，存储rule编译结果

/**
 * 规则会被多次使用
 * 因此，我们在使用前仅编译一次
 */

void init_regex() { // REGEX引擎初始化并处理rules的regex
  int i;
  char error_msg[128]; // 错误信息数组
  int ret;             // 错误码

  for (i = 0; i < NR_REGEX; i++) { // regcomp 是 POSIX C 编译regex的函数
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) { // 0 表示编译成功
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
  /* 正则表达式字符串本身不是机器可直接匹配的
   * regcomp 把文本形式的正则表达式“翻译”为内部机器可用的状态机形式
   * 是方便之后与 regexec 快速匹配，make_token()里面就用了
   * regerror 用于把 regcomp 的错误码 ret 转换为可读的错误信息字符串
   */
}

/*----------------------------------函数区-------------------------------*/
/**
 * token的数据结构，取名Token
 */
typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {}; // 匹配片段数组
static int nr_token __attribute__((used)) = 0;         // token有多少片段
/* __attribute__((used))让编译器不要把这两个变量优化掉*/

/**
 * 词法分析函数
 */
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;

  while (e[position] != '\0') {
    for (i = 0; i < NR_REGEX; i++) { // 遍历rule
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
          pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo; // 参数e的长度

        if (expr_debug) {
          Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
              rules[i].regex, position, substr_len, substr_len, substr_start);
        }

        position += substr_len; // 下一次开始位置

        switch (rules[i].token_type) {
        case TK_NOTYPE: // 忽略空格
          break;

        case TK_SUB: // 判断负号和减号
          // 如果在 token 开头，或前一个 token 不是可以结束一个表达式的类型，
          // 则把'-'视作一元负号（TK_NEG）；否则为二元减号
          // printf("Processing '-', TK_NEG=%d, TK_SUB=%d\n", TK_NEG, TK_SUB);
          if (nr_token == 0 || (tokens[nr_token - 1].type != TK_NUM &&
                                tokens[nr_token - 1].type != TK_RPAREN &&
                                tokens[nr_token - 1].type != TK_HEX &&
                                tokens[nr_token - 1].type != TK_REG)) {
            tokens[nr_token].type = TK_NEG; // 一元负号
            // printf("Set to TK_NEG: %d\n", TK_NEG);
          } else {
            tokens[nr_token].type = TK_SUB; // 二元减号
            // printf("Set to TK_SUB: %d\n", TK_SUB);
          }
          strncpy(tokens[nr_token].str, substr_start, 1);
          tokens[nr_token].str[1] = '\0'; // 字符串结束符
          nr_token++;                     // 匹配的token个数
          break;                          // 退出switch

        case TK_MUL: // 判断乘法和解引用
          // 如果在 token 开头，或前一个 token 不是可以结束一个表达式的类型，
          // 则把 '*' 视作一元解引用（TK_DETEF）；否则为二元乘法（TK_MUL）
          if (nr_token == 0 || (tokens[nr_token - 1].type != TK_NUM &&
                                tokens[nr_token - 1].type != TK_RPAREN &&
                                tokens[nr_token - 1].type != TK_HEX &&
                                tokens[nr_token - 1].type != TK_REG)) {
            tokens[nr_token].type = TK_DETEF; // 一元解引用
          } else {
            tokens[nr_token].type = TK_MUL; // 二元乘法
          }
          tokens[nr_token].str[0] = '*';
          tokens[nr_token].str[1] = '\0'; // 字符串结束符
          nr_token++;                     // 匹配的token个数
          break;                          // 退出switch

        default:
          tokens[nr_token].type = rules[i].token_type; // 确定token的type
          int len = substr_len; // 确定token的长度
          if (len >= sizeof(tokens[nr_token].str)) { // 若读取的len比token长
            len = sizeof(tokens[nr_token].str) - 1;  // 截断，并留一位补'\0'
          }
          strncpy(tokens[nr_token].str, substr_start, len);
          tokens[nr_token].str[len] = '\0'; // 字符串结束符
          nr_token++;                       // token个数
          break;                            // 退出switch
        }

        break; // 跳出 for 循环，回到 while 去处理下一个 token
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

/**
 * token调试输出
 */
static void debug_tokens() {
  printf("=== Token List ===\n");
  for (int i = 0; i < nr_token; i++) { // 匹配到的token长度
    printf("%2d: type=%d, str='%s'\n", i, tokens[i].type, tokens[i].str);
  }
}

/*
 * 判断表达式两端是否是成对括号
 */
static bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN) {
    return false;
  }

  int count = 0;
  for (int i = p + 1; i < q; i++) {
    if (tokens[i].type == TK_LPAREN)
      count++; // 记为内部括号
    else if (tokens[i].type == TK_RPAREN) {
      if (count == 0) // 内部括号完全闭合
        return false;
      count--; // 闭合内部括号
    }
  }

  return count == 0; // 说明完全匹配
}

/**
 * 找到最低优先级的二元运算符
 */
static int find_main_op(int p, int q) {
  int pos = -1;      // 最低优先级的二元运算符的位置
  int min_pri = 100; // 当前找到的最低优先级
  int paren = 0;     // 括号计数器，跳过括号内的运算符

  for (int i = p; i <= q; i++) { // 扫描token，找到最低优先运算符的分割点
    int type = tokens[i].type;
    /* 计算前面是否有括号，为优先运算符 */
    if (type == TK_LPAREN) {
      paren++;
      continue;
    }
    if (type == TK_RPAREN) {
      paren--;
      continue;
    }
    if (paren > 0)
      continue; // 内部括号闭合为止

    if (type == TK_SUB) { // 判断负号和减号，跳过负号，找到减号
      if (i == p || tokens[i - 1].type == TK_LPAREN ||
          tokens[i - 1].type == TK_ADD || tokens[i - 1].type == TK_SUB ||
          tokens[i - 1].type == TK_MUL || tokens[i - 1].type == TK_DIV) {
        continue; // '-' 的前面是'(','+','-','*','/'
      }
    }

    int pri;
    switch (type) {
    case TK_OR:
      pri = 0;
      break;
    case TK_AND:
      pri = 1;
      break;
    case TK_EQ:
    case TK_NEQ:
      pri = 2;
      break;
    case TK_ADD:
    case TK_SUB:
      pri = 3;
      break;
    case TK_MUL:
    case TK_DIV:
      pri = 4;
      break;
    default:
      continue;
    }

    if (pri <= min_pri) { // 当前扫描到的运算符优先级更低，需要更新pos
      min_pri = pri;      // 用来保证运算符的优先度最低，是否更新pos的关键
      pos = i;            // 递归计算表达式的时候的分界点
    }
  }

  return pos;
}

/*
 * 递归计算表达式eval
 * 判断边界
 * 单token
 * 去掉外层括号
 * 找最低优先级二元主运算符
 * 递归计算左右子表达式
 * 根据运算符来合并结果
 */
static word_t eval(int p, int q) {
  if (p > q) { // 子区间非法的错误处理，5 + （）
    panic("Bad expression: empty subexpression between %d and %d", p, q);
  }

  if (p == q) { // 单个 token

    if (tokens[p].type == TK_NUM) // 字符串转整数
      return strtoul(tokens[p].str, NULL, 10);

    if (tokens[p].type == TK_HEX) // 字符串转十六进制
      return strtoul(tokens[p].str, NULL, 16);

    if (tokens[p].type == TK_REG) { // 查看并返回寄存器的值
      char reg_name[32];
      strcpy(reg_name, tokens[p].str + 1); // 跳过第一个字符$，复制reg_name
      bool success = true;
      word_t val = isa_reg_str2val(reg_name, &success);  // 用reg_name计算对应值
      if (!success) {                                    // 如果查找失败
        printf("Invalid register name: %s\n", reg_name);
        assert(0);
      }
      return val; // 成功返回寄存器的值
    }
    panic("Unexpected single token at %d: type=%d, str='%s'", p, tokens[p].type,
          tokens[p].str);
  }

  if (check_parentheses(p, q)) { // 去掉外层括号，递归计算里面的子表达式
    return eval(p + 1, q - 1);
  }

  int op = find_main_op(p, q); // 找最低优先级的主运算符

  if (op == -1) { // 没有找到二元主运算符 -> 处理一元运算（负号或者解引用）
    if (tokens[p].type == TK_NEG) {
      return (word_t)(0 - eval(p + 1, q));
    }
    if (tokens[p].type == TK_DETEF) {

      word_t addr = (word_t)eval(p + 1, q); // 计算地址
      word_t val = vaddr_read(addr, 4);
      return val;
    }
    panic("No operator found but not a number or unary minus at %d..%d", p, q);
  }

  word_t val1 = eval(p, op - 1); // 计算 左子树 表达式
  word_t val2 = eval(op + 1, q); // 计算 右子树 表达式

  switch (tokens[op].type) { // 由op来合并计算结果
  case TK_OR:
    return val1 || val2;
  case TK_AND:
    return val1 && val2;
  case TK_EQ:
    return val1 == val2;
  case TK_NEQ:
    return val1 != val2;
  case TK_ADD:
    return (sword_t)val1 + (sword_t)val2;
  case TK_SUB:
    return (sword_t)val1 - (sword_t)val2;
  case TK_MUL:
    return (sword_t)val1 * (sword_t)val2;
  case TK_DIV: // 除法，检查除数是否为 0
    if ((sword_t)val2 == 0){
      printf("Division by zero (handled)\n");
      return 0; // -1
    }
    return (sword_t)val1 / (sword_t)val2;
  default: // 未知二元运算符，错误处理
    panic("Unknown operator at %d: type=%d, str='%s'", op, tokens[op].type,
          tokens[op].str);
  }
}
/*----------------------------------函数区-------------------------------*/

/*
 *顶层接口函数
 */
word_t expr(char *e, bool *success) {
  // printf("DEBUG: expr_debug is currently set to: %s\n",expr_debug ? "true" :
  // "false");
  if (!make_token(e)) { // 词法分析，返回false为失败，true为成功
    *success = false;
    return 0;
  }
  if (expr_debug) { // 调试输出
    debug_tokens();
  }

  /* 待办：插入代码以计算表达式的值。 */
  *success = true;                      // 计算前设为成功
  return eval(0, nr_token - 1); // 传入token区间
}
