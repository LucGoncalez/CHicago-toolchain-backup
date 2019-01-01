// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 17:37 BRT
// Last edited on January 01 of 2019, at 15:37 BRT

#include <arch.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <x86.h>

static char *registers[30] = {
	"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
	"ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
	"ah", "bh", "ch", "dh",
	"al", "bl", "cl", "dl",
	"cs", "ds", "es", "fs", "gs", "ss"
};

static char *gregsb[8] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
static char *gregsw[8] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
static char *gregsd[8] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };

static char *mnemonics[158] = {
	"aaa", "aad", "aam", "aas", "call", "cbw", "cwde", "clc", "cld", "cli",
	"clts", "cmc", "cmpsb", "cmpsw", "cmpsd", "cwd", "cdq", "daa", "das",
	"dec", "hlt", "idiv", "imul", "inc", "insb", "insw", "insd", "int3", "int",
	"into", "iret", "iretw", "iretd", "ja", "jae", "jb", "jbe", "jc", "jcxz",
	"jecxz", "je", "jz", "jg", "jge", "jl", "jle", "jna", "jnae", "jnb", "jnbe",
	"jnc", "jne", "jng", "jnge", "jnl", "jnle", "jno", "jnp", "jns", "jnz", "jo",
	"jp", "jpe", "jpo", "js", "jz", "jmp", "lahf", "leave", "lgdt", "lidt", "lldt",
	"lmsw", "lodsb", "lodsw", "lodsd", "loop", "loope", "loopz", "loopne", "loopnz",
	"ltr", "movsb", "movsw", "movsd", "neg", "nop", "not", "outsb", "outsw", "outsd",
	"pop", "popa", "popaw", "popad", "popf", "popfw", "popfd", "push", "pusha",
	"pushaw", "pushad", "pushf", "pushfw", "pushfd", "ret", "retf", "retfw", "retfd",
	"sahf", "scasb", "scasw", "scasd", "seta", "setae", "setb", "setbe", "setc",
	"sete", "setg", "setge", "setl", "setle", "setna", "setnae", "setnb", "setnbe",
	"setnc", "setne", "setng", "setnge", "setnl", "setnle", "setno", "setnp", "setns",
	"setnz", "seto", "setp", "setpe", "setpo", "sets", "setz", "sgdt", "sidt", "smsw",
	"stc", "std", "sti", "stosb", "stosw", "stosd", "str", "verr", "verrw", "wait",
	"xlatb", "xlat",
};

