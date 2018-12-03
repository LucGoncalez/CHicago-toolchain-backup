// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 14:35 BRT
// Last edited on December 03 of 2018, at 19:10 BRT

#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>

#define TOK_TYPE_IDENTIFIER 0x00
#define TOK_TYPE_DIRECTIVE 0x01
#define TOK_TYPE_NUMBER 0x02
#define TOK_TYPE_STRING 0x03
#define TOK_TYPE_EOS 0x04
#define TOK_TYPE_COLON 0x05

typedef struct token_s {
	uint8_t type;
	char *filename;
	char *value;
	int line;
	int col;
	struct token_s *next;
	struct token_s *prev;
} token_t;

typedef struct {
	char *filename;
	char *text;
	int length;
	int line;
	int col;
	int pos;
} lexer_t;

void token_free_list(token_t *token);
token_t *token_rewind_list(token_t *token);

void token_free(token_t *token);
void token_print(token_t *token);

lexer_t *lexer_new(char *filename, char *text);
void lexer_free(lexer_t *lexer);
token_t *lexer_lex(lexer_t *lexer);

#endif		// __LEXER_H__
