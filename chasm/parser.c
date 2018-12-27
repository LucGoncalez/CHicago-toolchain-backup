// File author is Ítalo Lima Marconato Matias
//
// Created on December 27 of 2018, at 11:42 BRT
// Last edited on December 27 of 2018, at 16:01 BRT

#include <arch.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

void node_free_list(node_t *node) {
	if (node != NULL) {																						// Null pointer check
		while (node != NULL) {																				// Ok, let's free the list!
			node_t *old = node;																				// Save the old one
			node = old->next;																				// Set the new one
			node_free(old);																					// Free the old one
		}
	}
}

node_t *node_rewind_list(node_t *node) {
	if (node != NULL) {																						// Null pointer check
		while (node->prev != NULL) {																		// First entry?
			node = node->prev;																				// No, go one entry back!
		}
	}
	
	return node;
}

void node_free(node_t *node) {
	if (node != NULL) {																						// Null pointer check
		free(node);																							// Free the node struct!
	}
}

parser_t *parser_new(token_t *tokens) {
	if (tokens == NULL) {																					// Null pointer check
		return NULL;
	}
	
	parser_t *parser = malloc(sizeof(parser_t));															// Alloc space
	
	if (parser != NULL) {																					// Failed?
		parser->tokens = tokens;																			// No! Set the token list
	}
	
	return parser;
}

void parser_free(parser_t *parser) {
	if (parser != NULL) {																					// Check if our argument is valid
		token_free_list(parser->tokens);																	// Free everything!
		free(parser);
	}
}

static void parser_consume(parser_t *parser) {
	if (parser->position != NULL) {																			// This was the last entry?
		parser->position = parser->position->next;															// Nope, go to the next one
	}
}

int parser_check_noval(parser_t *parser, uint8_t type) {
	if (parser == NULL || parser->position == NULL) {														// Null pointer check
		return 0;
	} else if (parser->position->type == type) {															// It's the one that we want?
		return 1;																							// Yes!
	}
	
	return 0;																								// Nope...
}

int parser_check_val(parser_t *parser, uint8_t type, char *val) {
	if (parser == NULL || parser->position == NULL) {														// Null pointer check
		return 0;
	} else if (val == NULL) {																				// We really need to check the val?
		return parser_check_noval(parser, type);															// Nope :)
	} else if (parser->position->type == type && !strcmp(parser->position->value, val)) {					// It's the one that we want?
		return 1;																							// Yes!
	}
	
	return 0;																								// Nope...
}

token_t *parser_accept_noval(parser_t *parser, uint8_t type) {
	if (parser_check_noval(parser, type)) {																	// It's the one that we want?
		token_t *ret = parser->position;																	// Yes!
		parser_consume(parser);
		return ret;
	}
	
	return NULL;																							// Nope...
}

token_t *parser_accept_val(parser_t *parser, uint8_t type, char *val) {
	if (parser_check_val(parser, type, val)) {																// It's the one that we want?
		token_t *ret = parser->position;																	// Yes!
		parser_consume(parser);
		return ret;
	}
	
	return NULL;																							// Nope...
}

token_t *parser_expect_noval(parser_t *parser, uint8_t type) {
	if (parser_check_noval(parser, type)) {																	// It's the one that we want?
		token_t *ret = parser->position;																	// Yes!
		parser_consume(parser);
		return ret;
	} else if (parser != NULL && parser->position != NULL) {												// We can print the error message?
		char *fn = parser->position->filename;																// Yes!
		int ttype = parser->position->type;
		int line = parser->position->line;
		int col = parser->position->col;
		
		printf("%s: %d: %d: expected ttype %d, got ttype %d\n", fn, line, col, type, ttype);				// Print the error message!
	}
	
	return NULL;
}

token_t *parser_expect_val(parser_t *parser, uint8_t type, char *val) {
	if (parser_check_val(parser, type, val)) {																// It's the one that we want?
		token_t *ret = parser->position;																	// Yes!
		parser_consume(parser);
		return ret;
	} else if (parser != NULL && val != NULL && parser->position != NULL) {									// We can print the error message (with the val)?
		char *fn = parser->position->filename;																// Yes!
		char *tval = parser->position->value;
		int ttype = parser->position->type;
		int line = parser->position->line;
		int col = parser->position->col;
		
		printf("%s: %d: %d: expected ttype %d with val '%s', got ttype %d with val '%s'\n",					// Print the error message!
			   fn, line, col, type, val, ttype, tval);
	} else if (parser != NULL && parser->position != NULL) {												// We can print the error message (without the val)?
		char *fn = parser->position->filename;																// Yes!
		int ttype = parser->position->type;
		int line = parser->position->line;
		int col = parser->position->col;
		
		printf("%s: %d: %d: expected ttype %d, got ttype %d\n", fn, line, col, type, ttype);				// Print the error message!
	}
	
	return NULL;
}

static void parser_consume_newlines(parser_t *parser) {
	while (parser->position != NULL && parser->position->type == TOK_TYPE_EOS) {							// Consume all the EOS until the next directive/label/etc
		parser_consume(parser);
	}
}

static node_t *parser_new_node(node_t *cur, size_t size) {
	if (cur == NULL) {																						// We're at the start of the list?
		return (node_t*)calloc(1, size);																	// Yes :)
	}
	
	cur->next = calloc(1, size);																			// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																						// Failed...
	}
	
	cur->next->prev = cur;																					// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