struct {
	char *name;
	int opcode;
	int postop;
	int optype;
	int alt_addr;
	int extension;
	uint32_t args;
	uint32_t arg1;
	uint32_t arg2;
} instructions[206] = {
	{ "aaa", 0x37, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aad", 0xD5, 0x0A, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aam", 0xD4, 0x0A, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aas", 0x3F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "call", 0xE8, -1, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "call", 0xFF, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "call", 0x9A, -1, INSTR_TYPE_POINTER, 0, -1, 1, INSTR_ARG_POINTER, INSTR_ARG_NONE },
	
	{ "cbw", 0x98, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cwde", 0x98, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clc", 0xF8, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cld", 0xFC, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cli", 0xFA, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clts", 0x0F, 0x06, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cmc", 0xF5, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cmpsb", 0xA6, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cmpsw", 0xA7, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cmpsd", 0xA7, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cwd", 0x99, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cdq", 0x99, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "daa", 0x27, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "das", 0x2F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "dec", 0xFF, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "dec", 0x48, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "dec", 0x48, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "hlt", 0xF4, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "idiv", 0xF6, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "imul", 0xF7, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "inc", 0xFF, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "inc", 0x40, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "inc", 0x40, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "insb", 0x6C, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "insw", 0x6D, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "insd", 0x6D, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "int3", 0xCC, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "int", 0xCD, -1, INSTR_TYPE_BYTE, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "into", 0xCE, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "iret", 0xCF, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "iretw", 0xCF, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "iretd", 0xCF, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ja", 0x77, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jae", 0x73, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jb", 0x72, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jbe", 0x76, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jc", 0x72, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jcxz", 0xE3, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jecxz", 0xE3, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "je", 0x74, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jz", 0x74, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jg", 0x7F, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jge", 0x7D, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jl", 0x7C, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jle", 0x7E, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jna", 0x76, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnae", 0x72, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnb", 0x73, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnbe", 0x77, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnc", 0x73, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jne", 0x75, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jng", 0x7E, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnge", 0x7C, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnl", 0x7D, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnle", 0x7F, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jno", 0x71, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnp", 0x7B, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jns", 0x79, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jnz", 0x75, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jo", 0x70, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jp", 0x7A, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jpe", 0x7A, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jpo", 0x7B, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "js", 0x78, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jz", 0x74, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "ja", 0x87, -1, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jae", 0x0F, 0x83, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jb", 0x0F, 0x82, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jbe", 0x0F, 0x86, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jc", 0x0F, 0x82, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "je", 0x0F, 0x84, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jz", 0x0F, 0x84, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jg", 0x0F, 0x8F, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jge", 0x0F, 0x8D, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jl", 0x0F, 0x8C, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jle", 0x0F, 0x8E, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jna", 0x0F, 0x86, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnae", 0x0F, 0x82, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnb", 0x0F, 0x83, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnbe", 0x0F, 0x87, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnc", 0x0F, 0x83, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jne", 0x0F, 0x85, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jng", 0x0F, 0x8E, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnge", 0x0F, 0x8C, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnl", 0x0F, 0x8D, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnle", 0x0F, 0x8F, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jno", 0x0F, 0x81, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnp", 0x0F, 0x8B, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jns", 0x0F, 0x89, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jnz", 0x0F, 0x85, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jo", 0x0F, 0x80, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jp", 0x0F, 0x8A, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jpe", 0x0F, 0x8A, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jpo", 0x0F, 0x8B, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "js", 0x0F, 0x88, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jz", 0x0F, 0x84, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jmp", 0xE8, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "jmp", 0xE9, -1, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "jmp", 0xFF, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "jmp", 0xEA, -1, INSTR_TYPE_POINTER, 0, -1, 1, INSTR_ARG_POINTER, INSTR_ARG_NONE },
	
	{ "lahf", 0x9F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "leave", 0xC9, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lgdt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "lidt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "lldt", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "lmsw", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "lodsb", 0xAC, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "lodsw", 0xAD, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "lodsd", 0xAD, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "loop", 0xE2, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "loope", 0xE1, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "loopz", 0xE1, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "loopne", 0xE0, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "loopnz", 0xE0, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	
	{ "ltr", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "movsb", 0xA4, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "movsw", 0xA5, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "movsd", 0xA5, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "neg", 0xF7, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "nop", 0x90, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "not", 0xF6, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "outsb", 0x6E, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "outsw", 0x6F, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "outsd", 0x6F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pop", 0x58, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "pop", 0x58, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "popa", 0x61, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popaw", 0x61, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popad", 0x61, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "popf", 0x9D, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popfw", 0x9D, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popfd", 0x9D, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "push", 0x50, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "push", 0x50, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "push", 0x68, -1, INSTR_TYPE_DWORD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	
	{ "pusha", 0x60, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushaw", 0x60, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushad", 0x60, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pushf", 0x9C, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushfw", 0x9C, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushfd", 0x9C, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ret", 0xC3, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "ret", 0xC2, -1, INSTR_TYPE_WORD, 0, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "retf", 0xCB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retf", 0xCA, -1, INSTR_TYPE_WORD, 0, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "retfw", 0xCB, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfw", 0xCA, -1, INSTR_TYPE_WORD, 1, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "retfd", 0xCB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfd", 0xCA, -1, INSTR_TYPE_WORD, 0, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	
	{ "sahf", 0x9E, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "scasb", 0xAE, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "scasw", 0xAF, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "scasd", 0xAF, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "seta", 0x0F, 0x97, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setae", 0x0F, 0x93, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setb", 0x0F, 0x92, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setbe", 0x0F, 0x96, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setc", 0x0F, 0x92, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sete", 0x0F, 0x94, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setz", 0x0F, 0x94, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setg", 0x0F, 0x9F, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setge", 0x0F, 0x9D, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setl", 0x0F, 0x9C, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setle", 0x0F, 0x9E, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setna", 0x0F, 0x96, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnae", 0x0F, 0x92, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnb", 0x0F, 0x93, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnbe", 0x0F, 0x97, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnc", 0x0F, 0x93, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setne", 0x0F, 0x95, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setng", 0x0F, 0x9E, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnge", 0x0F, 0x9C, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnl", 0x0F, 0x9D, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnle", 0x0F, 0x9F, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setno", 0x0F, 0x91, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnp", 0x0F, 0x9B, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setns", 0x0F, 0x99, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setnz", 0x0F, 0x95, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "seto", 0x0F, 0x90, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setp", 0x0F, 0x9A, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setpe", 0x0F, 0x9A, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setpo", 0x0F, 0x9B, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sets", 0x0F, 0x98, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "setz", 0x0F, 0x94, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "sgdt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sidt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "smsw", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "stc", 0xF9, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "std", 0xFD, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sti", 0xFB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "stosb", 0xAA, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "stosw", 0xAB, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "stosd", 0xAB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "str", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "verr", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "verrw", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "wait", 0x9B, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "xlatb", 0xD7, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "xlat", 0xD7, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
};

static void x86_help(void) { }
static int x86_option(int argc, char **argv, int i) { (void)argc; (void)argv; (void)i; return 0; }

static void lexer_consume(lexer_t *lexer) {
	if (lexer->pos < lexer->length) {																							// We can increase the position?
		if (lexer->text[lexer->pos] == '\n') {																					// Yes, new line?
			lexer->line++;																										// Yes! Increase the line count
			lexer->col = 0;																										// And set the current column to zero
		} else {
			lexer->col++;																										// No, just increase the current column
		}

		lexer->pos++;																											// Increase the position
	}
}

static token_t *lexer_new_token(token_t *list, token_t *cur) {
	if (cur == NULL) {																											// We're at the start of the list?
		return list;																											// Yes :)
	}
	
	cur->next = calloc(1, sizeof(token_t));																						// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																											// Failed...
	}
	
	cur->next->prev = cur;																										// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static int x86_find_register(char *name) {
	for (int i = 0; i < 30; i++) {
		if ((strlen(registers[i]) == strlen(name)) && !strcasecmp(registers[i], name)) {										// Found?
			return 1;																											// Yes :)
		}
	}
	
	return 0;
}

static token_t *x86_lex(lexer_t *lexer, token_t *list, token_t *cur) {
	if (lexer == NULL || list == NULL) {																						// Null pointer checks
		return NULL;
	} else if (lexer->text[lexer->pos] == ',' || lexer->text[lexer->pos] == '*' || lexer->text[lexer->pos] == '[' ||
			   lexer->text[lexer->pos] == ']') {																				// Single character token?
		cur = lexer_new_token(list, cur);																						// Yes, create a new token at the end of the list
		
		if (cur == NULL) {
			return NULL;																										// Failed...
		}
		
		cur->type = lexer->text[lexer->pos] == ',' ? TOK_TYPE_COMMA : (lexer->text[lexer->pos] == '*' ? TOK_TYPE_MUL :
					(lexer->text[lexer->pos] == '[' ? TOK_TYPE_OBRAC : TOK_TYPE_CBRAC));										// Set the type
		cur->filename = lexer->filename;																						// Set the filename
		cur->line = lexer->line;																								// Set the line
		cur->col = lexer->col;																									// And the column
		
		lexer_consume(lexer);																									// Consume the character
		
		return cur;
	} else {
		return NULL;																											// ...
	}
}

static node_t *parser_new_node(node_t *cur, size_t size) {
	if (cur == NULL) {																											// We're at the start of the list?
		return (node_t*)calloc(1, size);																						// Yes :)
	}
	
	cur->next = calloc(1, size);																								// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																											// Failed...
	}
	
	cur->next->prev = cur;																										// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static node_t *parser_parse_register(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_REGISTER);																// Get our token
	
	if (tok == NULL) {
		return NULL;																											// Failed...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(register_node_t));																// Create the node
	
	if (ret != NULL) {																											// Failed?
		ret->type = NODE_TYPE_REGISTER;																							// No, so let's set the type
		((register_node_t*)ret)->name = tok->value;																				// And the value!
	}
	
	return ret;
}

static node_t *parser_parse_pointer(parser_t *parser, node_t *cur, node_t *left) {
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																		// Identifier?
		left->next = parser_parse_identifier(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																	// Number?
		left->next = parser_parse_number(parser, NULL);
	}
	
	if (left->next == NULL) {
		return NULL;																											// ...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(node_t));																			// Create the node
	
	if (ret != NULL) {																											// Failed?
		ret->type = NODE_TYPE_POINTER;																							// No, so let's set the type
		ret->childs = left;																										// The val
	}
	
	return ret;
}

static node_t *parser_parse_address(parser_t *parser, node_t *cur) {
	node_t *val = NULL;																											// Let's get the val
	int have_disp = 0;
	int have_mul = 0;
	int32_t disp = 0;
	uint32_t mul = 0;
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																		// Identifier?
		val = parser_parse_identifier(parser, NULL);
		
		if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																		// Pointer?
			val = parser_parse_pointer(parser, NULL, val);																		// Yes
		}
	} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																	// Number?
		val = parser_parse_number(parser, NULL);
		
		if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																		// Pointer?
			val = parser_parse_pointer(parser, NULL, val);																		// Yes
		}
	} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {																	// Register?
		val = parser_parse_register(parser, NULL);
	}
	
	if (val == NULL) {
		return NULL;																											// ...
	}
	
	if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																			// Displacement?
		token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);															// Yes, now we should have a number
		char *endptr = NULL;
		
		if (tok == NULL) {
			return NULL;																										// ...
		} else if (tok->value[0] != '+' && tok->value[0] != '-') {																// It's really the displacement?
			return NULL;																										// ...
		} else if (tok->value[0] == '+') {																						// Displacement = Add?
			disp = strtoimax(tok->value + 1, &endptr, 0);																		// Yes
		} else {
			disp = strtoimax(tok->value, &endptr, 0);																			// Nope, we need to parse the '-'
		}
		
		have_disp = 1;
	}
	
	if (parser_accept_noval(parser, TOK_TYPE_MUL)) {																			// Multiply?
		token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);															// Yes, now we should have a number
		char *endptr = NULL;
		
		if (tok == NULL) {
			return NULL;																										// ...
		} else if (tok->value[0] == '+' || tok->value[0] == '-') {																// It's really what we want?
			return NULL;
		} else if (tok->value[0] == '0' && tok->value[1] == 'b') {																// Binary?
			mul = strtoumax(tok->value + 2, &endptr, 2);																		// Yes, convert using base = 2
		} else {
			mul = strtoumax(tok->value, &endptr, 0);																			// Use auto-detection (base = 0)
		}
		
		have_mul = 1;
	}
	
	parser_expect_noval(parser, TOK_TYPE_CBRAC);																				// Expect the closing bracket
	
	node_t *ret = parser_new_node(cur, sizeof(address_node_t));																	// Create the node
	
	if (ret != NULL) {																											// Failed?
		ret->type = NODE_TYPE_ADDRESS;																							// No, so let's set the type
		ret->childs = val;																										// The val
		((address_node_t*)ret)->have_disp = have_disp;																			// And some informations about the displacement etc
		((address_node_t*)ret)->have_mul = have_mul;
		((address_node_t*)ret)->disp = disp;
		((address_node_t*)ret)->mul = mul;
	}
	
	return ret;
}

