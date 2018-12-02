// File author is Ítalo Lima Marconato Matias
//
// Created on November 19 of 2018, at 13:44 BRT
// Last edited on November 30 of 2018, at 21:03 BRT

#ifndef __IR_H__
#define __IR_H__

#include <stdint.h>

#define IR_TYPE_VOID 0x00
#define IR_TYPE_INT8 0x01
#define IR_TYPE_UINT8 0x02
#define IR_TYPE_INT16 0x04
#define IR_TYPE_UINT16 0x08
#define IR_TYPE_INT32 0x10
#define IR_TYPE_UINT32 0x20
#define IR_TYPE_STRING 0x40
#define IR_TYPE_LOCAL 0x80
#define IR_TYPE_GLOBAL 0x100
#define IR_TYPE_METHOD 0x200

#define IR_INSTR_MOV 0x00
#define IR_INSTR_ADD 0x01
#define IR_INSTR_SUB 0x02
#define IR_INSTR_MUL 0x03
#define IR_INSTR_DIV 0x04
#define IR_INSTR_MOD 0x05
#define IR_INSTR_SHL 0x06
#define IR_INSTR_SHR 0x07
#define IR_INSTR_AND 0x08
#define IR_INSTR_OR 0x09
#define IR_INSTR_XOR 0x0A
#define IR_INSTR_CMP 0x0B
#define IR_INSTR_BR 0x0C
#define IR_INSTR_BRC 0x0D
#define IR_INSTR_CALL 0x0E
#define IR_INSTR_CALLC 0x0F
#define IR_INSTR_RETURN 0x10

typedef union {
	int8_t int8_val;
	uint8_t uint8_val;
	int16_t int16_val;
	uint16_t uint16_val;
	int32_t int32_val;
	uint32_t uint32_val;
	char *str_val;
} ir_val_t;

typedef struct {
	uint16_t type;
	ir_val_t val;
} ir_oper_t;

typedef struct {
	ir_oper_t *assign;
	uint16_t type;
	char *name;
} ir_arg_t;

typedef struct {
	uint8_t op;
	uint32_t operand_count;
	ir_oper_t **operands;
} ir_instr_t;

typedef struct {
	char *name;
	uint32_t arg_count;
	ir_arg_t **args;
	uint32_t local_count;
	uint16_t type;
	uint32_t body_count;
	ir_instr_t **body;
} ir_method_t;

typedef struct {
	uint32_t global_count;
	ir_arg_t **globals;
	uint32_t method_count;
	ir_method_t **methods;
} ir_module_t;

ir_oper_t *ir_create_oper(uint16_t type);
void ir_free_oper(ir_oper_t *oper);

ir_arg_t *ir_create_arg(uint16_t type, char *name);
void ir_free_arg(ir_arg_t *arg);

ir_instr_t *ir_create_instr(uint8_t op, uint32_t opers);
void ir_free_instr(ir_instr_t *instr);
int ir_check_instr(ir_module_t *module, ir_method_t *method, ir_instr_t *instr);

ir_method_t *ir_create_method(char *name, uint32_t args, uint32_t locals, uint16_t type);
void ir_free_method(ir_method_t *method);
int ir_check_method(ir_module_t *module, ir_method_t *method);
int ir_add_instr(ir_method_t *method, ir_instr_t *instr);

ir_module_t *ir_create_module(uint32_t globals);
ir_module_t *ir_module_from_text(char *text);
char *ir_module_to_text(ir_module_t *module);
void ir_free_module(ir_module_t *module);
int ir_check_module(ir_module_t *module);
int ir_add_method(ir_module_t *module, ir_method_t *method);

#endif		// __IR_H__
