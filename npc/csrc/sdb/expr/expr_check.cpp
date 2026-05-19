#include "expr.h"

bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN) {
    return false;
  }

  int count = 0;
  for (int i = p + 1; i < q; i++) {
    if (tokens[i].type == TK_LPAREN) {
      count++;
    } else if (tokens[i].type == TK_RPAREN) {
      if (count == 0)
        return false;
      count--;
    }
  }
  return count == 0;
}