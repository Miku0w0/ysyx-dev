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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void gen_rand_expr() {
  int len = 0;
  int depth = 0; // 括号深度

  buf[0] = '\0';

  int expr_len = 1 + rand() % 5; // 每个表达式随机生成1~5个操作数

  for (int i = 0; i < expr_len; i++) {
    // 随机决定是否加左括号
    if (rand() % 3 == 0) {
      buf[len++] = '(';
      depth++;
    }

    int num;

    // 如果上一个运算符是除号，就保证右操作数不为0
    if (i > 0 && buf[len-1] == '/') {
        num = 1 + rand() % 9;  // 1~9
    } else {
        num = rand() % 10;     // 0~9
    }

    len += sprintf(buf + len, "%d", num);

    // 随机决定是否加右括号
    while (depth > 0 && rand() % 3 == 0) {
      buf[len++] = ')';
      depth--;
    }

    // 除最后一个数字外，随机生成运算符
    if (i != expr_len - 1) {
      char ops[] = "+-*/";
      buf[len++] = ops[rand() % 4];
    }
  }

  // 补上剩余未闭合的括号
  while (depth-- > 0) {
    buf[len++] = ')';
  }

  buf[len] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
