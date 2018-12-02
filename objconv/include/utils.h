// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 10:02 BRT
// Last edited on November 15 of 2018, at 13:54 BRT

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

typedef struct section {
	char *name;
	uintptr_t vaddr;
	uint8_t flags;
	uintptr_t size;
	uint8_t *data;
	uintptr_t data_max;
	uintptr_t data_count;
	struct section *next;
} section_t;

typedef struct symbol {
	char *name;
	uintptr_t vaddr;
	uintptr_t flags;
	struct symbol *next;
} symbol_t;

typedef struct relocation {
	char *name;
	uint8_t op;
	uintptr_t vaddr;
	struct relocation *next;
} relocation_t;

typedef struct dependency {
	char *name;
	struct dependency *next;
} dependency_t;

#ifndef __UTILS__
extern section_t *sections;
extern uintptr_t section_count;
extern symbol_t *symbols;
extern uintptr_t symbol_count;
extern relocation_t *relocations;
extern uintptr_t relocation_count;
extern dependency_t *dependencies;
extern uintptr_t dependency_count;
#endif

section_t *add_section(char *name, uintptr_t vaddr, uintptr_t size, uint8_t flags);
symbol_t *add_symbol(char *name, uintptr_t vaddr, uint8_t flags);
relocation_t *add_relocation(char *name, uint8_t op, uintptr_t vaddr);
dependency_t *add_dependency(char *name);
void write_section_data(char *name, uint8_t data);
uint32_t get_sh_size32(void);
uint64_t get_sh_size64(void);
uint32_t get_st_size32(void);
uint64_t get_st_size64(void);
uint32_t get_rel_size32(void);
uint64_t get_rel_size64(void);
uint32_t get_dep_size32(void);
uint64_t get_dep_size64(void);

char *replace_extension(char *fname, char *newext);
void *read_file(char *fname);

#endif		// __UTILS_H__
