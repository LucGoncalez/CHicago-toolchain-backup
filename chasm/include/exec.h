// File author is Ítalo Lima Marconato Matias
//
// Created on January 10 of 2018, at 10:56 BRT
// Last edited on January 10 of 2019, at 10:56 BRT

#ifndef __EXEC_H__
#define __EXEC_H__

#include <codegen.h>
#include <stdio.h>

#define REGISTER_EXEC(name, namestr, help, option, gen) static __attribute__((constructor)) void name ## _register(void) { exec_register(namestr, help, option, gen); }

typedef struct {
	char *name;
	void (*help)();
	int (*option)(int, char**, int);
	int (*gen)(codegen_t*, FILE*);
} exec_t;

typedef struct exec_list_s {
	exec_t *exec;
	struct exec_list_s *next;
} exec_list_t;

int exec_register(char *name, void (*help)(), int (*option)(int, char**, int), int (*gen)(codegen_t*, FILE*));
int exec_select(char *name);
void exec_list_all();
void exec_help_all();
void exec_help();
int exec_option(int argc, char **argv, int i);
int exec_gen(codegen_t *codegen, FILE *out);

#endif		// __EXEC_H__
