// File author is Ítalo Lima Marconato Matias
//
// Created on December 01 of 2018, at 09:51 BRT
// Last edited on December 01 of 2018, at 10:28 BRT

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <ir.h>

#define CODEGEN_REGISTER_ARCH __attribute__((constructor))

typedef struct {
	char *name;
	char *(*gen_instr)(ir_module_t*, ir_method_t*, ir_instr_t*);
	char *(*gen_method)(ir_module_t*, ir_method_t*);
	char *(*gen_module)(ir_module_t*);
} codegen_arch_t;

typedef struct codegen_arch_list_s {
	codegen_arch_t *arch;
	struct codegen_arch_list_s *next;
} codegen_arch_list_t;

codegen_arch_t *codegen_create_arch(char *name);
int codegen_register_arch(codegen_arch_t *arch);
int codegen_select_arch(char *name);
char *codegen_gen_instr(ir_module_t *module, ir_method_t *method, ir_instr_t *instr);
char * codegen_gen_method(ir_module_t *module, ir_method_t *method);
char * codegen_gen_module(ir_module_t *module);

#endif
