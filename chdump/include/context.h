// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:38 BRT
// Last edited on February 16 of 2019, at 09:54 BRT

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <stdint.h>

#define CONTEXT_SYMBOL_GLOBAL 0x00
#define CONTEXT_SYMBOL_EXTERN 0x01
#define CONTEXT_SYMBOL_LOCAL 0x02

typedef struct context_section_s {
	char *name;
	uintptr_t size;
	uintptr_t virt;
	uintptr_t off;
	uint8_t *data;
	struct context_section_s *next;
} context_section_t;

typedef struct context_symbol_s {
	char *name;
	char *sect;
	uint8_t type;
	uintptr_t loc;
	struct context_symbol_s *next;
} context_symbol_t;

typedef struct context_reloc_s {
	char *name;
	char *sect;
	uint8_t size;
	uintptr_t loc;
	int increment;
	int relative;
	struct context_reloc_s *next;
} context_reloc_t;

typedef struct {
	context_section_t *sections;
	context_symbol_t *symbols;
	context_reloc_t *relocs;
} context_t;

context_t *context_new();
void context_free(context_t *context);
void context_add_section(context_t *context, char *name, uintptr_t size, uintptr_t virt, uintptr_t off, uint8_t *data);
void context_add_symbol(context_t *context, char *name, char *sect, uint8_t type, uintptr_t loc);
void context_add_relocation(context_t *context, char *name, char *sect, uint8_t size, uintptr_t loc, int inc, int rel);

#endif		// __CONTEXT_H__