node_t *parser_parse_identifier(parser_t *parser) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);										// Get our token
	
	if (tok == NULL) {
		return NULL;																						// Failed...
	}
	
	return NULL;
}

node_t *parser_parse_number(parser_t *parser) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);											// Get our token
	char *endptr = NULL;
	uintmax_t val = 0;
	
	if (tok == NULL) {
		return NULL;																						// Failed...
	} if (tok->value[0] == '0' && tok->value[1] == 'b') {													// Binary?
		val = strtoumax(tok->value + 2, &endptr, 2);														// Yes, convert using base = 2
	} else {
		val = strtoumax(tok->value, &endptr, 0);															// Use auto-detection (base = 0)
	}
	
	node_t *ret = parser_new_node(NULL, sizeof(number_node_t));												// Create the node
	
	if (ret != NULL) {																						// Failed?
		ret->type = NODE_TYPE_NUMBER;																		// No, so let's set the type
		((number_node_t*)ret)->value = val;																	// And the value!
	}
	
	return ret;
}

static void node_print(node_t *node) {
	if (node == NULL) {
		return;
	}
	
	static int tabs = 0;
	
	if (node->type == NODE_TYPE_IDENTIFIER) {
		for (int i = 0; i < tabs; i++) {
			printf("\t");
		}
		
		printf("Identifier: %s\n", ((identifier_node_t*)node)->value);
	} else if (node->type == NODE_TYPE_NUMBER) {
		for (int i = 0; i < tabs; i++) {
			printf("\t");
		}
		
		printf("Number: %lu\n", ((number_node_t*)node)->value);
	} else if (node->type == NODE_TYPE_DEFINE_DIRECTIVE) {
		for (int i = 0; i < tabs; i++) {
			printf("\t");
		}
		
		printf("Define Directive (size = %d)\n", ((define_directive_node_t*)node)->size);
		
		if (node->childs != NULL) {
			tabs++;
			node_print(node->childs);
			tabs--;
		}
	} else if (node->type == NODE_TYPE_LABEL) {
		for (int i = 0; i < tabs; i++) {
			printf("\t");
		}
		
		printf("Label: %s\n", ((label_node_t*)node)->name);
	}
	
	if (node->next != NULL) {
		node_print(node->next);
	}
}

node_t *parser_parse(parser_t *parser) {
	if (parser == NULL || parser->tokens == NULL) {															// Null pointer checks
		return NULL;
	}
	
	node_t *list = NULL;
	node_t *cur = NULL;
	
	parser->position = parser->tokens;																		// Set the current token to the start of the list
	
	parser_consume_newlines(parser);																		// Consume all the EOS until the first directive/label/etc
	
	while (parser->position != NULL) {																		// Let's parse!
		token_t *tok = parser->position;
		int size = 0;
		
		if ((size = parser_accept_val(parser, TOK_TYPE_DIRECTIVE, "db") != NULL ? 1 : 0) == 1 ||
		    (size = parser_accept_val(parser, TOK_TYPE_DIRECTIVE, "dw") != NULL ? 2 : 0) == 2 ||
		    (size = parser_accept_val(parser, TOK_TYPE_DIRECTIVE, "dd") != NULL ? 4 : 0) == 4 ||
		    (size = parser_accept_val(parser, TOK_TYPE_DIRECTIVE, "dq") != NULL ? 8 : 0) == 8) {			// Define byte/word/dword/quad directive?
			node_t *val = NULL;																				// First, let's get the val!
			
			if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {											// With an identifier?
				val = parser_parse_identifier(parser);
			} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {										// Number?
				val = parser_parse_number(parser);
			}
			
			if (val == NULL) {
				printf("%s: %d: %d: invalid argument\r\n", tok->filename, tok->line, tok->col);				// Failed to get the val...
				node_free_list(list);
				return NULL;
			}
			
			parser_expect_noval(parser, TOK_TYPE_EOS);														// Expect the new line in the end of the statement
			
			cur = parser_new_node(cur, sizeof(define_directive_node_t));									// Create the node
			
			if (cur == NULL) {
				node_free_list(list);																		// Failed...
				return NULL;
			} else if (list == NULL) {
				list = cur;
			}
			
			cur->type = NODE_TYPE_DEFINE_DIRECTIVE;															// Set the type
			cur->childs = val;																				// Set the argument
			((define_directive_node_t*)cur)->size = size;													// And the size
		} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {										// Label?
			char *name = parser_expect_noval(parser, TOK_TYPE_IDENTIFIER)->value;							// Yes, save the name
			
			if (!parser_accept_noval(parser, TOK_TYPE_COLON)) {												// Expect the colon
				printf("%s: %d: %d: label without a colon\n", tok->filename, tok->line, tok->col);			// ...
			}
			
			cur = parser_new_node(cur, sizeof(label_node_t));												// Create the node
			
			if (cur == NULL) {
				node_free_list(list);																		// Failed...
				return NULL;
			} else if (list == NULL) {
				list = cur;
			}
			
			cur->type = NODE_TYPE_LABEL;																	// Set the type
			((label_node_t*)cur)->name = name;																// And the label name!
		} else {
			printf("%s: %d: %d: Invalid/unimplemented ttype %d\n", tok->filename, tok->line, tok->col,		// Invalid/unimplemented...
				   tok->type);
			node_free_list(list);
			return NULL;
		}
		
		parser_consume_newlines(parser);																	// Consume all the EOS until the next directive/label/etc
	}
	
	node_print(list);
	
	return list;
}
