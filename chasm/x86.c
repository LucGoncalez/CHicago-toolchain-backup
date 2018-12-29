// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 17:37 BRT
// Last edited on December 28 of 2018, at 17:15 BRT

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

static char *mnemonics[1] = {
	"aaa"
};

struct {
	char *name;
	int opcode;
	int optype;
	int extension;
	uint32_t arg1;
	uint32_t arg2;
} instructions[1] = {
	{ "aaa", 0x37, INSTR_TYPE_NONE, 0x00, INSTR_ARG_NONE, INSTR_ARG_NONE }
};

static void lexer_consume(lexer_t *lexer) {
	if (lexer->pos < lexer->length) {																					// We can increase the position?
		if (lexer->text[lexer->pos] == '\n') {																			// Yes, new line?
			lexer->line++;																								// Yes! Increase the line count
			lexer->col = 0;																								// And set the current column to zero
		} else {
			lexer->col++;																								// No, just increase the current column
		}

		lexer->pos++;																									// Increase the position
	}
}

static token_t *lexer_new_token(token_t *list, token_t *cur) {
	if (cur == NULL) {																									// We're at the start of the list?
		return list;																									// Yes :)
	}
	
	cur->next = calloc(1, sizeof(token_t));																				// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																									// Failed...
	}
	
	cur->next->prev = cur;																								// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static int x86_find_register(char *name) {
	for (int i = 0; i < 30; i++) {
		if ((strlen(registers[i]) == strlen(name)) && !strcasecmp(registers[i], name)) {								// Found?
			return 1;																									// Yes :)
		}
	}
	
	return 0;
}

static token_t *x86_lex(lexer_t *lexer, token_t *list, token_t *cur) {
	if (lexer == NULL || list == NULL) {																				// Null pointer checks
		return NULL;
	} else if (lexer->text[lexer->pos] == ',' || lexer->text[lexer->pos] == '*' || lexer->text[lexer->pos] == '[' ||
			   lexer->text[lexer->pos] == ']') {																		// Single character token?
		cur = lexer_new_token(list, cur);																				// Yes, create a new token at the end of the list
		
		if (cur == NULL) {
			return NULL;																								// Failed...
		}
		
		cur->type = lexer->text[lexer->pos] == ',' ? TOK_TYPE_COMMA : (lexer->text[lexer->pos] == '*' ? TOK_TYPE_MUL :
					(lexer->text[lexer->pos] == '[' ? TOK_TYPE_OBRAC : TOK_TYPE_CBRAC));								// Set the type
		cur->filename = lexer->filename;																				// Set the filename
		cur->line = lexer->line;																						// Set the line
		cur->col = lexer->col;																							// And the column
		
		lexer_consume(lexer);																							// Consume the character
		
		return cur;
	} else {
		return NULL;																									// ...
	}
}

static node_t *parser_new_node(node_t *cur, size_t size) {
	if (cur == NULL) {																									// We're at the start of the list?
		return (node_t*)calloc(1, size);																				// Yes :)
	}
	
	cur->next = calloc(1, size);																						// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																									// Failed...
	}
	
	cur->next->prev = cur;																								// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static node_t *parser_parse_register(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_REGISTER);														// Get our token
	
	if (tok == NULL) {
		return NULL;																									// Failed...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(register_node_t));														// Create the node
	
	if (ret != NULL) {																									// Failed?
		ret->type = NODE_TYPE_REGISTER;																					// No, so let's set the type
		((register_node_t*)ret)->name = tok->value;																		// And the value!
	}
	
	return ret;
}

static node_t *parser_parse_address(parser_t *parser, node_t *cur) {
	node_t *val = NULL;																									// Let's get the val
	int have_disp = 0;
	int have_mul = 0;
	int32_t disp = 0;
	uint32_t mul = 0;
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																// Identifier?
		val = parser_parse_identifier(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {															// Number?
		val = parser_parse_number(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {															// Register?
		val = parser_parse_register(parser, NULL);
	} else {
		return NULL;																									// ...
	}
	
	if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																	// Displacement?
		token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);													// Yes, now we should have a number
		char *endptr = NULL;
		
		if (tok == NULL) {
			return NULL;																								// ...
		} else if (tok->value[0] != '+' && tok->value[0] != '-') {														// It's really the displacement?
			return NULL;																								// ...
		} else if (tok->value[0] == '+') {																				// Displacement = Add?
			disp = strtoimax(tok->value + 1, &endptr, 0);																// Yes
		} else {
			disp = strtoimax(tok->value, &endptr, 0);																	// Nope, we need to parse the '-'
		}
		
		have_disp = 1;
	}
	
	if (parser_accept_noval(parser, TOK_TYPE_MUL)) {																	// Multiply?
		token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);													// Yes, now we should have a number
		char *endptr = NULL;
		
		if (tok == NULL) {
			return NULL;																								// ...
		} else if (tok->value[0] == '+' || tok->value[0] == '-') {														// It's really what we want?
			return NULL;
		} else if (tok->value[0] == '0' && tok->value[1] == 'b') {														// Binary?
			mul = strtoumax(tok->value + 2, &endptr, 2);																// Yes, convert using base = 2
		} else {
			mul = strtoumax(tok->value, &endptr, 0);																	// Use auto-detection (base = 0)
		}
		
		have_mul = 1;
	}
	
	parser_expect_noval(parser, TOK_TYPE_CBRAC);																		// Expect the closing bracket
	
	node_t *ret = parser_new_node(cur, sizeof(address_node_t));															// Create the node
	
	if (ret != NULL) {																									// Failed?
		ret->type = NODE_TYPE_ADDRESS;																					// No, so let's set the type
		ret->childs = val;																								// The val
		((address_node_t*)ret)->have_disp = have_disp;																	// And some informations about the displacement etc
		((address_node_t*)ret)->have_mul = have_mul;
		((address_node_t*)ret)->disp = disp;
		((address_node_t*)ret)->mul = mul;
	}
	
	return ret;
}

