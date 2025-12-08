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

#include <isa.h>
#include <memory/paddr.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm();

static void welcome() { // 打印指令跟踪状态
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN),
                          ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE,
        Log("If trace is enabled, a log file will be generated "
            "to record the trace. This may lead to a large log file. "
            "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  // 宏转为字符串，黄字红底
  printf("Welcome to %s-NEMU!\n",
         ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  // Log("Exercise: Please remove me in the source code and compile NEMU
  // again."); assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) { // 没有用命令行参数指定镜像文件
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size 默认返回一个尺寸，表示加载成功
  }

  FILE *fp = fopen(img_file, "rb"); // 只读，二进制模式打开
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END); // 文件读写指针定位到文件末尾，返回指针，得到大小
  long size = ftell(fp);  // 返回当前指针位置，得到文件总字节大小

  Log("The image is %s, size = %ld", img_file, size); // 打印日志，文件及其大小

  fseek(fp, 0, SEEK_SET); // 文件读写指针定位到文件开头，进行覆盖
  // 加载并覆盖，目标地址，数据像大小，数据项个数，该数据的文件指针
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1); // 检查fread是否成功读取文件

  fclose(fp);  // 关闭句柄，释放资源
  return size; // 返回加载的镜像文件的总大小，传递给接下来的 init_difftest
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
      {"batch", no_argument, NULL, 'b'},
      {"log", required_argument, NULL, 'l'},
      {"diff", required_argument, NULL, 'd'},
      {"port", required_argument, NULL, 'p'},
      {"help", no_argument, NULL, 'h'},
      {0, 0, NULL, 0},
  };
  int o; // 解析函数：-(加载IMAGE) b h l: d: p: 有冒号后面有参数，处理完返回-1
  while ((o = getopt_long(argc, argv, "-bhl:d:p:", table, NULL)) != -1) {
    switch (o) {
    case 'b':
      sdb_set_batch_mode(); // 批处理自动运行客户程序
      break;
    case 'p':
      sscanf(optarg, "%d", &difftest_port); // 差分测试TCP端口号
      break;
    case 'l':
      log_file = optarg; // 日志文件路径，到init_log
      break;
    case 'd':
      diff_so_file = optarg; // 参考模拟器共享库路径，到init_difftest
      break;
    case 1:
      img_file = optarg; // 非参数选项-，指定客户程序路径，立即返回，到log_img
      return 0;
    default: // h,打印程序使用说明
      printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
      printf("\t-b,--batch              run with batch mode\n");
      printf("\t-l,--log=FILE           output log to FILE\n");
      printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
      printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
      printf("\n");
      exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv); // 解析命令行参数

  /* Set random seed. */
  init_rand(); // 设置随机数种子

  /* Open the log file. */
  init_log(log_file); // 初始化打开日志文件

  /* Initialize memory. */
  init_mem(); // 初始化模拟物理内存

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device()); // 初始化外部设备

  /* Perform ISA dependent initialization. */
  init_isa(); // 指令集架构依赖的初始化

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img(); // 加载镜像客户程序到模拟内存，覆盖内置指令

  /* Initialize differential testing. */
  /* 动态链接库 客户程序大小 通信端口 */
  init_difftest(diff_so_file, img_size, difftest_port); // 初始化差分测试

  /* Initialize the simple debugger. */
  init_sdb(); // 初始化简易调试器

  IFDEF(CONFIG_ITRACE, init_disasm()); // 初始化反汇编模块

  /* Display welcome message. */
  welcome(); // 打印欢迎信息
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
