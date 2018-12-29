// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 14:57 BRT
// Last edited on December 28 of 2018, at 18:03 BRT

#ifndef __ARCH_H__
#define __ARCH_H__

#include <lexer.h>
#include <parser.h>
#include <codegen.h>

#define REGISTER_ARCH(name, namestr, lex, parse, ttype, tfree, tprint) static __attribute__((constructor)) void name ## _register(void) { arch_register(namestr, lex, parse, ttype, tfree, tprint); }

typedef struct {
	char *name;
	token_t *(*lex)(lexer_t*, token_t*, token_t*);
	node_t *(*parse)(parser_t*, node_t*);
	uint8_t (*ttype)(char*);
	void (*tfree)(token_t*);
	void (*tprint)(token_t*);
} arch_t;

typedef struct arch_list_s {
	arch_t *arch;
	struct arch_list_s *next;
} arch_list_t;

int arch_register(char *name, token_t *(*lex)(lexer_t*, token_t*, token_t*), node_t *(*parse)(parser_t*, node_t*), uint8_t (*ttype)(char*), void (*tfree)(token_t*), void (*tprint)(token_t*));
int arch_select(char *name);
void arch_list_all();
token_t *arch_lex(lexer_t *lexer, token_t *list, token_t *cur);
node_t *arch_parse(parser_t *parser, node_t *cur);
uint8_t arch_ttype(char *ident);
void arch_tfree(token_t *token);
void arch_tprint(token_t *token);

#endif		// __ARCH_H__
