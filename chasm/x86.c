// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 17:37 BRT
// Last edited on December 29 of 2018, at 23:35 BRT

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

static char *mnemonics[43] = {
	"aaa", "aad", "aam", "aas", "cbw", "cwde", "clc", "cld", "cli", "clts",
	"cmc", "cwd", "cdq", "daa", "das", "hlt", "iret", "iretw", "iretd",
	"lahf", "leave", "nop", "popa", "popaw", "popad", "popf", "popfw",
	"popfd", "pusha", "pushaw", "pushad", "pushf", "pushfw", "pushfd",
	"ret", "retf", "retfw", "retfd", "sahf", "stc", "std", "sti", "wait"
};

struct {
	char *name;
	int opcode;
	int optype;
	int extension;
	uint32_t args;
	uint32_t arg1;
	uint32_t arg2;
} instructions[43] = {
	{ "aaa", 0x37, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aad", 0x0AD5, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aam", 0x0AD4, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aas", 0x3F, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cbw", 0x9866, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cwde", 0x98, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clc", 0xF8, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cld", 0xFC, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cli", 0xFA, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clts", 0x060F, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cmc", 0xF5, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cwd", 0x9966, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cdq", 0x99, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "daa", 0x27, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "das", 0x2F, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "hlt", 0xF4, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "iret", 0xCF, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "iretw", 0xCF66, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "iretd", 0xCF, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lahf", 0x9F, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "leave", 0xC9, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "nop", 0x90, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "popa", 0x61, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popaw", 0x6166, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popad", 0x61, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "popf", 0x9D, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popfw", 0x9D66, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popfd", 0x9D, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pusha", 0x60, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushaw", 0x6066, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushad", 0x60, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pushf", 0x9C, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushfw", 0x9C66, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushfd", 0x9C, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ret", 0xC3, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retf", 0xCB, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfw", 0xCB66, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfd", 0xCB, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sahf", 0x9E, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "stc", 0xF9, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "std", 0xFD, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sti", 0xFB, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "wait", 0x9B, INSTR_TYPE_NONE, 0, 0, INSTR_ARG_NONE, INSTR_ARG_NONE },
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

static node_t *parser_parse_address(parser_t *parser, node_t *cur) {
	node_t *val = NULL;																											// Let's get the val
	int have_disp = 0;
	int have_mul = 0;
	int32_t disp = 0;
	uint32_t mul = 0;
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																		// Identifier?
		val = parser_parse_identifier(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																	// Number?
		val = parser_parse_number(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {																	// Register?
		val = parser_parse_register(parser, NULL);
	} else {
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
	for (int i = 0; i < 43; i++) {
		if ((strlen(mnemonics[i]) == strlen(name)) && !strcasecmp(mnemonics[i], name)) {										// Found?
			return 1;																											// Yes :)
		}
	}
	
	return 0;
}

static node_t *x86_parse(parser_t *parser, node_t *cur) {
	if (parser == NULL || cur == NULL) {																						// Null pointer check
		return NULL;	
	}
	
	token_t *tok = parser->position;																							// Save the initial token (for the errors/warnings)
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && x86_find_mnemonic(parser->position->value)) {						// Instruction?
		char *name = parser_expect_noval(parser, TOK_TYPE_IDENTIFIER)->value;													// Yes, save the name
		node_t *args = NULL;
		
		if (!parser_accept_noval(parser, TOK_TYPE_EOS)) {																		// We have any argument?
start:		if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																// Yes, identifier?
				args = parser_parse_identifier(parser, args);
			} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {															// Number?
				args = parser_parse_number(parser, args);
			} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {															// Registers?
				args = parser_parse_register(parser, args);
			} else if (parser_accept_noval(parser, TOK_TYPE_OBRAC)) {															// Address
				args = parser_parse_address(parser, args);
			} else {
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
	if (node == NULL) {																											// Null pointer check
		return 0;
	}
	
	uint32_t ret = 0;
	
	for (; node != NULL; ret++, node = node->next) ;																			// Just keep on going forward and counting!
	
	return ret;
}

static void write_opcode(codegen_t *codegen, int opcode) {
	if (opcode <= UINT8_MAX) {																									// 1 byte
		codegen_write_byte(codegen, (uint8_t)opcode);
	} else if (opcode <= UINT16_MAX) {																							// 2 bytes
		codegen_write_word(codegen, (uint16_t)opcode);
	} else {																													// 4 bytes
		codegen_write_dword(codegen, (uint32_t)opcode);
	}
}

static int x86_gen(codegen_t *codegen, node_t *node) {
	if (codegen == NULL || node == NULL) {																						// Null pointer check
		return 0;	
	} else if (node->type != NODE_TYPE_INSTRUCTION) {																			// Our node?
		return 0;																												// Nope
	}
	
	instruction_node_t *inod = (instruction_node_t*)node;
	uint32_t ops = count_childs(node->childs);
	int exists = 0;
	int found = 0;
	int inst = 0;
	int op1 = 0;
	int op2 = 0;
	
	if (ops > 0) {																												// Too many args (> 2)?
		printf("too many operands to the instruction '%s'\n", inod->name);														// ...
		return -1;
	}
	
	for (; inst < 43; inst++) {																									// Let's try to find this instruction!
		if ((strlen(instructions[inst].name) != strlen(inod->name)) || strcasecmp(instructions[inst].name, inod->name)) {		// Same name?
			continue;																											// Nope
		} else {
			exists = 1;																											// Ok, at least this instr exists!
		}
		
		if (instructions[inst].args != ops) {																					// Same operand count?
			continue;																											// Nope
		} else if (ops == 1 && (op1 & instructions[inst].arg1) == 0) {															// Valid operands (we only need to check one)
			continue;
		} else if (ops == 2 && ((op1 & instructions[inst].arg1) == 0 || (op2 & instructions[inst].arg2) == 0)) {				// We need to check two operands
			continue;
		}
		
		found = 1;																												// Ok, we found it!
		break;
	}
	
	if (!found && exists) {																										// Invalid operands?
		printf("invalid operands to the instruction '%s'\n", inod->name);														// Yeah...
		return -1;
	} else if (!exists) {
		printf("invalid instruction '%s'\n", inod->name);																		// ... This wasn't supposed to happen
		return -1;
	} else if (instructions[inst].optype == INSTR_TYPE_NONE) {																	// No operands!
		write_opcode(codegen, instructions[inst].opcode);																		// Just write the opcode
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
