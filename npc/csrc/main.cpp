#include "npc.h"
#include "sdb/sdb.h"

int main(int argc, char **argv) {
  const char *img_file = NULL;
  parse_args(argc, argv, &img_file);
  npc_init(argc, argv);
  load_img((char *)img_file);
  reset(10);

  if (sdb_enabled) sdb_mainloop();
  else {
    while (!Verilated::gotFinish()) single_cycle();
  }
  
  npc_exit();
  return 0;
}