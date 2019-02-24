// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 14:35 BRT
// Last edited on February 23 of 2019, at 21:14 BRT

#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>

#define TOK_TYPE_IDENTIFIER 0x00
#define TOK_TYPE_TYPE 0x01
#define TOK_TYPE_INSTRUCTION 0x02
#define TOK_TYPE_CONDITION 0x03
#define TOK_TYPE_NUMBER 0x04
#define TOK_TYPE_STRING 0x05
#define TOK_TYPE_FLOAT 0x06
#define TOK_TYPE_COMMA 0x07
#define TOK_TYPE_COLON 0x08
#define TOK_TYPE_SEMICOLON 0x09
#define TOK_TYPE_OPEN_PAREN 0x0A
#define TOK_TYPE_CLOSE_PAREN 0x0B
#define TOK_TYPE_OPEN_BRACE 0x0C
#define TOK_TYPE_CLOSE_BRACE 0x0D
#define TOK_TYPE_AT 0x0E
#define TOK_TYPE_QUESTION 0x0F
#define TOK_TYPE_PERCENT 0x10
#define TOK_TYPE_DOLLAR 0x11
#define TOK_TYPE_HASH 0x12

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

lexer_t *lexer_new(char *filename, char *text);
void lexer_free(lexer_t *lexer);
token_t *lexer_lex(lexer_t *lexer);

#endif		// __LEXER_H__