static int x86_find_mnemonic(char *name) {
	for (int i = 0; i < 158; i++) {
		if ((strlen(mnemonics[i]) == strlen(name)) && !strcasecmp(mnemonics[i], name)) {										// Found?
			return 1;																											// Yes :)
		}
	}
	
	return 0;
}

static node_t *x86_parse(parser_t *parser, node_t *cur) {
	if (parser == NULL) {																										// Null pointer check
		return NULL;	
	}
	
	token_t *tok = parser->position;																							// Save the initial token (for the errors/warnings)
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && x86_find_mnemonic(parser->position->value)) {						// Instruction?
		char *name = parser_expect_noval(parser, TOK_TYPE_IDENTIFIER)->value;													// Yes, save the name
		node_t *args = NULL;
		
		if (!parser_accept_noval(parser, TOK_TYPE_EOS)) {																		// We have any argument?
start:		if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																// Yes, identifier?
				node_t *arg = parser_parse_identifier(parser, NULL);
				
				if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																// Pointer?
					args = parser_parse_pointer(parser, args, arg);																// Yes
				} else if (args != NULL) {																						// Nope, first op?
					args->next = arg;																							// Nope
				} else {
					args = arg;																									// Yes
				}
			} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {															// Number?
				node_t *arg = parser_parse_number(parser, NULL);
				
				if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																// Pointer?
					args = parser_parse_pointer(parser, args, arg);																// Yes
				} else if (args != NULL) {																						// Nope, first op?
					args = arg;																									// Nope
				} else {
					args = arg;																									// Yes
				}
			} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {															// Registers?
				args = parser_parse_register(parser, args);
			} else if (parser_accept_noval(parser, TOK_TYPE_OBRAC)) {															// Address
				args = parser_parse_address(parser, args);
			}
			
			if (args == NULL) {
				printf("%s: %d: %d: invalid argument\n", tok->filename, tok->line, tok->col);									// Invalid, return -1 (error)
				
				if (args != NULL) {
					node_rewind_list(args);
					node_free_list(args);
				}
				
				return (node_t*)-1;
			}
			
			if (parser_accept_noval(parser, TOK_TYPE_COMMA)) {																	// More arguments?
				goto start;																										// Yes, go back to the start!
			}
			
			parser_expect_noval(parser, TOK_TYPE_EOS);																			// Now we MUST have a EOS
		}
		
		cur = parser_new_node(cur, sizeof(instruction_node_t));																	// Create the node
		
		if (cur == NULL) {
			if (args != NULL) {																									// Failed, return -1 (error)
				node_rewind_list(args);
				node_free_list(args);
			}
			
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_INSTRUCTION;																						// Set the type
		cur->childs = args;																										// Set the arguments
		((instruction_node_t*)cur)->name = name;																				// And the instruction name
		
		return cur;
	} else {
		return NULL;																											// Nope, this is a label (probably)
	}
}

