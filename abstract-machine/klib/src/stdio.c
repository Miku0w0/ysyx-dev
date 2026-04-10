#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static int itoa(int n, char *s) { // 数字转字符串
  char buf[32];
  int i = 0, len = 0;
  unsigned int num;

  if (n < 0) { // 处理负数
    *s ++ = '-';
    len ++;
    num = (unsigned int)(-n);
  } else {
    num = (unsigned int)n;
  }

  do { // 转变字符，顺序是反的
    buf[i ++] = (num % 10) + '0';
    num /= 10;
  } while (num);
  len += i;
  while (i > 0) {
    *s ++ = buf[-- i]; // 逆序载入字符串
  }
  return len;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) { // ap参数指针
  char *p = out; // 输出的字符
  size_t i = 0; // 记录写入的字符数

  while (*fmt && (i < n - 1)) { // 读取*fmt的字符 留一个位置给 '\0'
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
      case 's': {
        char *s = va_arg(ap, char *); // 抓取char *
        if (s == NULL)
          s = "(null)";
        while (*s && (i < n - 1)) { // 字符不为空
          *p++ = *s++;
          i++;
        }
        break;
      }
      case 'd': {
        char buf[32];
        int num = va_arg(ap, int); // 抓取int
        int num_len = itoa(num, buf); // 数字转字符
        for (int j = 0; j < num_len && (i < n - 1); j++) { // 留位置给\0
          *p++ = buf[j];
          i++;
        }
        break;
      }
      case '%': {
        *p++ = '%';
        i++;
        break;
      }
      }
    } else {
      *p++ = *fmt;
      i++;
    }
    fmt++;
  }

  if (n > 0) {
    *p = '\0'; // 读取完*fmt之后强制封口
  }
  return i; // 返回写入的有效字符长度
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap); // 无符号整数作为范围最大值，2*32-1
}

int snprintf(char *out, size_t n, const char *fmt, ...) { // 和sprintf相比只是多了一个n
  va_list ap;
  va_start(ap, fmt); // …丢进ap
  int len = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return len;
}

int printf(const char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt); // …丢进ap
  int len = vsprintf(buf, fmt, ap);
  va_end(ap);
  for (int i = 0; i < len; i++) {
    putch(buf[i]); // 调用硬件接口 打印到终端 串口寄存器
  }
  return len;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt); // …丢进ap
  int len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
}

#endif
