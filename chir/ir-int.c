// File author is Ítalo Lima Marconato Matias
//
// Created on November 24 of 2018, at 09:26 BRT
// Last edited on November 25 of 2018, at 14:41 BRT

#include <ir-int.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

ir_lexer_t *ir_create_lexer(char *text) {
	if (text == NULL) {																										// Valid input?
		return NULL;																										// Nope
	}
	
	ir_lexer_t *lexer = calloc(1, sizeof(ir_lexer_t));																		// Alloc space
	
	if (lexer != NULL) {																									// Failed?
		lexer->text = strdup(text);																							// No! Set the input
		lexer->length = strlen(text);																						// And set the input length
	}
	
	return lexer;
}

void ir_free_lexer(ir_lexer_t *lexer) {
	if (lexer != NULL) {																									// Check if our argument is valid
		free(lexer->text);																									// Free everything!
		free(lexer);
	}
}

ir_tok_t *ir_get_cur_tok(ir_lexer_t *lexer) {
	if (lexer == NULL) {																									// Check if our argument is valid
		return NULL;
	}
	
	return &lexer->cur;																										// Return!
}

int ir_lex_check(ir_lexer_t *lexer) {
	if (lexer == NULL) {																									// Check if our argument is valid
		return 0;
	}
	
	ir_tok_t *tok = NULL;
	
	ir_reset_lexer(lexer);																									// Reset the lexer
	
	while ((tok = ir_lex_next_tok(lexer)) != NULL) {																		// Let's try to lex!
		if (tok->type == IR_TTYPE_GVARIABLE || tok->type == IR_TTYPE_METHOD || tok->type == IR_TTYPE_STRING ||
			tok->type == IR_TTYPE_TYPE || tok->type == IR_TTYPE_IDENT) {													// Free the str_val?
			free(tok->str_val);																								// Yes
		}
	}
	
	if (lexer->pos < lexer->length) {
		return 0;																											// Failed, probably this code is not lexically correct
	}
	
	ir_reset_lexer(lexer);																									// Reset again
	
	return 1;																												// And return TRUE
}

static void ir_consume_whitespaces(ir_lexer_t *lexer) {
	while (lexer->pos < lexer->length && (lexer->text[lexer->pos] == ' ' || lexer->text[lexer->pos] == '\t' ||				// Whitespace maybe a: Space, Tab, Carriage Return or New Line
										  lexer->text[lexer->pos] == '\r' || lexer->text[lexer->pos] == '\n')) {
		lexer->pos++;
	}
}

static int ir_is_number(char c) {
	return c >= '0' && c <= '9';																							// Number is a... number
}

static int ir_is_ident(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';													// Identifier is any ASCII character or a '_'
}

static int ir_is_alnum(char c) {
	return ir_is_number(c) || ir_is_ident(c);																				// Alphanumeric would be any unicode valid character or number, but for us is any identifier character or number
}

