// File author is Ítalo Lima Marconato Matias
//
// Created on December 03 of 2018, at 19:52 BRT
// Last edited on December 28 of 2018, at 10:59 BRT

#ifndef __X86_H__
#define __X86_H__

#include <parser.h>

#define TOK_TYPE_COMMA (TOK_TYPE_COLON + 0x01)
#define TOK_TYPE_MUL (TOK_TYPE_COLON + 0x02)
#define TOK_TYPE_OBRAC (TOK_TYPE_COLON + 0x03)
#define TOK_TYPE_CBRAC (TOK_TYPE_COLON + 0x04)
#define TOK_TYPE_REGISTER (TOK_TYPE_COLON + 0x05)

#define NODE_TYPE_ADDRESS (NODE_TYPE_LABEL + 0x01)
#define NODE_TYPE_REGISTER (NODE_TYPE_LABEL + 0x02)
#define NODE_TYPE_INSTRUCTION (NODE_TYPE_LABEL + 0x03)

typedef struct {
	node_t base;
	int have_disp;
	int have_mul;
	int32_t disp;
	uint32_t mul;
} address_node_t;

typedef struct {
	node_t base;
	char *name;
} register_node_t, instruction_node_t;

#endif		// __X86_H__
