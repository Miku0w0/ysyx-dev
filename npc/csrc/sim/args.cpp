#include "../npc.h"
#include "../trace/trace.h"
#include <getopt.h>

extern bool sdb_enabled;

void parse_args(int argc, char **argv, const char **img_file) {
  int opt;
  while ((opt = getopt(argc, argv, "imfthd")) != -1) {
    switch (opt) {
    case 'i':
      itrace_set_enable(true);
      break;
    case 'm':
      mtrace_set_enable(true);
      break;
    case 'f':
      ftrace_set_enable(true);
      break;
    case 't':
      itrace_set_enable(true);
      mtrace_set_enable(true);
      ftrace_set_enable(true);
      break;
    case 'd':
      sdb_enabled = true;
      break;
    case 'h':
      printf("Usage: npc [options] <image.bin>\n");
      printf("Options:\n");
      printf("  -i    Enable instruction trace\n");
      printf("  -m    Enable memory trace\n");
      printf("  -f    Enable function call trace\n");
      printf("  -t    Enable all traces\n");
      printf("  -h    Show this help\n");
      exit(0);
    default:
      break;
    }
  }
  // 获取镜像文件
  if (optind >= argc) {
    printf("[ERROR] No image file specified!\n");
    exit(1);
  }
  *img_file = argv[optind];
}