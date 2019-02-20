// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:30 BRT
// Last edited on February 20 of 2019, at 17:35 BRT

#ifndef __EXEC_H__
#define __EXEC_H__

#include <context.h>
#include <stdio.h>

#define REGISTER_EXEC(name, namestr, help, option, load, add_dep, gen) static __attribute__((constructor)) void name ## _register(void) { exec_register(namestr, help, option, load, add_dep, gen); }

typedef struct {
	char *name;
	void (*help)();
	int (*option)(int, char**, int);
	int (*load)(context_t*, char*);
	int (*add_dep)(context_t*, char*, char*);
	int (*gen)(context_t*, FILE*);
} exec_t;

typedef struct exec_list_s {
	exec_t *exec;
	struct exec_list_s *next;
} exec_list_t;

int exec_register(char *name, void (*help)(), int (*option)(int, char**, int), int (*load)(context_t*, char*), int (*add_dep)(context_t*, char*, char*), int (*gen)(context_t*, FILE*));
void exec_list_all();
void exec_help_all();
int exec_option(char *format, int argc, char **argv, int i);
int exec_load(context_t *context, char *fname, char *file, char **format);
int exec_add_dep(context_t *context, char *fname, char *file);
int exec_gen(char *format, context_t *context, FILE *out);

#endif		// __EXEC_H__
