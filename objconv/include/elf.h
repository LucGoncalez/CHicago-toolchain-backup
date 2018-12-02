// File author is Ítalo Lima Marconato Matias
//
// Created on November 13 of 2018, at 15:34 BRT
// Last edited on November 15 of 2018, at 13:58 BRT

#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

#define ELF_IDENT_MAGIC_0 0
#define ELF_IDENT_MAGIC_1 1
#define ELF_IDENT_MAGIC_2 2
#define ELF_IDENT_MAGIC_3 3
#define ELF_IDENT_CLASS 4
#define ELF_IDENT_DATA 5
#define ELF_IDENT_VERSION 6

#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'

#define ELF_CLASS_32 1
#define ELF_CLASS_64 2

#define ELF_DATA_LSB 1
#define ELF_DATA_MSB 2

#define ELF_TYPE_DYN 3

#define ELF_MACHINE_386 3

#define ELF_SHT_PROGBITS 1
#define ELF_SHT_NOBITS 8

#define ELF_PT_DYNAMIC 2

#define ELF_SHN_UNDEF 0
#define ELF_SHN_LORESERVE 0xFF00
#define ELF_SHN_ABS 0xFFF1

#define ELF_STB_GLOBAL 1

#define ELF_DT_NULL 0
#define ELF_DT_NEEDED 1
#define ELF_DT_PLTRELSZ 2
#define ELF_DT_HASH 4
#define ELF_DT_STRTAB 5
#define ELF_DT_SYMTAB 6
#define ELF_DT_STRSZ 10
#define ELF_DT_SYMENT 11
#define ELF_DT_REL 17
#define ELF_DT_RELSZ 18
#define ELF_DT_RELENT 19
#define ELF_DT_PLTREL 20
#define ELF_DT_JMPREL 23

#define ELF_SH_GET32(hdr, idx) ((elf32_section_header_t*)((void*)(hdr) + (hdr)->sh_offset + (hdr)->sh_ent_size * (idx)))
#define ELF_SH_GET64(hdr, idx) ((elf64_section_header_t*)((void*)(hdr) + (hdr)->sh_offset + (hdr)->sh_ent_size * (idx)))
#define ELF_SH_CONTENT(hdr, sh) ((void*)(hdr) + (sh)->offset)

#define ELF_PH_GET32(hdr, idx) ((elf32_program_header_t*)((void*)(hdr) + (hdr)->ph_offset + (hdr)->ph_ent_size * (idx)))
#define ELF_PH_GET64(hdr, idx) ((elf64_program_header_t*)((void*)(hdr) + (hdr)->ph_offset + (hdr)->ph_ent_size * (idx)))
#define ELF_PH_CONTENT(hdr, ph) ((void*)(hdr) + (ph)->offset)

#define ELF_ST_BIND(info) ((info) >> 4)

#define ELF_R_SYM(info) ((info) >> 8)
#define ELF_R_TYPE(info) ((unsigned char)(info))

typedef struct {
	uint8_t ident[16];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t ph_offset;
	uint32_t sh_offset;
	uint32_t flags;
	uint16_t eh_size;
	uint16_t ph_ent_size;
	uint16_t ph_num;
	uint16_t sh_ent_size;
	uint16_t sh_num;
	uint16_t sh_str_ndx;
} elf32_header_t;

typedef struct {
	uint8_t ident[16];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint64_t entry;
	uint64_t ph_offset;
	uint64_t sh_offset;
	uint32_t flags;
	uint16_t eh_size;
	uint16_t ph_ent_size;
	uint16_t ph_num;
	uint16_t sh_ent_size;
	uint16_t sh_num;
	uint16_t sh_str_ndx;
} elf64_header_t;

typedef struct {
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t file_size;
	uint32_t mem_size;
	uint32_t flags;
	uint32_t align;
} elf32_program_header_t;

typedef struct {
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t vaddr;
	uint64_t paddr;
	uint64_t file_size;
	uint64_t mem_size;
	uint64_t align;
} elf64_program_header_t;

typedef struct {
	uint32_t name;
	uint32_t type;
	uint32_t flags;
	uint32_t vaddr;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t addr_align;
	uint32_t ent_size;
} elf32_section_header_t;

typedef struct {
	uint32_t name;
	uint32_t type;
	uint64_t flags;
	uint64_t vaddr;
	uint64_t offset;
	uint64_t size;
	uint32_t link;
	uint32_t info;
	uint64_t addr_align;
	uint64_t ent_size;
} elf64_section_header_t;

typedef struct {
	uint32_t name;
	uint32_t value;
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;
} elf32_symbol_t;

typedef struct {
	uint32_t name;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;
	uint64_t addr;
	uint64_t size;
} elf64_symbol_t;

typedef struct {
	uint32_t offset;
	uint32_t info;
} elf32_relocation_t;

typedef struct {
	uint64_t offset;
	uint64_t info;
} elf64_relocation_t;

typedef struct {
	int tag;
	uint32_t val;
} elf32_dynamic_t;

typedef struct {
	int64_t tag;
	uint64_t val;
} elf64_dynamic_t;

uintptr_t conv_elf32(void *buf, int swap);
uintptr_t conv_elf64(void *buf, int swap);

#endif		// __ELF_H__
