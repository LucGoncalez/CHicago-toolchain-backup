// File author is Ítalo Lima Marconato Matias
//
// Created on February 18 of 2019, at 12:53 BRT
// Last edited on February 18 of 2019, at 13:36 BRT

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <stdint.h>
#include <inttypes.h>

typedef struct linker_variable_s {
	char *name;
	uintmax_t value_num;
	char *value_sym;
	int value_type;
	struct linker_variable_s *next;
} linker_variable_t;

typedef struct linker_section_wildcard_s {
	char *postfix;
	char *prefix;
	struct linker_section_wildcard_s *next;
} linker_section_wildcard_t;

typedef struct linker_section_s {
	char *name;
	uintmax_t align;
	uintmax_t vaddr;
	linker_section_wildcard_t *sections;
	struct linker_section_s *next;
} linker_section_t;

typedef struct {
	linker_section_t *sections;
	linker_variable_t *vars;
	linker_variable_t cur;
	uintmax_t entry_num;
	char *entry_sym;
	int entry_type;
} linker_options_t;

linker_options_t *linker_options_new();
linker_section_t *linker_add_section(linker_options_t *options, char *name);
int linker_add_section_wildcard(linker_section_t *section, char *name);
int linker_assign_variable(linker_options_t *options, char *name, uintmax_t num, char *sym);
int linker_parse_script(linker_options_t *options, char *script, char *code);

#endif		// __SCRIPT_H__
