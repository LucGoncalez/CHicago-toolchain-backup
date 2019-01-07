// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 14:38 BRT
// Last edited on January 07 of 2019, at 15:27 BRT

#include <arch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static char *directives[8] = {
	"section",
	"global", "extern",
	"db", "dw", "dd", "dq", "dt"
};

void token_free_list(token_t *token) {
	if (token != NULL) {																						// Null pointer check
		while (token != NULL) {																					// Ok, let's free the list!
			token_t *old = token;																				// Save the old one
			token = old->next;																					// Set the new one
			token_free(old);																					// Free the old one
		}
	}
}

token_t *token_rewind_list(token_t *token) {
	if (token != NULL) {																						// Null pointer check
		while (token->prev != NULL) {																			// First entry?
			token = token->prev;																				// No, go one entry back!
		}
	}
	
	return token;
}

void token_free(token_t *token) {
	if (token != NULL) {																						// Null pointer check
		if (token->type != TOK_TYPE_COLON && token->type <= TOK_TYPE_COLON) {									// Ok, free the value?
			free(token->value);																					// Yes!
		} else if (token->type > TOK_TYPE_COLON) {																// Arch token?
			arch_tfree(token);																					// Yes
		}
		
		free(token);																							// Free the token struct!
	}
}

void token_print(token_t *token) {
	if (token == NULL) {																						// Null pointer check
		return;
	}
	
	printf("%s: Line %d, Column %d: ", token->filename, token->line, token->col);								// Print the filename, the line and the column of this token
	
	if (token->type == TOK_TYPE_IDENTIFIER) {																	// Identifier?
		printf("Identifier: %s\n", token->value);																// Yes, print it
	} else if (token->type == TOK_TYPE_DIRECTIVE) {																// Directive?
		printf("Directive: %s\n", token->value);																// Yes, print it
	} else if (token->type == TOK_TYPE_NUMBER) {																// Number?
		printf("Number: %s\n", token->value);																	// Yes, print it
	} else if (token->type == TOK_TYPE_STRING) {																// String?
		printf("String: %s\n", token->value);																	// Yes, print it
	} else if (token->type == TOK_TYPE_EOS) {																	// New line?
		printf("New Line\n");																					// Yes, print it
	} else if (token->type == TOK_TYPE_COMMA) {																	// Comma?
		printf("Comma\n");																						// Yes
	} else if (token->type == TOK_TYPE_COLON) {																	// Colon?
		printf("Colon\n");																						// Yes
	} else {
		arch_tprint(token);																						// Arch token
	}
}

lexer_t *lexer_new(char *filename, char *text) {
	if (filename == NULL || text == NULL) {																		// Null pointer check
		return NULL;
	}
	
	lexer_t *lexer = calloc(1, sizeof(lexer_t));																// Alloc space
	
	if (lexer != NULL) {																						// Failed?
		lexer->filename = strdup(filename);																		// No! Set the filename
		lexer->text = strdup(text);																				// Set the input
		lexer->length = strlen(text);																			// And set the input length
	}
	
	return lexer;
}

void lexer_free(lexer_t *lexer) {
	if (lexer != NULL) {																						// Check if our argument is valid
		free(lexer->text);																						// Free everything!
		free(lexer->filename);
		free(lexer);
	}
}

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

static int lexer_is_bin(char c) {
	return c == '0' || c == '1';																				// Binary number is 0 or 1
}

static int lexer_is_hex(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||													// Hexadecimal number is any number from 0 to 9 or from A to F
		   (c >= 'A' && c <= 'F');
}

static int lexer_is_dec(char c) {
	return c >= '0' && c <= '9';																				// Decimal number is any number from 0 to 9
}

static int lexer_is_ident(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '.';							// Identifier is any ASCII character or a '_'
}

static int lexer_is_alnum(char c) {
	return lexer_is_dec(c) || lexer_is_ident(c);																// Alphanumeric would be any unicode valid character or number, but for us is any identifier character or number
}

