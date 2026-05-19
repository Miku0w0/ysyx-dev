#include "expr.h"

extern word_t eval(int p, int q);

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  if (expr_debug) {
    debug_tokens();
  }
  *success = true;
  return eval(0, nr_token - 1);
}