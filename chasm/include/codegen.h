// File author is Ítalo Lima Marconato Matias
//
// Created on December 28 of 2018, at 17:16 BRT
// Last edited on December 28 of 2018, at 21:30 BRT

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <stdint.h>

#define CODEGEN_LABEL_LOCAL 0x00
#define CODEGEN_LABEL_GLOBAL 0x01
#define CODEGEN_LABEL_EXTERN 0x02

typedef struct codegen_section_s {
	char *name;
	uintptr_t size;
	uintptr_t max;
	uint8_t *data;
	struct codegen_section_s *next;
} codegen_section_t;

typedef struct codegen_label_s {
	char *name;
	char *sect;
	uint8_t type;
	uintptr_t loc;
	int local_resolved;
	struct codegen_label_s *next;
} codegen_label_t;

typedef struct codegen_reloc_s {
	char *name;
	char *sect;
	int resolved;
	uint8_t size;
	uintptr_t loc;
	struct codegen_reloc_s *next;
} codegen_reloc_t;

typedef struct {
	codegen_section_t *current_section;
	codegen_section_t *sections;
	codegen_label_t *labels;
	codegen_reloc_t *relocs;
	node_t *ast;
} codegen_t;

codegen_t *codegen_new(node_t *ast);
void codegen_free(codegen_t *codegen);
void codegen_write_byte(codegen_t *codegen, uint8_t data);
void codegen_write_word(codegen_t *codegen, uint16_t data);
void codegen_write_dword(codegen_t *codegen, uint32_t data);
void codegen_write_qword(codegen_t *codegen, uint64_t data);
void codegen_add_relocation(codegen_t *codegen, char *name, char *sect, uint8_t size, uintptr_t loc);
void codegen_select_section(codegen_t *codegen, char *name);
uintptr_t codegen_get_section_start(codegen_t *codegen, char *name);
void codegen_add_label(codegen_t *codegen, char *name, uint8_t type, uintptr_t loc);
codegen_label_t *codegen_get_label(codegen_t *codegen, char *name);
int codegen_have_label(codegen_t *codegen, char *name);
int codegen_gen(codegen_t *codegen);

#endif		// __CODEGEN_H__
