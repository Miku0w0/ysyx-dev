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
 * 该部分为头文件和宏定义的部分
 */
#include <regex.h>
#include "expr.h"

bool expr_debug = false; // 是否打印 token 调试信息

enum { // token的类型常量
  TK_NOTYPE = 256, TK_EQ,    // ==
  TK_NEQ,                    // !=
  TK_AND,                    // &&
  TK_OR,                     // ||
  TK_NUM,                    // 数字
  TK_ADD,                    // 加号
  TK_SUB,                    // 减号
  TK_MUL,                    // 乘号
  TK_DIV,                    // 除号
  TK_LPAREN,                 // 左括号
  TK_RPAREN,                 // 右括号
  TK_NEG = 270,              // 负号
  TK_HEX,                    // 十六进制
  TK_REG                     // 寄存器 
    /* TODO: Add more token types */
    /* 待办：添加更多标记类型 */
};

static struct rule { // 正则表达式与token类型的映射表
  const char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},               // spaces
  {"\\+", TK_ADD},                 // plus
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
   * 待办：添加更多规则
   * Pay attention to the precedence level of different rules.
   * 注意不同规则的优先级顺序
   */
};

#define NR_REGEX ARRLEN(rules) //计算rules的数组大小并赋值给NR_REGEX
                                  // POSIX C 标准库，为了初始化做准备
static regex_t re[NR_REGEX] = {}; // regex.h的结构体类型，存rule的编译结果

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 * 规则会被多次使用
 * 因此，我们在使用前仅编译一次
 */
void init_regex() { //初始化正则表达式引擎，处理rules[]中的正则表达式
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) { //POSIX C 的正则表达式编译函数
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
/*
 * Token的数据结构
 */
typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {}; // 匹配片段数组
static int nr_token __attribute__((used))  = 0;        // token有多少片段
/* __attribute__((used))让编译器不要把这两个变量优化掉*/

/*
 * 词法分析函数
 */
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch; // POSIX 正则库的一个存储一次正则匹配的结构体，成员为rm_so和rm_eo，start/end offset，匹配开始与结束的位置
  nr_token = 0;      // token的片段长度
  /* 对于下面用到的regexec函数，
   * re[i]：第 i 条规则的正则表达式
   * e + position：从当前位置开始匹配字符串
   * 1：只匹配 1 组（整个 token）
   * &pmatch：把匹配的起止位置写入 pmatch，即长度
   * 0：flag默认参数
   */
  while (e[position] != '\0') { // 开始扫描字符串，还没扫描到结尾的'\0'
    /* Try all rules one by one. */
    /* 逐一尝试所有规则。 */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) { // 匹配到一个规则，进入switch
        char *substr_start = e + position; // 指向匹配的字符串的开头
        int substr_len = pmatch.rm_eo;     // 当前token的长度
        if (expr_debug){                   // 调试信息
          Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",i, rules[i].regex, position, substr_len, substr_len, substr_start);
        }

        position += substr_len; // 记录当前扫描到的长度，更新扫描位置，准备扫描下一个token

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         * 待办：现已通过规则 rules [i] 识别出一个新标记（token）
         * 补充代码将该标记记录到 tokens 数组中。
         * 对于特定类型的标记，需执行相应的额外操作。
         */

        switch (rules[i].token_type) { // token.type
          /* default: TODO(); */
          /* 默认情况：预留待实现（TODO）；*/
          case TK_NOTYPE: // 忽略空格
            break;        //退出switch

          case TK_SUB: // 判断是负号还是减号
            printf("Processing '-', TK_NEG=%d, TK_SUB=%d\n", TK_NEG, TK_SUB); // 添加调试
            if (nr_token == 0 ||
                (tokens[nr_token - 1].type != TK_NUM &&
                tokens[nr_token - 1].type != TK_RPAREN &&
                tokens[nr_token - 1].type != TK_HEX)) { //前面是number，')',HEX，即为二元减号，这里是不等于
              tokens[nr_token].type = TK_NEG;           // 一元负号
              printf("Set to TK_NEG: %d\n", TK_NEG);    // 添加调试
            } else {
              tokens[nr_token].type = TK_SUB;           // 二元减号
              printf("Set to TK_SUB: %d\n", TK_SUB);    // 添加调试
            }
              strncpy(tokens[nr_token].str, substr_start, 1); // 拷贝token.str，strncpy(dest, src, len)，减号处理就1个
              tokens[nr_token].str[1] = '\0';
              nr_token++; // token个数
              break;      // 退出switch

          default:
            tokens[nr_token].type = rules[i].token_type;
            int len = substr_len;
            if (len >= sizeof(tokens[nr_token].str)){ // 确保不会越界
              len = sizeof(tokens[nr_token].str) - 1; // 留下一个位置补上结束符'\0'
            }
            strncpy(tokens[nr_token].str, substr_start, len); // 拷贝token.str，strncpy(dest, src, len)
            tokens[nr_token].str[len] = '\0'; // 补个结束符
            nr_token++; // token个数
            break;      // 退出switch
        }

        break; // 跳出for，回到while处理下一个token
      }
    }
    // 所有规则都无法匹配
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

/*
 * Token调试输出
 */
static void debug_tokens() {
  printf("=== Token List ===\n");
  for (int i = 0; i < nr_token; i++) { // 匹配的token长度
    printf("%2d: type=%d, str='%s'\n", i, tokens[i].type, tokens[i].str); // 这正是make_token输出的结果
  }
}

