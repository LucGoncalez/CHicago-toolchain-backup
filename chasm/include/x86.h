// File author is Ítalo Lima Marconato Matias
//
// Created on December 03 of 2018, at 19:52 BRT
// Last edited on December 28 of 2018, at 09:45 BRT

#ifndef __X86_H__
#define __X86_H__

#include <parser.h>

#define TOK_TYPE_COMMA (TOK_TYPE_COLON + 0x01)
#define TOK_TYPE_ADD (TOK_TYPE_COLON + 0x02)
#define TOK_TYPE_SUB (TOK_TYPE_COLON + 0x03)
#define TOK_TYPE_MUL (TOK_TYPE_COLON + 0x04)
#define TOK_TYPE_OBRAC (TOK_TYPE_COLON + 0x05)
#define TOK_TYPE_CBRAC (TOK_TYPE_COLON + 0x06)
#define TOK_TYPE_REGISTER (TOK_TYPE_COLON + 0x07)

#define NODE_TYPE_REGISTER (NODE_TYPE_LABEL + 0x01)
#define NODE_TYPE_INSTRUCTION (NODE_TYPE_LABEL + 0x02)

typedef struct {
	node_t base;
	char *name;
} register_node_t, instruction_node_t;

#endif		// __X86_H__
