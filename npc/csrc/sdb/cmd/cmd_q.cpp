#include "../sdb.h"
#include <cstdlib>

int cmd_q(char *args) {
  printf("Exiting NPC...\n");
  exit(0);
  return -1;
}