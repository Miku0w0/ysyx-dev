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

#include <common.h>


void init_monitor(int, char *[]);//初始化monitor
void am_init_monitor();
void engine_start();//开始执行
int is_exit_status_bad();
int expr(char *e, bool *success);


int main(int argc, char *argv[]) {
  /* Initialize the monitor. 初始化控制台*/
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

// ------------------------ PA1: expr 测试部分 ------------------------
#ifdef CONFIG_PA1
  {
    extern bool expr_debug; // 引用来自 expr.c 的全局变量
    expr_debug = false;     

    FILE *fp = fopen("tools/gen-expr/input", "r");
    Assert(fp, "Cannot open input file!");

    char buf[65536];
    int expected = 0;
    char expr_str[65536];

    int line = 0;
    bool success = true;

    while (fgets(buf, sizeof(buf), fp) != NULL) {
      // 格式：value expression
      //      12345 1+2+3
      sscanf(buf, "%d %s", &expected, expr_str);

      int result = expr(expr_str, &success);

      if (!success || result != expected) {
        printf("Test failed at line %d\n", line + 1);
        printf("Expr: %s\n", expr_str);
        printf("Expected: %d, Got: %d\n", expected, result);
        assert(0);
      }else{ // 测试成功也打印一下
        printf("\x1b[32m[PASS] Line %d: %s = %d (Expected: %d)\x1b[0m\n",
               line + 1, expr_str, result, expected);
      }
      line++;
    }

    printf("\x1b[32mAll %d tests passed!\x1b[0m\n", line);
    fclose(fp);

    return 0;   // 非常重要：测试通过后直接退出！
  }
#endif
// -------------------------------------------------------------------

  /* Start engine. 执行阶段入口*/
  engine_start();

  return is_exit_status_bad();
}