static uint32_t count_childs(node_t *node) {
	uint32_t ret = 0;
	
	for (; node != NULL; ret++, node = node->next) ;																			// Just keep on going forward and counting!
	
	return ret;
}

static int get_optype(node_t *node) {
	int ret = 0;
	
	if (node->type == NODE_TYPE_IDENTIFIER) {																					// Identifier (symbol name without the brackets), for now, always 32 bits
		ret |= INSTR_ARG_IMMD;
	} else if (node->type == NODE_TYPE_NUMBER) {																				// Number
		uintmax_t val = ((number_node_t*)node)->value;																			// Get the value
		
		if (val <= UINT8_MAX) {																									// Byte
			ret |= INSTR_ARG_IMMB | INSTR_ARG_IMMW | INSTR_ARG_IMMD;
		} else if (val <= UINT16_MAX) {																							// Word
			ret |= INSTR_ARG_IMMW | INSTR_ARG_IMMD;
		} else {																												// DWord
			ret |= INSTR_ARG_IMMD;
		}
	} else if (node->type == NODE_TYPE_REGISTER) {																				// Register
		char *name = ((register_node_t*)node)->name;
		int found = 0;
		
		for (int i = 0; !found && i < 8; i++) {																					// First, check if it is a 32-bits register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_GREGD;
				found = 1;
			}
		}
		
		for (int i = 8; !found && i < 16; i++) {																				// Or a 16-bits register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_GREGW;
				found = 1;
			}
		}
		
		for (int i = 16; !found && i < 24; i++) {																				// Or a 8-bits register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_GREGB;
				found = 1;
			}
		}
	} else if (node->type == NODE_TYPE_ADDRESS) {																				// Identifier, number or register with the brackets
		if (node->childs->type == NODE_TYPE_REGISTER) {																			// Register?
			ret |= INSTR_ARG_MODRM_REG;
		} else {
			ret |= INSTR_ARG_MODRM_ADDR;																						// Identifier/number?
		}
		
		if (((address_node_t*)node)->have_disp) {																				// We have the displacement?
			ret |= INSTR_ARG_MODRM_DISP;
		}
		
		if (((address_node_t*)node)->have_mul) {																				// We have the scale?
			ret |= INSTR_ARG_MODRM_MULT;
		}
	} else if (node->type == NODE_TYPE_POINTER) {																				// Pointer
		ret |= INSTR_ARG_POINTER;
	}
	
	return ret;
}

