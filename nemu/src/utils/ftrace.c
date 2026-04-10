#include <common.h>
#include <elf.h>
#include <stdlib.h> 
#include <string.h> 

typedef struct {
    char name[64];
    paddr_t addr;
    size_t size;
} Symbol;

static Symbol *symtab_entries = NULL;
static int nr_symtab = 0;
static int depth = 0;

const char *find_symbol(paddr_t addr) {
  for (int i = 0; i < nr_symtab; i++) {
    // 如果有 size，判断是否在区间内
    if (symtab_entries[i].size > 0) {
      if (addr >= symtab_entries[i].addr && addr < symtab_entries[i].addr + symtab_entries[i].size) {
        return symtab_entries[i].name;
      }
    } 
    // 如果没有 size，或者刚好命中首地址，如call 目标
    else if (addr == symtab_entries[i].addr) {
      return symtab_entries[i].name;
    }
  }
  return NULL;
}

void ftrace_print(vaddr_t pc, vaddr_t target, bool is_call) {
  if (is_call) { // call调用
    const char *name = find_symbol(target); // 查找 目标函数名
    printf("0x%08x: %*scall [%s @ 0x%08x]\n", pc, depth * 2, "", name ? name : "???", target); // 共5个占位 23为动态缩进占位，*为dep*2 s为空
    depth++;
  } else { // ret返回 则dep-- dep用两倍空格来表现
    depth--;
    if (depth < 0) depth = 0;
    const char *name = find_symbol(pc); // 查找 当前函数名
    printf("0x%08x: %*sret  [%s]\n", pc, depth * 2, "", name ? name : "???");
  }
}

void init_ftrace(const char *elf_file) {
    if(elf_file == NULL) return;
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp,"Can not open '%s'",elf_file);

    int ret;
    // 读取 ELF Header 封面
    Elf32_Ehdr ehdr;
    ret = fread(&ehdr, sizeof(Elf32_Ehdr), 1, fp); // 读取元素个数
    Assert(ret == 1, "Failed to read ELF header");
    Assert(*(uint32_t *)ehdr.e_ident == 0x464c457f, "Not a valid ELF file");

    // 读取Section Headers 目录
    Elf32_Shdr shdr[ehdr.e_shnum];
    fseek(fp, ehdr.e_shoff, SEEK_SET); // 定位
    ret = fread(shdr, sizeof(Elf32_Shdr), ehdr.e_shnum, fp); // 读取个数
    Assert(ret == ehdr.e_shnum, "Failed to read section headers");

    // 寻找符号表 (.symtab) 和 字符串表 (.strtab)
    Elf32_Shdr *symtab_sh = NULL;
    Elf32_Shdr *strtab_sh = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++) {
      if (shdr[i].sh_type == SHT_SYMTAB)
        symtab_sh = &shdr[i];
      if (shdr[i].sh_type == SHT_STRTAB && i != ehdr.e_shstrndx) // 排除掉存章节名字的
        strtab_sh = &shdr[i];
    }

    // 读取符号表项
    int sym_count = symtab_sh->sh_size / sizeof(Elf32_Sym); // 符号的个数
    Elf32_Sym syms[sym_count];
    fseek(fp, symtab_sh -> sh_offset, SEEK_SET);
    ret = fread(syms, sizeof(Elf32_Sym), sym_count, fp);
    Assert(ret == sym_count, "Failed to read .symtab headers");

    // 读取字符串表内容
    char strtab[strtab_sh->sh_size];
    fseek(fp, strtab_sh->sh_offset, SEEK_SET);
    ret = fread(strtab, strtab_sh->sh_size, 1, fp);
    Assert(ret == 1, "Failed to read .strtab headers");

    // 筛选出 FUNC 类型的符号并存入
    symtab_entries = malloc(sizeof(Symbol) * sym_count); // 开辟空间存入func符号
    for (int i = 0; i < sym_count; i++) {
      if (ELF32_ST_TYPE(syms[i].st_info) == STT_FUNC) {
        strncpy(symtab_entries[nr_symtab].name, &strtab[syms[i].st_name], 63); // 拼接目的地 函数页码对应的起始地址
        symtab_entries[nr_symtab].addr = syms[i].st_value;
        symtab_entries[nr_symtab].size = syms[i].st_size;
        nr_symtab ++;
      }
    }
    fclose(fp);
    Log("Ftrace: Loaded %d symbols from %s", nr_symtab, elf_file);
}
