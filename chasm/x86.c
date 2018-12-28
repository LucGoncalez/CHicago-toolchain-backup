// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 17:37 BRT
// Last edited on December 28 of 2018, at 10:04 BRT

#include <arch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <x86.h>

static char *registers[] = {
	"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
	"ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
	"ah", "bh", "ch", "dh",
	"al", "bl", "cl", "dl",
	"cs", "ds", "es", "fs", "gs", "ss",
	NULL
};

static char *mnemonics[] = {
	"aaa", "aad", "aam", "aas", "adc", "add", "and", "arpl", "bound", "bsf",
	"bsr", "bt", "btc", "btr", "bts", "call", "cbw", "cwde", "clc", "cld",
	"cli", "clts", "cmc", "cmp", "cmps", "cmpsb", "cmpsw", "cmpsd", "cwd",
	"cdq", "daa", "das", "dec", "div", "enter", "hlt", "idiv", "imul", "in",
	"inc", "ins", "insb", "insw", "insd", "int", "into", "iret", "iretd",
	"jcc", "jmp", "lahf", "lar", "lea", "leave", "lgdt", "lidt", "lgs", "lss",
	"lds", "les", "lfs", "lldt", "lmsw", "lock", "lods", "lodsb", "lodsw",
	"lodsd", "loop", "loope", "loopz", "loopne", "loopnz", "lsl", "ltr",
	"mov", "movs", "movsb", "movw", "movsd", "movsx", "movzx", "mul", "neg",
	"nop", "not", "or", "out", "outs", "outsb", "outsw", "outsd", "pop",
	"popa", "popf", "push", "pusha", "pushf", "rcl", "rcr", "rol", "ror",
	"ret", "sahf", "sal", "sar", "shl", "shr", "sbb", "scas", "scasb",
	"scasw", "scasd", "seta", "setae", "setb", "setbe", "setc", "sete",
	"setg", "setge", "setl", "setle", "setna", "setnae", "setnb", "setnbe",
	"setnc", "setne", "setng", "setngr", "setnl", "setnle", "setno", "setnp",
	"setns", "setnz", "seto", "setp", "setpe", "setpo", "sets", "setz",
	"sgdt", "sidt", "shld", "shrd", "sldt", "smsw", "stc", "std", "sti",
	"stos", "stosb", "stosw", "stosw", "stosd", "str", "sub", "test", "verr",
	"verw", "wait", "xchg", "xlat", "xlatb", "xor",
	NULL
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
	for (int i = 0; registers[i] != NULL; i++) {
		if ((strlen(registers[i]) == strlen(name)) && !strcasecmp(registers[i], name)) {								// Found?
			return 1;																									// Yes :)
		}
	}
	
	return 0;
}

static token_t *x86_lex(lexer_t *lexer, token_t *list, token_t *cur) {
	if (lexer == NULL || list == NULL) {																				// Null pointer checks
		return NULL;
	} else if (lexer->text[lexer->pos] == ',' || lexer->text[lexer->pos] == '+' || lexer->text[lexer->pos] == '-' ||
			   lexer->text[lexer->pos] == '*' || lexer->text[lexer->pos] == '[' || lexer->text[lexer->pos] == ']') {	// Single character token?
		cur = lexer_new_token(list, cur);																				// Yes, create a new token at the end of the list
		
		if (cur == NULL) {
			return NULL;																								// Failed...
		}
		
		cur->type = lexer->text[lexer->pos] == ',' ? TOK_TYPE_COMMA : (lexer->text[lexer->pos] == '+' ? TOK_TYPE_ADD :
					(lexer->text[lexer->pos] == '-' ? TOK_TYPE_SUB : (lexer->text[lexer->pos] == '*' ? TOK_TYPE_MUL :
					(lexer->text[lexer->pos] == '[' ? TOK_TYPE_OBRAC : TOK_TYPE_CBRAC))));								// Set the type
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

static int x86_find_mnemonic(char *name) {
	for (int i = 0; mnemonics[i] != NULL; i++) {
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
	} else if (token->type == TOK_TYPE_ADD) {																			// Add?
		printf("Add\n");																								// Yes, print it
	} else if (token->type == TOK_TYPE_SUB) {																			// Subtract?
		printf("Subtract\n");																							// Yes, print it
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
