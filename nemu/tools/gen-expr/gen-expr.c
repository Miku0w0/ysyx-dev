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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// this should be enough
static char buf[65536];    // 存储生成的算术表达式
static size_t buf_pos = 0; // 缓冲区位置指针
// a little larger than `buf`
static char
    code_buf[65536 + 128]; // 存储最终的c代码模板，包含生成表达式的格式控制模板
static char *code_format = // 格式化c代码模板
    "#include <stdio.h>\n"
    "int main() { "
    "  unsigned result = %s; "
    "  printf(\"%%u\", result); "
    "  return 0; "
    "}";

static void gen_rand_expr(int depth);
static void gen_binary_expr(int depth);

// 生成字符
static inline void gen(char c) {
  if (buf_pos < sizeof(buf) - 1) { // 防止缓冲区溢出
    buf[buf_pos++] = c;            // 将字符c添加到buf缓冲区中，以构建表达式
  }
}

// 生成数字
static inline void gen_num() {
  if (buf_pos >= sizeof(buf) - 4) // 检查是否溢出
    return;
  // 生成1-99的数字，避免0和过大的数字
  // int num = rand() % 99 + 1; // 1-99
  int num = rand() % 99 + 1;
  if (num < 10)              // 小于10直接生成一个字符，加入缓冲区
    buf[buf_pos++] = '0' + num;
  else { // 大于10则分开处理十位和个位，分别加入缓冲区
    int tens = num / 10;
    int ones = num % 10;
    buf[buf_pos++] = '0' + tens;
    buf[buf_pos++] = '0' + ones;
  }
}

// 生成随机运算符
static inline void gen_rand_op() {
  if (buf_pos >= sizeof(buf) - 1) // 检查是否溢出
    return;
  // 生成运算符
  static const char ops[] = "+*/-";
  char op = ops[rand() % 4]; // 随机选择运算符
  buf[buf_pos++] = op;       // 写入缓冲区
}

// 生成二元表达式
static void gen_binary_expr(int depth) {
  // 生成左操作数
  if (rand() % 2 == 0) {
    gen('(');
    gen_rand_expr(depth + 1);
    gen(')');
  } else {
    gen_rand_expr(depth + 1);
  }
  gen_rand_op(); // 生成运算符
  // 生成右操作数
  if (rand() % 2 == 0) {
    gen('(');
    gen_rand_expr(depth + 1);
    gen(')');
  } else {
    gen_rand_expr(depth + 1);
  }
}

// 递归生成随机表达式
static void gen_rand_expr(int depth) {
  if (buf_pos >= sizeof(buf) - 10 || depth > 4) { // 缓冲区防溢出 和 递归深度
    gen_num(); // 如果缓冲区空间不足，生成数字并返回
    return;
  }
  int choice = rand() % 10;
  switch (choice) {
  case 0: // 10% 的概率生成数字
    gen_num();
    break;
  case 1: // 10% 的概率生成括号
    gen('(');
    gen_rand_expr(depth + 1);
    gen(')');
    break;
  default: // 80% 的概率生成二元表达式
    gen_binary_expr(depth + 1);
    break;
  }
}
/*************************************主函数**************************************/
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 100;

  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }

  int success_count = 0;

  while (success_count < loop) { // 检查表达式 并 处理生成的表达式
    buf_pos = 0; // 重置缓冲区
    gen_binary_expr(0);

    // 确保以空字符结尾
    if (buf_pos < sizeof(buf)) {
      buf[buf_pos] = '\0';
    } else {
      buf[sizeof(buf) - 1] = '\0';
    }
    /* 检查表达式是否合法 */
    int valid = 1;       // 假设表达式有效
    int paren_count = 0; // 括号匹配数
    // 括号是否匹配
    for (size_t j = 0; j < strlen(buf); j++) {
      if (buf[j] == '(')
        paren_count++;
      else if (buf[j] == ')')
        paren_count--;
      // 检查是否有连续的两个运算符
      if (j > 0 && strchr("+-*/", buf[j - 1]) && strchr("+-*/", buf[j])) {
        valid = 0;
        break;
      }
      // 检查是否有运算符在开头或结尾
      if ((j == 0 && strchr("*/", buf[j])) ||
          (j == strlen(buf) - 1 && strchr("+-*/", buf[j]))) {
        valid = 0;
        break;
      }
    }
    // 无效表达式 括号数量不匹配 跳过
    if (!valid || paren_count != 0) {
      continue;
    }
    /* 编译并运行（上述检查完以后）*/
    // 代码封装
    sprintf(code_buf, code_format, buf);
    FILE *fp = fopen("/tmp/.code.c", "w");
    if (fp == NULL) // 未能成功打开文件，跳过
      continue;
    // 写入临时文件 /tmp/.code.c
    fputs(code_buf, fp);
    fclose(fp);
    // system编译c代码
    // int ret = system("gcc /tmp/.code.c -Wall -Werror -o /tmp/.expr 2>/dev/null");
    int ret = system("gcc /tmp/.code.c -o /tmp/.expr 2>/dev/null");
    if (ret != 0) { // 编译失败，跳过
      continue;
    }

    /* 运行编译好的表达式程序 */
    fp = popen("/tmp/.expr 2>/dev/null ", "r"); // 读取程序标准输出
    if (fp == NULL)                 // 程序无法运行，跳过
      continue;
    // 尝试读取子程序打印的结果
    unsigned int result_val;
    int ret_scan = fscanf(fp, "%u", &result_val);

    // 等待子程序终止并且返回退出状态信息
    pclose(fp);

    // 成功读取到子程序打印的结果
    if (ret_scan == 1) {
      printf("%u %s\n", result_val, buf);
      success_count++;
    } else { // 编译失败、其他运行时错误，跳过
      continue;
    }

    // 每成功生成10个表达式，输出进度
    if (success_count > 0 && success_count % 10 == 0) {
      fprintf(stderr, "Generated %d valid expressions\n", success_count);
    }
  }

  // 清理临时生成的文件
  system("rm -f /tmp/.code.c /tmp/.expr 2>/dev/null");
  fprintf(stderr, "Total: %d valid expressions generated\n", success_count);
  return 0;
}