static uint32_t get_opval(codegen_t *codegen, node_t *node, int size, int rel) {
	codegen_section_t *sect = codegen->current_section;
	int inc = rel ? sect->size + size : 0;
	uint32_t ret = 0;
	
	if (node->type == NODE_TYPE_IDENTIFIER) {																					// Identifier (symbol)?
		codegen_add_relocation(codegen, ((identifier_node_t*)node)->value, sect->name, size, sect->size, -inc);					// Yes, add relocation
	} else if (node->type == NODE_TYPE_NUMBER) {																				// Number?
		ret = (uint32_t)(((number_node_t*)node)->value) - inc;																	// Yes
	}
	
	return ret;
}

static int get_gregb(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, gregsb[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_gregw(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, gregsw[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_gregd(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, gregsd[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int x86_gen(codegen_t *codegen, node_t *node) {
	if (codegen == NULL || node == NULL) {																						// Null pointer check
		return 0;	
	} else if (node->type != NODE_TYPE_INSTRUCTION) {																			// Our node?
		return 0;																												// Nope
	}
	
	instruction_node_t *inod = (instruction_node_t*)node;
	
	uint32_t ops = count_childs(node->childs);
	uint32_t bestop1 = 0;
	uint32_t bestop2 = 0;
	int opcode = 0;
	int exists = 0;
	int found = 0;
	int instc = 0;
	int inst = 0;
	int op1 = 0;
	int op2 = 0;
	
	if (ops > 1) {																												// Too many args (> 2)?
		printf("too many operands to the instruction '%s'\n", inod->name);														// ...
		return -1;
	} else if (ops == 1) {
		op1 = get_optype(node->childs);																							// Get the optype from the first operand
	}
	
	for (; instc < 206; instc++) {																								// Let's try to find this instruction!
		if ((strlen(instructions[instc].name) != strlen(inod->name)) || strcasecmp(instructions[instc].name, inod->name)) {		// Same name?
			continue;																											// Nope
		} else {
			exists = 1;																											// Ok, at least this instr exists!
		}
		
		if (instructions[instc].args != ops) {																					// Same operand count?
			continue;																											// Nope
		} else if (ops == 1 && (op1 & instructions[instc].arg1) == 0) {															// Valid operands (we only need to check one)
			continue;
		} else if (ops == 2 && ((op1 & instructions[instc].arg1) == 0 || (op2 & instructions[instc].arg2) == 0)) {				// We need to check two operands
			continue;
		}
		
		found = 1;																												// Ok, we found it!
		
		if (ops == 0) {
			inst = instc;
		} else if (ops >= 1 && instructions[instc].arg1 - (op1 & instructions[instc].arg1) >= bestop1) {						// Best one?
			inst = instc;																										// Yes
			bestop1 = instructions[instc].arg1 - (op1 & instructions[instc].arg1);
			bestop2 = instructions[instc].arg2 - (op2 & instructions[instc].arg2);
		} else if (ops >= 2 && instructions[instc].arg2 - (op2 & instructions[instc].arg2) >= bestop2) {						// Best one?
			inst = instc;																										// Yes
			bestop1 = instructions[instc].arg1 - (op1 & instructions[instc].arg1);
			bestop2 = instructions[instc].arg2 - (op2 & instructions[instc].arg2);
		}
	}
	
	if (!found && exists) {																										// Invalid operands?
		printf("invalid operands to the instruction '%s'\n", inod->name);														// Yeah...
		return -1;
	} else if (!exists) {
		printf("invalid instruction '%s'\n", inod->name);																		// ... This wasn't supposed to happen
		return -1;
	}
	
	
	if (instructions[inst].alt_addr) {																							// Write the address size override?
		codegen_write_byte(codegen, 0x66);																						// Yes
	}
	
	if (instructions[inst].optype == INSTR_TYPE_OPREGB) {																		// Add the register to the opcode? (+rb)
		opcode = instructions[inst].opcode + get_gregb(((register_node_t*)node->childs)->name);
	} else if (instructions[inst].optype == INSTR_TYPE_OPREGW) {																// Add the register to the opcode? (+rw)
		opcode = instructions[inst].opcode + get_gregw(((register_node_t*)node->childs)->name);
	} else if (instructions[inst].optype == INSTR_TYPE_OPREGD) {																// Add the register to the opcode? (+rd)
		opcode = instructions[inst].opcode + get_gregd(((register_node_t*)node->childs)->name);
	} else {																													// Just the opcode
		opcode = instructions[inst].opcode;
	}
	
	codegen_write_byte(codegen, opcode);																						// Write the opcode
	
	if (instructions[inst].postop != -1) {																						// Two bytes opcode?
		codegen_write_byte(codegen, instructions[inst].postop);																	// Yes
	}
	
	if (instructions[inst].optype == INSTR_TYPE_RELB) {																			// 1 bytes after the opcode, but the value it's relative to the end of the instr, not absolute
		codegen_write_byte(codegen, get_opval(codegen, node->childs, 1, 1));
	} else if (instructions[inst].optype == INSTR_TYPE_RELW) {																	// 2 bytes after the opcode, but the value it's relative to the end of the instr, not absolute
		codegen_write_dword(codegen, get_opval(codegen, node->childs, 2, 1));
	} else if (instructions[inst].optype == INSTR_TYPE_RELD) {																	// 4 bytes after the opcode, but the value it's relative to the end of the instr, not absolute
		codegen_write_dword(codegen, get_opval(codegen, node->childs, 4, 1));
	} else if (instructions[inst].optype == INSTR_TYPE_BYTE) {																	// 1 byte after the opcode
		codegen_write_byte(codegen, (uint8_t)get_opval(codegen, node->childs, 1, 0));
	} else if (instructions[inst].optype == INSTR_TYPE_WORD) {																	// 2 byte after the opcode
		codegen_write_word(codegen, (uint16_t)get_opval(codegen, node->childs, 2, 0));
	} else if (instructions[inst].optype == INSTR_TYPE_DWORD) {																	// 4 byte after the opcode
		codegen_write_dword(codegen, get_opval(codegen, node->childs, 4, 0));
	} else if (instructions[inst].optype == INSTR_TYPE_MODRM) {																	// ModR/M
		uint8_t ext = (uint8_t)(instructions[inst].extension != -1 ? instructions[inst].extension << 3 : 0);					// First, get the "extension" that some instructions use
		
		if (op1 == INSTR_ARG_MODRM_ADDR) {																						// Just a number/identifier in the brackets?
			codegen_write_byte(codegen, ext | 0x05);																			// Yes, write the modr/m byte
			codegen_write_dword(codegen, get_opval(codegen, node->childs->childs, 4, 0));										// And the value
		} else if (op1 == INSTR_ARG_MODRM_REG) {																				// 32-bits register?
			if (!strcasecmp(((register_node_t*)node->childs->childs)->name, "ebp")) {											// Yes, write the modr/m byte, it would be ext | 0x05
				codegen_write_word(codegen, 0x4500);																			// Yes, fix that
			} else {
				codegen_write_byte(codegen, ext | get_gregd(((register_node_t*)node->childs->childs)->name));					// Nope!
			}
			
			if (!strcasecmp(((register_node_t*)node->childs->childs)->name, "esp")) {											// modr/m = ext | 0x04?
				codegen_write_byte(codegen, 0x24);																				// Yes, fix!
			}
		} else if (op1 == (INSTR_ARG_MODRM_REG | INSTR_ARG_MODRM_DISP)) {														// 32-bits register + displacement?
			uint32_t disp = ((address_node_t*)node->childs)->disp;																// Get the displacement
			uint8_t dshft = 0x80;
			
			if (((int32_t)disp <= INT8_MAX) || ((int32_t)disp >= INT8_MIN)) {													// 8-bits disp?
				dshft = 0x40;																									// Yeah
			}
			
			if (!strcasecmp(((register_node_t*)node->childs->childs)->name, "ebp")) {											// Yes, write the modr/m byte, it would be ext | 0x05
				codegen_write_word(codegen, 0x4500 | dshft);																	// Yes, fix that
			} else {
				codegen_write_byte(codegen, ext | get_gregd(((register_node_t*)node->childs->childs)->name) | dshft);			// Nope!
			}
			
			if (!strcasecmp(((register_node_t*)node->childs->childs)->name, "esp")) {											// modr/m = ext | 0x04?
				codegen_write_byte(codegen, 0x24);																				// Yes, fix!
			}
			
			if (dshft == 0x40) {																								// 8-bits disp?
				codegen_write_byte(codegen, (uint8_t)disp);																		// Yes
			} else {
				codegen_write_dword(codegen, disp);																				// 32-bits disp
			}
		} else {
			printf("invalid operands to the instruction '%s'\n", inod->name);													// Unsupported for now...
			return -1;
		}
	} else if (instructions[inst].optype == INSTR_TYPE_POINTER) {																// Pointer (16:32)
		codegen_write_dword(codegen, get_opval(codegen, node->childs->childs->next, 4, 0));										// Write the right part
		codegen_write_word(codegen, (uint16_t)get_opval(codegen, node->childs->childs, 2, 0));									// Write the left part
	}
	
	return 1;
}

static uint8_t x86_ttype(char *ident) {
	if (ident != NULL && x86_find_register(ident)) {																			// Register?
		return TOK_TYPE_REGISTER;																								// Yes!
	} else {
		return TOK_TYPE_IDENTIFIER;																								// No, so it's an normal identifier
	}
}

static void x86_tfree(token_t *token) {
	if (token != NULL && token->type == TOK_TYPE_REGISTER) {																	// Register?
		free(token->value);																										// Yes, free it!
	}
}

static void x86_tprint(token_t *token) {
	if (token == NULL) {																										// Null pointer check
		return;
	} else if (token->type == TOK_TYPE_COMMA) {																					// Comma?
		printf("Comma\n");																										// Yes, print it
	} else if (token->type == TOK_TYPE_MUL) {																					// Multiply?
		printf("Multiply\n");																									// Yes, print it
	} else if (token->type == TOK_TYPE_OBRAC) {																					// Opening Bracket?
		printf("Opening Bracket\n");																							// Yes, print it
	} else if (token->type == TOK_TYPE_CBRAC) {																					// Closing Bracket?
		printf("Closing Bracket\n");																							// Yes, print it
	} else if (token->type == TOK_TYPE_REGISTER) {																				// Register?
		printf("Register: %s\n", token->value);																					// Yes, print it
	}
}

REGISTER_ARCH(x86, "x86", x86_help, x86_option, x86_lex, x86_parse, x86_gen, x86_ttype, x86_tfree, x86_tprint);					// Register this architecture
