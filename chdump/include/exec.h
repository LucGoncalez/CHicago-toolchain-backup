// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:30 BRT
// Last edited on February 11 of 2019, at 16:46 BRT

#ifndef __EXEC_H__
#define __EXEC_H__

#include <context.h>
#include <stdio.h>

#define REGISTER_EXEC(name, namestr, gen) static __attribute__((constructor)) void name ## _register(void) { exec_register(namestr, gen); }

typedef struct {
	char *name;
	int (*gen)(context_t*, char*);
} exec_t;

typedef struct exec_list_s {
	exec_t *exec;
	struct exec_list_s *next;
} exec_list_t;

int exec_register(char *name, int (*gen)(context_t*, char*));
void exec_list_all();
int exec_gen(context_t *context, char *file);

#endif		// __EXEC_H__
