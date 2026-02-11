#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static int itoa(int n, char *s) {
  char buf[32];
  int i = 0, len = 0;
  unsigned int num;
  if (n < 0) {
    *s ++ = '-';
    len ++;
    num = (unsigned int)(-n);
  } else {
    num = (unsigned int)n;
  }

  do {
    buf[i ++] = (num % 10) + '0';// 转变字符
    num /= 10; // 逐位
  } while (num);
  len += i;
  while (i > 0) {
    *s ++ = buf[-- i]; // 倒过来载入字符串
  }
  return len;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *p = out;
  size_t i = 0; // 记录当前已经写入的字符数

  while (*fmt && (i < n - 1)) { // 留一个位置给 '\0'
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
      case 's': {
        char *s = va_arg(ap, char *);
        if (s == NULL)
          s = "(null)";
        while (*s && (i < n - 1)) {
          *p++ = *s++;
          i++;
        }
        break;
      }
      case 'd': {
        char buf[32];
        int num = va_arg(ap, int);
        // 借用你已经写好的 itoa，注意它返回的是数字长度
        int num_len = itoa(num, buf);
        for (int j = 0; j < num_len && (i < n - 1); j++) {
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
    *p = '\0'; // 强制封口
  }
  return i; // 返回写入的有效字符长度
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return len;
}

int printf(const char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  int len = vsprintf(buf, fmt, ap);
  va_end(ap);
  for (int i = 0; i < len; i++) {
    putch(buf[i]);
  }
  return len;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
}

#endif
