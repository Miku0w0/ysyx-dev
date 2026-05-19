#include "../../npc.h"
#include "../sdb.h"

int cmd_info(char *args) {
  if (args == NULL) {
    printf("Usage: info r  or  info w\n");
    return 0;
  }

  if (strcmp(args, "r") == 0) {
    printf("\n");
    for (int i = 0; i < 32; i++) {
      printf("x%d: 0x%08x  ", i, get_reg_value(i));
      if ((i + 1) % 4 == 0)
        printf("\n");
    }
    printf("\n");
  } else if (strcmp(args, "w") == 0) {
    info_wp();
  } else {
    printf("Unknown info subcommand: %s\n", args);
  }
  return 0;
}