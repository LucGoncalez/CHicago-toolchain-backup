// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 17:37 BRT
// Last edited on December 02 of 2018, at 20:10 BRT

#include <arch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <x86.h>

static char *registers[30] = {
	"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
	"ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
	"ah", "bh", "ch", "dh",
	"al", "bl", "cl", "dl",
	"cs", "ds", "es", "fs", "gs", "ss"
};

static void lexer_consume(lexer_t *lexer) {
	if (lexer->pos < lexer->length) {																			// We can increase the position?
		if (lexer->text[lexer->pos] == '\n') {																	// Yes, new line?
			lexer->line++;																						// Yes! Increase the line count
			lexer->col = 0;																						// And set the current column to zero
		} else {
			lexer->col++;																						// No, just increase the current column
		}

		lexer->pos++;																							// Increase the position
	}
}

static token_t *lexer_new_token(token_t *list, token_t *cur) {
	if (cur == NULL) {																							// We're at the start of the list?
		return list;																							// Yes :)
	}
	
	cur->next = calloc(1, sizeof(token_t));																		// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																							// Failed...
	}
	
	cur->next->prev = cur;																						// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static int x86_find_register(char *name) {
	for (int i = 0; i < 30; i++) {
		if ((strlen(registers[i]) == strlen(name)) && !strcmp(registers[i], name)) {							// Found?
			return 1;																							// Yes :)
		}
	}
	
	return 0;
}

static token_t *x86_lex(lexer_t *lexer, token_t *list, token_t *cur) {
	if (lexer == NULL || list == NULL) {																		// Null pointer checks
		return NULL;
	} else if (lexer->text[lexer->pos] == ',') {																// Comma?
		cur = lexer_new_token(list, cur);																		// Yes, create a new token at the end of the list
		
		if (cur == NULL) {
			return NULL;																						// Failed...
		}
		
		cur->type = TOK_TYPE_COMMA;																				// Set the type
		cur->filename = lexer->filename;																		// Set the filename
		cur->line = lexer->line;																				// Set the line
		cur->col = lexer->col;																					// And the column
		
		lexer_consume(lexer);																					// Consume the character
		
		return cur;
	} else {
		return NULL;																							// ...
	}
}

static uint8_t x86_ttype(char *ident) {
	if (ident != NULL && x86_find_register(ident)) {															// Register?
		return TOK_TYPE_REGISTER;																				// Yes!
	} else {
		return TOK_TYPE_IDENTIFIER;																				// No, so it's an normal identifier
	}
}

static void x86_tfree(token_t *token) {
	if (token != NULL && token->type == TOK_TYPE_REGISTER) {													// Register?
		free(token->value);																						// Yes, free it!
	}
}

static void x86_tprint(token_t *token) {
	if (token == NULL) {																						// Null pointer check
		return;
	} else if (token->type == TOK_TYPE_COMMA) {																	// Comma?
		printf("Comma\n");																						// Yes, print it
	} else if (token->type == TOK_TYPE_REGISTER) {																// Register?
		printf("Register: %s\n", token->value);																	// Yes, print it
	}
}

REGISTER_ARCH(x86, "x86", x86_lex, x86_ttype, x86_tfree, x86_tprint);
