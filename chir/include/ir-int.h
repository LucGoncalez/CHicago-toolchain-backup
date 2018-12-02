// File author is Ítalo Lima Marconato Matias
//
// Created on November 24 of 2018, at 09:24 BRT
// Last edited on November 25 of 2018, at 14:42 BRT

#ifndef __IR_INT_H__
#define __IR_INT_H__

#include <stdint.h>

#define IR_TTYPE_LVARIABLE 0x00
#define IR_TTYPE_GVARIABLE 0x01
#define IR_TTYPE_METHOD 0x02
#define IR_TTYPE_SNUMBER 0x03
#define IR_TTYPE_UNUMBER 0x04
#define IR_TTYPE_STRING 0x05
#define IR_TTYPE_TYPE 0x06
#define IR_TTYPE_PAREN 0x07
#define IR_TTYPE_BRACE 0x08
#define IR_TTYPE_IDENT 0x09
#define IR_TTYPE_SEP 0x0A
#define IR_TTYPE_ASSIGN 0x0B
#define IR_TTYPE_EOS 0x0C

typedef struct {
	uint8_t type;
	union {
		int32_t sint_val;
		uint32_t uint_val;
		char char_val;
		char *str_val;
	};
} ir_tok_t;

typedef struct {
	char *text;
	int length;
	int pos;
	ir_tok_t cur;
} ir_lexer_t;

ir_lexer_t *ir_create_lexer(char *text);
void ir_free_lexer(ir_lexer_t *lexer);
int ir_lex_check(ir_lexer_t *lexer);
ir_tok_t *ir_get_cur_tok(ir_lexer_t *lexer);
ir_tok_t *ir_lex_next_tok(ir_lexer_t *lexer);
void ir_reset_lexer(ir_lexer_t *lexer);

#endif		// __IR_INT_H__
