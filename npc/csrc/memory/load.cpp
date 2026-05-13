#include "../npc.h"

extern uint8_t *guest_to_host(uint32_t paddr);

void load_img(char *img_file) {
  if (img_file == NULL)
    return;
  FILE *fp = fopen(img_file, "rb");
  if (fp == NULL) {
    printf("Can't open %s\n", img_file);
    return;
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  printf("[NPC] Image file size: %ld bytes\n", size);
  if (size <= 0) {
    printf("[NPC] Error: Image size is 0!\n");
    exit(1);
  }

  uint8_t *host_addr = guest_to_host(MEM_BASE);
  printf("[NPC] Destination host address: %p\n", host_addr);

  size_t ret = fread(host_addr, size, 1, fp);
  if (ret != 1) {
    printf("[NPC] Error: fread failed!\n");
  }

  fclose(fp);
  printf("[NPC] Load success!\n");
}