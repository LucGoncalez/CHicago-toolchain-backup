// File author is Ítalo Lima Marconato Matias
//
// Created on January 10 of 2019, at 11:37 BRT
// Last edited on January 12 of 2019, at 21:39 BRT

#ifndef __ELF32_H__
#define __ELF32_H__

#include <stdint.h>

#define ELF32_MACHINE_NONE 0
#define ELF32_MACHINE_386 3

#define ELF32_SHT_PROGBITS 1
#define ELF32_SHT_SYMTAB 2
#define ELF32_SHT_STRTAB 3
#define ELF32_SHT_RELA 4
#define ELF32_SHT_HASH 5
#define ELF32_SHT_DYNAMIC 6
#define ELF32_SHT_NOTE 7
#define ELF32_SHT_NOBITS 8
#define ELF32_SHT_REL 9
#define ELF32_SHT_SHLIB 10
#define ELF32_SHT_DYNSYM 11
#define ELF32_SHT_LOPROC 0x70000000
#define ELF32_SHT_HIPROC 0x7FFFFFFF
#define ELF32_SHT_LOUSER 0x80000000
#define ELF32_SHT_HIUSER 0xFFFFFFFF

#define ELF32_SHF_WRITE 0x01
#define ELF32_SHF_ALLOC 0x02
#define ELF32_SHF_EXECINSTR 0x04

#define ELF32_SHN_UNDEF 0
#define ELF32_SHN_LORESERVE 0xFF00
#define ELF32_SHN_LOPROC 0xFF00
#define ELF32_SHN_HIPROC 0xFF1F
#define ELF32_SHN_ABS 0xFFF1
#define ELF32_SHN_COMMON 0xFFF2
#define ELF32_SHN_HIRESERVE 0xFFFF

#define ELF32_R_386_32 1
#define ELF32_R_386_PC32 2
#define ELF32_R_386_16 20
#define ELF32_R_386_PC16 21
#define ELF32_R_386_8 22
#define ELF32_R_386_PC8 23

#define ELF32_R_SYM(info) ((info) >> 8)
#define ELF32_R_TYPE(info) ((uint8_t)(info))
#define ELF32_R_INFO(sym, type) (((sym) << 8) + (uint8_t)(type))

#define ELF32_ST_BIND(info) ((info) >> 4)
#define ELF32_ST_TYPE(info) ((info) & 0xF)
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xF))

#define ELF32_SH_GET(header, index) ((elf32_section_header_t*)((char*)(header) + (header)->sh_offset + (header)->sh_ent_size * (index)))
#define ELF32_SH_CONTENT(header, section) ((char*)(header) + (section)->offset)

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
	uint32_t offset;
	uint32_t info;
} elf32_rel_t;

typedef struct {
	uint32_t offset;
	uint32_t info;
	int addend;
} elf32_rela_t;

typedef struct {
	uint32_t name;
	uint32_t value;
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;
} elf32_symbol_t;

#endif		// __ELF_H__