/*
 * 判断表达式两端是否是成对括号
 */
static bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN){
    return false;
  }

  int count = 0;
  for (int i = p + 1; i < q; i++) {
    if (tokens[i].type == TK_LPAREN) count++; // 若匹配到(，记为内部括号
    else if (tokens[i].type == TK_RPAREN) {
      if (count == 0) return false; // 说明匹配到的)是外层括号之外的闭合，内层括号已经全部匹配好并且闭合了，故false
      count--;                      // 若匹配到)，则闭合内部括号
    }
  }

  return count == 0; // 最后 count 只要是 0 就说明内部括号完全匹配，这是一个判断式
}

/*
 * 找最低优先级的二元运算符
 */
static int find_main_op(int p, int q) {
    int pos = -1;      // 最低优先级的二元运算符的位置，-1默认没找到
    int min_pri = 100; // 当前找到的最低优先级
    int paren = 0;     // 括号计数器，跳过括号内的运算符

    for (int i = p; i <= q; i++) { // 扫描token
        int type = tokens[i].type;

        if (type == TK_LPAREN) { paren++; continue; }
        if (type == TK_RPAREN) { paren--; continue; }
        if (paren > 0) continue; //说明在括号里面，要跳出去，直到paren的值为0

        if (type == TK_SUB) { // 判断一元负号
            if (i == p || 
                tokens[i-1].type == TK_LPAREN ||
                tokens[i-1].type == TK_ADD ||
                tokens[i-1].type == TK_SUB || 
                tokens[i-1].type == TK_MUL ||
                tokens[i-1].type == TK_DIV) { // '-'的前面是'(','+','-','*','/'
                continue; // 跳过
            }
        }

        int pri;
        switch (type) {
            case TK_OR:  pri = 0; break; // ||
            case TK_AND: pri = 1; break; // &&
            case TK_EQ:                  // ==
            case TK_NEQ: pri = 2; break; // !=
            case TK_ADD:                 // +
            case TK_SUB: pri = 3; break; // -
            case TK_MUL:                 // *
            case TK_DIV: pri = 4; break; // /
            default: continue;
        }

        if (pri <= min_pri) { // 当前扫描到的运算符优先级更低，需要更新pos
            min_pri = pri; // 用来保证运算符的优先度最低，是否更新pos的关键
            pos = i;  //递归计算表达式的时候的分界点
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
    if (p > q) { // 子区间非法的错误处理
        panic("Bad expression: empty subexpression between %d and %d", p, q);
    }

    if (p == q) { // 单个 token
        if (tokens[p].type == TK_NUM) return atoi(tokens[p].str);              // 数字转成整数
        if (tokens[p].type == TK_HEX) return strtoul(tokens[p].str, NULL, 16); // 十六进制转成整数
        if (tokens[p].type == TK_REG) {                                        // 查看并返回寄存器的值
            char reg_name[32];
            strcpy(reg_name, tokens[p].str + 1); // 跳过第一个字符$，复制了reg的name
            bool success = true;
            word_t val = isa_reg_str2val(reg_name, &success); // 接口函数用reg_name查询对应的值
            if (!success) { // 是否查找成功，寄存器名字存在
                printf("Invalid register name: %s\n", reg_name);
                assert(0);
            }
            return val; // 返回寄存器的值
        }
        panic("Unexpected single token at %d: type=%d, str='%s'", p, tokens[p].type, tokens[p].str); // 非上述类型就报错
    }

    if (check_parentheses(p, q)) { // 括号包围整个表达式
        return eval(p + 1, q - 1); // 去掉外层括号，递归计算里面的子表达式
    }

    int op = find_main_op(p, q); // 找最低优先级的主运算符，用于将表达式拆分为左右子表达式进行递归计算

    /* 若未找到主运算符，
     * 只有括号和负号的可能（参考上面函数），
     * 而上面调用了check_parentheses保证除去了括号，
     * 故此处if只有负号或空的可能
     */
    if (op == -1) { // 处理一元负号
          if (tokens[p].type == TK_NEG) { 
            return -eval(p + 1, q);
        }
        panic("No operator found but not a number or unary minus at %d..%d", p, q); // 可能负号也没有
    }

    word_t val1 = eval(p, op - 1); // 计算左子树表达式
    word_t val2 = eval(op + 1, q); // 计算右子树表达式

    switch (tokens[op].type) { // 根据op来合并计算结果
        case TK_OR:  return val1 || val2;
        case TK_AND: return val1 && val2;
        case TK_EQ:  return val1 == val2;
        case TK_NEQ: return val1 != val2;
        case TK_ADD: return val1 + val2;
        case TK_SUB: return val1 - val2;
        case TK_MUL: return val1 * val2;
        case TK_DIV: // 除法操作，检查除数是否为 0
            if (val2 == 0) panic("Division by zero");
            return val1 / val2;
        default:     // 错误处理
            panic("Unknown operator at %d: type=%d, str='%s'", op, tokens[op].type, tokens[op].str);
    }
}
/*----------------------------------函数区-------------------------------*/

/*
 *顶层接口函数
 */
word_t expr(char *e, bool *success) {
  if (!make_token(e)) { // 词法分析，返回0为失败
    *success = false;
    return 0;
  }
  if(expr_debug){ // 调试输出
     debug_tokens(); 
  }

  /* TODO: Insert codes to evaluate the expression. */
  /* 待办：插入代码以计算表达式的值。 */
  *success = true; // 计算前设为成功
  return eval(0, nr_token - 1); // 传入token区间

}
