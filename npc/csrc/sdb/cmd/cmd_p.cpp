#include "../expr/expr.h"
#include "../sdb.h"

int cmd_p(char *args) {
  if (args == NULL) {
    printf("Usage: p EXPR\n");
    return 0;
  }

  bool success = true;
  word_t result = expr(args, &success);

  if (success) {
    printf("%s = %d (0x%08x)\n", args, result, result);
  } else {
    printf("Invalid expression: %s\n", args);
  }
  return 0;
}