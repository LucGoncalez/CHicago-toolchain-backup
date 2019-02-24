// File author is Ítalo Lima Marconato Matias
//
// Created on December 27 of 2018, at 11:43 BRT
// Last edited on February 23 of 2019, at 21:57 BRT

#ifndef __PARSER_H__
#define __PARSER_H__

#include <lexer.h>

#define NODE_TYPE_METHOD_DEF 0x00
#define NODE_TYPE_VARIABLE_DEF 0x01
#define NODE_TYPE_TYPECAST 0x02
#define NODE_TYPE_INSTRUCTION 0x03
#define NODE_TYPE_CONDITION 0x04
#define NODE_TYPE_IDENTIFIER 0x05
#define NODE_TYPE_NUMBER 0x06
#define NODE_TYPE_STRING 0x07
#define NODE_TYPE_FLOAT 0x08
#define NODE_TYPE_METHOD 0x09
#define NODE_TYPE_VAR 0x0A

typedef struct node_s {
	uint8_t type;
	char *filename;
	int line;
	int col;
	struct node_s *next;
	struct node_s *prev;
	struct node_s *childs;
} node_t;

typedef struct {
	node_t base;
	node_t *name;
	node_t *type;
} method_def_node_t;

typedef struct {
	node_t base;
	node_t *name;
	node_t *type;
} variable_def_node_t;

typedef struct {
	node_t base;
	node_t *type;
} typecast_node_t;

typedef struct {
	node_t base;
	node_t *name;
} instruction_node_t;

typedef struct {
	node_t base;
	char *value;
} condition_node_t, identifier_node_t, string_node_t;

typedef struct {
	node_t base;
	uintmax_t value;
} number_node_t;

typedef struct {
	node_t base;
	long double value;
} float_node_t;

typedef struct {
	node_t base;
	char *name;
} method_node_t;

typedef struct {
	token_t *tokens;
	token_t *position;
} parser_t;

void node_free_list(node_t *node);
node_t *node_rewind_list(node_t *node);
void node_free(node_t *node);
parser_t *parser_new(token_t *tokens);
void parser_free(parser_t *parser);
int parser_check_noval(parser_t *parser, uint8_t type);
int parser_check_val(parser_t *parser, uint8_t type, char *val);
token_t *parser_accept_noval(parser_t *parser, uint8_t type);
token_t *parser_accept_val(parser_t *parser, uint8_t type, char *val);
token_t *parser_expect_noval(parser_t *parser, uint8_t type);
token_t *parser_expect_val(parser_t *parser, uint8_t type, char *val);
node_t *parser_parse_identifier(parser_t *parser, node_t *cur);
node_t *parser_parse_number(parser_t *parser, node_t *cur);
node_t *parser_parse_string(parser_t *parser, node_t *cur);
node_t *parser_parse_float(parser_t *parser, node_t *cur);
node_t *parser_parse(parser_t *parser);

#endif		// __PARSER_H__