ir_tok_t *ir_lex_next_tok(ir_lexer_t *lexer) {
	if (lexer == NULL) {																									// Check if our argument is valid
		return NULL;
	} else if (lexer->pos >= lexer->length) {																				// End of file?
		return NULL;																										// Yes
	}
	
	ir_consume_whitespaces(lexer);																							// Consume whitespaces
	
	if (lexer->pos + 1 < lexer->length && lexer->text[lexer->pos] == '/' && lexer->text[lexer->pos + 1] == '/') {			// Single line comment?
		while (lexer->pos < lexer->length && lexer->text[lexer->pos] != '\n') {												// Consume everthing until we find the end of the line (or the end of the file)
			lexer->pos++;
		}
		
		ir_consume_whitespaces(lexer);																						// Consume the whitespaces
	} else if (lexer->pos + 1 < lexer->length && lexer->text[lexer->pos] == '/' &&
			   lexer->text[lexer->pos + 1] == '*') {																		// Multi-line comment?
		lexer->pos += 2;																									// Yes!
		
		while (lexer->pos < lexer->length) {																				// Let's consume until we find the closing tag (or the end of the file)
			if (lexer->pos + 1 < lexer->length && lexer->text[lexer->pos] == '*' &&
				lexer->text[lexer->pos + 1] == '/') {																		// Close tag?
				lexer->pos += 2;																							// Yes :)
				break;
			}
			
			lexer->pos++;																									// Just consume
		}
		
		ir_consume_whitespaces(lexer);																						// Consume the whitespaces
	}
	
	if (lexer->pos >= lexer->length) {																						// End of file?
		return NULL;																										// Yes
	}
	
	if (ir_is_ident(lexer->text[lexer->pos]) || lexer->text[lexer->pos] == '*') {											// Identifier?
		int len = 0;																										// Let's get the length of this identifier
		
		for (; lexer->pos + len < lexer->length && (ir_is_alnum(lexer->text[lexer->pos + len]) ||
													lexer->text[lexer->pos + len] == '*'); len++) ;
		
		char *str = malloc(len + 1);																						// Alloc memory for copying the identifier
		
		if (str == NULL) {
			lexer->pos--;																									// Failed :(
			return NULL;
		}
		
		strncpy(str, &lexer->text[lexer->pos], len);																		// Copy!
		str[len] = '\0';																									// Add the NUL terminator
		
		lexer->cur.type = IR_TTYPE_IDENT;																					// Set the type
		lexer->cur.str_val = str;																							// Set the value
		lexer->pos += len;																									// Increase the current lexer position
	} else {
		switch (lexer->text[lexer->pos++]) {																				// Let's go!
		case '$': {																											// Variable
			if (lexer->pos >= lexer->length) {																				// End of file?
				lexer->pos--;																								// Yes... :(
				return NULL;
			}

			if (ir_is_number(lexer->text[lexer->pos])) {																	// Local variable?
				char *end = NULL;																							// Yes!

				lexer->cur.type = IR_TTYPE_LVARIABLE;																		// Set the type
				lexer->cur.uint_val = strtoul(&lexer->text[lexer->pos], &end, 0);											// Convert to uint
				lexer->pos += end - &lexer->text[lexer->pos];																// Increase the current lexer position
			} else if (ir_is_ident(lexer->text[lexer->pos])) {
				int len = 0;																								// Global variable, let's get the length of the var name

				for (; lexer->pos + len < lexer->length && ir_is_alnum(lexer->text[lexer->pos + len]); len++) ;

				char *str = malloc(len + 1);																				// Alloc memory for copying the var name

				if (str == NULL) {
					lexer->pos--;																							// Failed :(
					return NULL;
				}

				strncpy(str, &lexer->text[lexer->pos], len);																// Copy!
				str[len] = '\0';																							// Add the NUL terminator

				lexer->cur.type = IR_TTYPE_GVARIABLE;																		// Set the type
				lexer->cur.str_val = str;																					// Set the value
				lexer->pos += len;																							// Increase the current lexer position
			}

			break;
		}
		case '@': {																											// Method
			if (lexer->pos >= lexer->length) {																				// End of file?
				lexer->pos--;																								// Yes... :(
				return NULL;
			}
			
			int len = 0;																									// Let's get the length of the method name
		
			for (; lexer->pos + len < lexer->length && ir_is_alnum(lexer->text[lexer->pos + len]); len++) ;

			char *str = malloc(len + 1);																					// Alloc memory for copying the method name

			if (str == NULL) {
				lexer->pos--;																								// Failed :(
				return NULL;
			}

			strncpy(str, &lexer->text[lexer->pos], len);																	// Copy!
			str[len] = '\0';																								// Add the NUL terminator

			lexer->cur.type = IR_TTYPE_METHOD;																				// Set the type
			lexer->cur.str_val = str;																						// Set the value
			lexer->pos += len;																								// Increase the current lexer position
			
			break;
		}
		case '%': {																											// Number
			if (lexer->pos >= lexer->length) {																				// End of file?
				lexer->pos--;																								// Yes... :(
				return NULL;
			} else if (!ir_is_number(lexer->text[lexer->pos]) && lexer->text[lexer->pos] != '-' &&
					   lexer->text[lexer->pos] != '+') {																	// Valid number?
				lexer->pos--;																								// No >:(
				return NULL;
			}
			
			char *end = NULL;
			int val = strtol(&lexer->text[lexer->pos], &end, 0);															// First, let's try to convert to int
			
			if ((val == INT_MIN || val == INT_MAX) && errno == ERANGE) {
				lexer->cur.type = IR_TTYPE_UNUMBER;																			// Unsigned value (ERANGE), set the type
				lexer->cur.uint_val = strtoul(&lexer->text[lexer->pos], &end, 0);											// Convert to uint
				lexer->pos += end - &lexer->text[lexer->pos];																// Increase the current lexer position
			} else {
				lexer->cur.type = IR_TTYPE_SNUMBER;																			// It's a signed value :)
				lexer->cur.sint_val = val;																					// Set the value
				lexer->pos += end - &lexer->text[lexer->pos];																// Increase the current lexer position
			}
			
			break;
		}
		case '#': {
			if (lexer->pos + 1 >= lexer->length) {																			// End of file?
				lexer->pos--;																								// Yes... :(
				return NULL;
			} else if (lexer->text[lexer->pos] != '"') {
				lexer->pos--;																								// Yes... :(
				return NULL;
			}
			
			int len = 0;																									// Let's get the length of the method name
		
			for (; lexer->pos + len + 1 < lexer->length && lexer->text[lexer->pos + len + 1] != '"'; len++) ;
			
			if (lexer->pos + len + 1 >= lexer->length) {
				lexer->pos--;																								// End of file before closing the string :(
				return NULL;
			}
			
			char *str = malloc(len + 1);																					// Alloc memory for copying the string
			
			if (str == NULL) {
				lexer->pos--;																								// Failed :(
				return NULL;
			}
			
			strncpy(str, &lexer->text[lexer->pos + 1], len);																// Copy!
			str[len] = '\0';																								// Add the NUL terminator

			lexer->cur.type = IR_TTYPE_STRING;																				// Set the type
			lexer->cur.str_val = str;																						// Set the value
			lexer->pos += len + 2;																							// Increase the current lexer position
			
			break;
		}
		case '?': {																											// Type
			if (lexer->pos >= lexer->length) {																				// End of file?
				lexer->pos--;																								// Yes... :(
				return NULL;
			} else if (!ir_is_ident(lexer->text[lexer->pos])) {																// Valid identifier?
				lexer->pos--;																								// No >:(
				return NULL;
			}
			
			int len = 0;																									// Let's get the length of the type name
		
			for (; lexer->pos + len < lexer->length && ir_is_alnum(lexer->text[lexer->pos + len]); len++) ;

			char *str = malloc(len + 1);																					// Alloc memory for copying the type name

			if (str == NULL) {
				lexer->pos--;																								// Failed :(
				return NULL;
			}

			strncpy(str, &lexer->text[lexer->pos], len);																	// Copy!
			str[len] = '\0';																								// Add the NUL terminator

			lexer->cur.type = IR_TTYPE_TYPE;																				// Set the type
			lexer->cur.str_val = str;																						// Set the value
			lexer->pos += len;																								// Increase the current lexer position
			
			break;
		}
		case '(':
		case ')': {																											// Parentheses
			lexer->cur.type = IR_TTYPE_PAREN;
			lexer->cur.char_val = lexer->text[lexer->pos - 1];
			break;
		}
		case '{':
		case '}': {																											// Braces
			lexer->cur.type = IR_TTYPE_BRACE;
			lexer->cur.char_val = lexer->text[lexer->pos - 1];
			break;
		}
		case ',': {																											// Separator
			lexer->cur.type = IR_TTYPE_SEP;
			lexer->cur.char_val = lexer->text[lexer->pos - 1];
			break;
		}
		case '=': {																											// Assign
			lexer->cur.type = IR_TTYPE_ASSIGN;
			lexer->cur.char_val = lexer->text[lexer->pos - 1];
			break;
		}
		case ';': {																											// End Of Statement
			lexer->cur.type = IR_TTYPE_EOS;
			lexer->cur.char_val = lexer->text[lexer->pos - 1];
			break;
		}
		default: {
			lexer->pos--;																									// Invalid!!!!!
			return NULL;
			break;
		}
		}
	}
	
	return &lexer->cur;
}

void ir_reset_lexer(ir_lexer_t *lexer) {
	if (lexer == NULL) {																									// Check if our argument is valid
		return;
	}
	
	lexer->pos = 0;																											// Go back to the first position
	lexer->cur.type = 0;																									// Clean the type of the curr token
	lexer->cur.uint_val = 0;																								// And the value of the curr token
}