static int x86_find_mnemonic(char *name) {
	for (int i = 0; i < 1; i++) {
		if ((strlen(mnemonics[i]) == strlen(name)) && !strcasecmp(mnemonics[i], name)) {								// Found?
			return 1;																									// Yes :)
		}
	}
	
	return 0;
}

static node_t *x86_parse(parser_t *parser, node_t *cur) {
	if (parser == NULL || cur == NULL) {																				// Null pointer check
		return NULL;	
	}
	
	token_t *tok = parser->position;																					// Save the initial token (for the errors/warnings)
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && x86_find_mnemonic(parser->position->value)) {				// Instruction?
		char *name = parser_expect_noval(parser, TOK_TYPE_IDENTIFIER)->value;											// Yes, save the name
		node_t *args = NULL;
		
		if (!parser_accept_noval(parser, TOK_TYPE_EOS)) {																// We have any argument?
start:		if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {														// Yes, identifier?
				args = parser_parse_identifier(parser, args);
			} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {													// Number?
				args = parser_parse_number(parser, args);
			} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {													// Registers?
				args = parser_parse_register(parser, args);
			} else if (parser_accept_noval(parser, TOK_TYPE_OBRAC)) {													// Address
				args = parser_parse_address(parser, args);
			} else {
				printf("%s: %d: %d: invalid argument\n", tok->filename, tok->line, tok->col);							// Invalid, return -1 (error)
				
				if (args != NULL) {
					node_rewind_list(args);
					node_free_list(args);
				}
				
				return (node_t*)-1;
			}
			
			if (parser_accept_noval(parser, TOK_TYPE_COMMA)) {															// More arguments?
				goto start;																								// Yes, go back to the start!
			}
			
			parser_expect_noval(parser, TOK_TYPE_EOS);																	// Now we MUST have a EOS
		}
		
		cur = parser_new_node(cur, sizeof(instruction_node_t));															// Create the node
		
		if (cur == NULL) {
			if (args != NULL) {																							// Failed, return -1 (error)
				node_rewind_list(args);
				node_free_list(args);
			}
			
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_INSTRUCTION;																				// Set the type
		cur->childs = args;																								// Set the arguments
		((instruction_node_t*)cur)->name = name;																		// And the instruction name
		
		return cur;
	} else {
		return NULL;																									// Nope, this is a label (probably)
	}
}

static uint8_t x86_ttype(char *ident) {
	if (ident != NULL && x86_find_register(ident)) {																	// Register?
		return TOK_TYPE_REGISTER;																						// Yes!
	} else {
		return TOK_TYPE_IDENTIFIER;																						// No, so it's an normal identifier
	}
}

static void x86_tfree(token_t *token) {
	if (token != NULL && token->type == TOK_TYPE_REGISTER) {															// Register?
		free(token->value);																								// Yes, free it!
	}
}

static void x86_tprint(token_t *token) {
	if (token == NULL) {																								// Null pointer check
		return;
	} else if (token->type == TOK_TYPE_COMMA) {																			// Comma?
		printf("Comma\n");																								// Yes, print it
	} else if (token->type == TOK_TYPE_MUL) {																			// Multiply?
		printf("Multiply\n");																							// Yes, print it
	} else if (token->type == TOK_TYPE_OBRAC) {																			// Opening Bracket?
		printf("Opening Bracket\n");																					// Yes, print it
	} else if (token->type == TOK_TYPE_CBRAC) {																			// Closing Bracket?
		printf("Closing Bracket\n");																					// Yes, print it
	} else if (token->type == TOK_TYPE_REGISTER) {																		// Register?
		printf("Register: %s\n", token->value);																			// Yes, print it
	}
}

REGISTER_ARCH(x86, "x86", x86_lex, x86_parse, x86_ttype, x86_tfree, x86_tprint);
