// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 14:57 BRT
// Last edited on December 29 of 2018, at 22:28 BRT

#ifndef __ARCH_H__
#define __ARCH_H__

#include <lexer.h>
#include <parser.h>
#include <codegen.h>

#define REGISTER_ARCH(name, namestr, help, option, lex, parse, gen, ttype, tfree, tprint) static __attribute__((constructor)) void name ## _register(void) { arch_register(namestr, help, option, lex, parse, gen, ttype, tfree, tprint); }

typedef struct {
	char *name;
	void (*help)();
	int (*option)(int, char**, int);
	token_t *(*lex)(lexer_t*, token_t*, token_t*);
	node_t *(*parse)(parser_t*, node_t*);
	int (*gen)(codegen_t*, node_t*);
	uint8_t (*ttype)(char*);
	void (*tfree)(token_t*);
	void (*tprint)(token_t*);
} arch_t;

typedef struct arch_list_s {
	arch_t *arch;
	struct arch_list_s *next;
} arch_list_t;

int arch_register(char *name, void (*help)(), int (*option)(int, char**, int), token_t *(*lex)(lexer_t*, token_t*, token_t*), node_t *(*parse)(parser_t*, node_t*), int (*gen)(codegen_t*, node_t*), uint8_t (*ttype)(char*), void (*tfree)(token_t*), void (*tprint)(token_t*));
int arch_select(char *name);
void arch_list_all();
void arch_help_all();
void arch_help();
int arch_option(int argc, char **argv, int i);
token_t *arch_lex(lexer_t *lexer, token_t *list, token_t *cur);
node_t *arch_parse(parser_t *parser, node_t *cur);
int arch_gen(codegen_t *codegen, node_t *node);
uint8_t arch_ttype(char *ident);
void arch_tfree(token_t *token);
void arch_tprint(token_t *token);

#endif		// __ARCH_H__