static void lexer_consume_whitespaces(lexer_t *lexer) {
	while (lexer->pos < lexer->length && (lexer->text[lexer->pos] == ' ' ||
										  lexer->text[lexer->pos] == '\t' ||
										  lexer->text[lexer->pos] == '\r' ||
										  lexer->text[lexer->pos] <= 1)) {										// Whitespace = Space, Tab, Carriage Return (the <= 1 is because of a bug)
		lexer_consume(lexer);
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

static int lexer_find_directive(char *name) {
	for (int i = 0; i < 8; i++) {
		if ((strlen(directives[i]) == strlen(name)) && !strcasecmp(directives[i], name)) {						// Found?
			return 1;																							// Yes :)
		}
	}
	
	return 0;
}

token_t *lexer_lex(lexer_t *lexer) {
	if (lexer == NULL || lexer->text == NULL) {																	// Null pointer checks
		return NULL;
	}
	
	token_t *list = calloc(1, sizeof(token_t));																	// Alloc space for the token list
	token_t *cur = NULL;																						// Set the current token to the start of the list
	
	if (list == NULL) {
		return NULL;																							// Failed!
	}
	
	lexer->line = 1;																							// Set the line to 1
	lexer->col = 0;																								// The column to 0
	lexer->pos = 0;																								// And the position to 0
	
	while (lexer->pos < lexer->length) {																		// Let's go!
		if (lexer->text[lexer->pos] == ' ' || lexer->text[lexer->pos] == '\t' ||
			lexer->text[lexer->pos] == '\r' || lexer->text[lexer->pos] <= 1) {									// Consume any whitespaces
			lexer_consume_whitespaces(lexer);
		} else if (lexer->text[lexer->pos] == ';') {															// Comment?
			while (lexer->pos < lexer->length && lexer->text[lexer->pos] != '\n') {								// Yes, consume until we find the next line!
				lexer->pos++;
			}
		} else if (lexer_is_ident(lexer->text[lexer->pos])) {													// Identifier?
			cur = lexer_new_token(list, cur);																	// Yes, create a new token at the end of the list
			
			if (cur == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			cur->filename = lexer->filename;																	// Set the filename
			cur->line = lexer->line;																			// Set the line
			cur->col = lexer->col;																				// And the column
			
			int len = 0;																						// Let's get the length of this identifier!
			
			for (; lexer->pos + len < lexer->length && lexer_is_alnum(lexer->text[lexer->pos + len]);
				   len++) ;
			
			cur->value = malloc(len + 1);																		// Alloc space for copying the identifier
			
			if (cur->value == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			strncpy(cur->value, &lexer->text[lexer->pos], len);													// Copy it!
			cur->value[len] = '\0';																				// Add the NUL terminator at the end
			
			if (lexer_find_directive(cur->value)) {																// Directive?
				cur->type = TOK_TYPE_DIRECTIVE;																	// Yes!
			} else {
				cur->type = arch_ttype(cur->value);																// No, use the arch_ttype to find the type
			}
			
			for (int i = 0; i < len; i++) {																		// Consume len bytes
				lexer_consume(lexer);
			}
		} else if (lexer_is_dec(lexer->text[lexer->pos]) ||
				   lexer->text[lexer->pos] == '-' || lexer->text[lexer->pos] == '+') {							// Number?
			cur = lexer_new_token(list, cur);																	// Yes, create a new token at the end of the list
			
			if (cur == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			cur->type = TOK_TYPE_NUMBER;																		// Set the type
			cur->filename = lexer->filename;																	// Set the filename
			cur->line = lexer->line;																			// Set the line
			cur->col = lexer->col;																				// And the column
			
			int len = 0;																						// Let's get the length of the number!
			int sign = -1;
			
			if (lexer->text[lexer->pos] == '-' || lexer->text[lexer->pos] == '+') {								// Force negative/positive number sign?
				sign = lexer->pos++;																			// Yes
			}
			
			lexer_consume_whitespaces(lexer);																	// Consume the whitespaces
			
			if ((lexer->pos + len + 1 < lexer->length) && lexer->text[lexer->pos + len] == '0' &&
				lexer->text[lexer->pos + len + 1] == 'b') {														// Binary number
				len += 2;																						// Jump the 0b prefix
				
				for (; lexer->pos + len < lexer->length && lexer_is_bin(lexer->text[lexer->pos + len]);
					   len++) ;
			} else if ((lexer->pos + len + 1 < lexer->length) && lexer->text[lexer->pos + len] == '0' &&
					   lexer->text[lexer->pos + len + 1] == 'x') {												// Hexadecimal number
				len += 2;																						// Jump the 0x prefix
				
				for (; lexer->pos + len < lexer->length && lexer_is_hex(lexer->text[lexer->pos + len]);
					   len++) ;
			} else {																							// Decimal number
				for (; lexer->pos + len < lexer->length && lexer_is_dec(lexer->text[lexer->pos + len]);
					   len++) ;
				
				if (lexer->pos + len < lexer->length && lexer->text[lexer->pos + len] == '.') {					// Float number?
					cur->type = TOK_TYPE_FLOAT;																	// Yes!
					len++;
					
					for (; lexer->pos + len < lexer->length && lexer_is_dec(lexer->text[lexer->pos + len]);
						   len++) ;
				}
			}
			
			cur->value = malloc((sign != -1 ? 1 : 0) + len + 1);												// Alloc space for copying the number
			
			if (cur->value == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			if (sign != -1) {																					// We have the sign?
				cur->value[0] = lexer->text[sign];																// Yes
				strncpy(cur->value + 1, &lexer->text[lexer->pos], len);
			} else {
				strncpy(cur->value, &lexer->text[lexer->pos], len);												// Nope!
			}
			
			cur->value[(sign != -1 ? 1 : 0) + len] = '\0';														// Add the NUL terminator at the end
			
			for (int i = 0; i < len; i++) {																		// Consume len bytes
				lexer_consume(lexer);
			}
		} else if (lexer->text[lexer->pos] == '"') {															// String?
			cur = lexer_new_token(list, cur);																	// Yes, create a new token at the end of the list
			
			if (cur == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			cur->type = TOK_TYPE_STRING;																		// Set the type
			cur->filename = lexer->filename;																	// Set the filename
			cur->line = lexer->line;																			// Set the line
			cur->col = lexer->col;																				// And the column
			
			lexer_consume(lexer);																				// Jump the start string character (")
			
			int len = 0;																						// Let's get the length of the string!
			
			for (; lexer->pos + len < lexer->length && lexer->text[lexer->pos + len] != '"'; len++) ;
			
			if (lexer->pos + len >= lexer->length) {															// Unterminated string?
				printf("%s: %d: %d: unterminated string\n", cur->filename, cur->line, cur->col);				// Yes :(
				token_free_list(list);
				return NULL;
			}
			
			cur->value = malloc(len + 1);																		// Alloc space for copying the string
			
			if (cur->value == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			strncpy(cur->value, &lexer->text[lexer->pos], len);													// Copy it!
			cur->value[len] = '\0';																				// Add the NUL terminator at the end
			
			for (int i = 0; i < len + 1; i++) {																	// Consume len bytes and the string end character (")
				lexer_consume(lexer);
			}
		} else if (lexer->text[lexer->pos] == '\'') {															// Single ASCII character?
			cur = lexer_new_token(list, cur);																	// Yes, create a new token at the end of the list
			
			if (cur == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			} else if (lexer->pos + 2 >= lexer->length) {														// Unterminated?
				printf("%s: %d: %d: unterminated character\n", cur->filename, cur->line, cur->col);				// Yes :(
				token_free_list(list);
				return NULL;
			} else if (lexer->text[lexer->pos + 2] != '\'') {													// Multi-byte character?
				printf("%s: %d: %d: multi-byte character\n", cur->filename, cur->line, cur->col);				// Yes :(
				token_free_list(list);
				return NULL;
			}
			
			cur->type = TOK_TYPE_NUMBER;
			cur->filename = lexer->filename;																	// Set the filename
			cur->line = lexer->line;																			// Set the line
			cur->col = lexer->col;																				// And the column
			
			int len = snprintf(NULL, 0, "%d", (int)lexer->text[lexer->pos + 1]);								// Get the buffer size
			
			cur->value = malloc(len + 1);																		// Alloc space for transform the character into int, and the int into string
			
			if (cur->value == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			snprintf(cur->value, len + 1, "%d", (int)lexer->text[lexer->pos + 1]);								// Transform the character into int, and the int into string!
			
			for (int i = 0; i < 3; i++) {																		// Consume the start (') char, the char itself and the end char (')
				lexer_consume(lexer);
			}
		} else if (lexer->text[lexer->pos] == '\n' || lexer->text[lexer->pos] == ',' ||
				   lexer->text[lexer->pos] == ':') {															// Single character token?
			cur = lexer_new_token(list, cur);																	// Yes, create a new token at the end of the list
			
			if (cur == NULL) {
				token_free_list(list);																			// Failed...
				return NULL;
			}
			
			cur->type = lexer->text[lexer->pos] == '\n' ? TOK_TYPE_EOS :
						(lexer->text[lexer->pos] == ',' ? TOK_TYPE_COMMA : TOK_TYPE_COLON);						// Set the type
			cur->filename = lexer->filename;																	// Set the filename
			cur->line = lexer->line;																			// Set the line
			cur->col = lexer->col;																				// And the column
			
			lexer_consume(lexer);																				// Consume the character
		} else if ((cur = arch_lex(lexer, list, cur)) == NULL) {												// Arch-specific?
			printf("%s: %d: %d: unrecognized character\n", lexer->filename, lexer->line, lexer->col);			// Nope...
			token_free_list(list);
			return NULL;
		}
	}
	
	return list;
}
