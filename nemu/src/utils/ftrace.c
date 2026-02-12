/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

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
    // 1. 如果有 size，判断是否在区间内
    if (symtab_entries[i].size > 0) {
      if (addr >= symtab_entries[i].addr && addr < symtab_entries[i].addr + symtab_entries[i].size) {
        return symtab_entries[i].name;
      }
    } 
    // 2. 如果没有 size，或者刚好命中首地址（call 目标）
    else if (addr == symtab_entries[i].addr) {
      return symtab_entries[i].name;
    }
  }
  return NULL;
}

void ftrace_print(vaddr_t pc, vaddr_t target, bool is_call) {
  if (is_call) {
    const char *name = find_symbol(target); // 查找目标函数名
    printf("0x%08x: %*scall [%s @ 0x%08x]\n", pc, depth * 2, "", name ? name : "???", target);
    depth++;
  } else {
    depth--;
    if (depth < 0) depth = 0;
    const char *name = find_symbol(pc); // 查找当前函数名
    printf("0x%08x: %*sret  [%s]\n", pc, depth * 2, "", name ? name : "???");
  }
}

void init_ftrace(const char *elf_file) {
    if(elf_file == NULL) return;
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp,"Can not open '%s'",elf_file);

    int ret;
    // 读取 ELF Header
    Elf32_Ehdr ehdr;
    ret = fread(&ehdr, sizeof(Elf32_Ehdr), 1, fp);
    Assert(ret == 1, "Failed to read ELF header");
    Assert(*(uint32_t *)ehdr.e_ident == 0x464c457f, "Not a valid ELF file");

    // 读取Section Headers
    Elf32_Shdr shdr[ehdr.e_shnum];
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    ret = fread(shdr, sizeof(Elf32_Shdr), ehdr.e_shnum, fp);
    Assert(ret == ehdr.e_shnum, "Failed to read section headers");

    // 寻找符号表 (.symtab) 和 字符串表 (.strtab)
    Elf32_Shdr *symtab_sh = NULL;
    Elf32_Shdr *strtab_sh = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++) {
      if (shdr[i].sh_type == SHT_SYMTAB)
        symtab_sh = &shdr[i];
      if (shdr[i].sh_type == SHT_STRTAB && i != ehdr.e_shstrndx)
        strtab_sh = &shdr[i];
    }

    // 读取符号表项
    int sym_count = symtab_sh->sh_size / sizeof(Elf32_Sym);
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
    symtab_entries = malloc(sizeof(Symbol) * sym_count); 
    for (int i = 0; i < sym_count; i++) {
      if (ELF32_ST_TYPE(syms[i].st_info) == STT_FUNC) {
        strncpy(symtab_entries[nr_symtab].name, &strtab[syms[i].st_name], 63);
        symtab_entries[nr_symtab].addr = syms[i].st_value;
        symtab_entries[nr_symtab].size = syms[i].st_size;
        nr_symtab ++;
      }
    }
    fclose(fp);
    Log("Ftrace: Loaded %d symbols from %s", nr_symtab, elf_file);
}
