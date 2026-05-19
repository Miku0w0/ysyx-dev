#include "../sdb.h"
#include "../wp/wp.h"

int cmd_d(char *args) {
  if (args == NULL) {
    printf("Usage: d N\n");
    return 0;
  }

  int no = atoi(args);
  WP *wp = get_wp(no);
  if (wp == NULL) {
    printf("No watchpoint with number %d\n", no);
    return 0;
  }

  free_wp(wp);
  printf("Deleted watchpoint %d\n", no);
  return 0;
}