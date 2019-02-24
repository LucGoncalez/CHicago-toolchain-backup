// File author is Ítalo Lima Marconato Matias
//
// Created on December 27 of 2018, at 11:42 BRT
// Last edited on February 23 of 2019, at 22:20 BRT

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>
#include <inttypes.h>

void node_free_list(node_t *node) {
	if (node != NULL) {																						// Null pointer check
		while (node != NULL) {																				// Ok, let's free the list!
			node_t *old = node;																				// Save the old one
			
			node = old->next;																				// Set the new one
			
			if (old->childs != NULL) {																		// Free the childs from this node?
				node_free_list(old->childs);																// Yes
				old->childs = NULL;
			}
			
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
		if (node->type == NODE_TYPE_METHOD_DEF) {															// Method definition?
			method_def_node_t *def = (method_def_node_t*)node;												// Yes, let's free the name, type and the body
			node_free(def->name);
			node_free(def->type);
			node_free_list(node->childs);
		} else if (node->type == NODE_TYPE_VARIABLE_DEF) {													// Variable definition?
			variable_def_node_t *def = (variable_def_node_t*)node;											// Yes, free the name, type and the value
			node_free(def->name);
			node_free(def->type);
			node_free_list(node->childs);
		} else if (node->type == NODE_TYPE_TYPECAST) {														// Typecast?
			variable_def_node_t *def = (variable_def_node_t*)node;											// Yes, free the type and the value
			node_free(def->type);
			node_free_list(node->childs);
		}
		
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

static node_t *parser_new_node(node_t *cur, size_t size, token_t *tok) {
	if (cur == NULL) {																						// We're at the start of the list?
		return (node_t*)calloc(1, size);																	// Yes :)
	}
	
	cur->next = calloc(1, size);																			// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																						// Failed...
	}
	
	cur->next->filename = tok->filename;																	// Set some info
	cur->next->line = tok->line;
	cur->next->col = tok->col;
	cur->next->prev = cur;																					// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

node_t *parser_parse_identifier(parser_t *parser, node_t *cur) {
	token_t *tok = parser_accept_noval(parser, TOK_TYPE_IDENTIFIER);										// Get our token
	
	if (tok == NULL) {
		tok = parser_accept_noval(parser, TOK_TYPE_TYPE);
		
		if (tok == NULL) {
			tok = parser_accept_noval(parser, TOK_TYPE_INSTRUCTION);
			
			if (tok == NULL) {
				tok = parser_accept_noval(parser, TOK_TYPE_CONDITION);
			}
		}
	}
	
	if (tok == NULL) {
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);													// Failed...
		return NULL;
	}
	
	node_t *ret = parser_new_node(cur, sizeof(identifier_node_t), tok);										// Create the node
	
	if (ret != NULL) {																						// Failed?
		ret->type = NODE_TYPE_IDENTIFIER;																	// No, so let's set the type
		((identifier_node_t*)ret)->value = tok->value;														// And the value!
	}
	
	return ret;
}

node_t *parser_parse_number(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);											// Get our token
	char *endptr = NULL;
	uintmax_t val = 0;
	
	if (tok == NULL) {
		return NULL;																						// Failed...
	} else if (tok->value[0] == '0' && tok->value[1] == 'b') {												// Binary?
		val = strtoumax(tok->value + 2, &endptr, 2);														// Yes, convert using base = 2
	} else {
		val = strtoumax(tok->value, &endptr, 0);															// Use auto-detection (base = 0)
	}
	
	node_t *ret = parser_new_node(cur, sizeof(number_node_t), tok);											// Create the node
	
	if (ret != NULL) {																						// Failed?
		ret->type = NODE_TYPE_NUMBER;																		// No, so let's set the type
		((number_node_t*)ret)->value = val;																	// And the value!
	}
	
	return ret;
}

node_t *parser_parse_string(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_STRING);											// Get our token
	
	if (tok == NULL) {
		return NULL;																						// Failed...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(string_node_t), tok);											// Create the node
	
	if (ret != NULL) {																						// Failed?
		ret->type = NODE_TYPE_STRING;																		// No, so let's set the type
		((identifier_node_t*)ret)->value = tok->value;														// And the value!
	}
	
	return ret;
}

node_t *parser_parse_float(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_FLOAT);												// Get our token
	char *endptr = NULL;
	long double val = 0;
	
	if (tok == NULL) {
		return NULL;																						// Failed...
	} else {
		val = strtold(tok->value, &endptr);
	}
	
	node_t *ret = parser_new_node(cur, sizeof(float_node_t), tok);											// Create the node
	
	if (ret != NULL) {																						// Failed?
		ret->type = NODE_TYPE_FLOAT;																		// No, so let's set the type
		((float_node_t*)ret)->value = val;																	// And the value!
	}
	
	return ret;
}

static node_t *parser_parse_variable(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_DOLLAR);											// Expect the $ sign
	
	if (tok == NULL) {
		return NULL;																						// ...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(node_t), tok);												// Create the node
	
	if (ret == NULL) {
		return NULL;																						// Failed
	}
	
	ret->type = NODE_TYPE_VAR;																				// Set the type
	
	if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {														// Number (local variable)?
		ret->childs = parser_parse_number(parser, NULL);													// Yes
	} else {
		ret->childs = parser_parse_identifier(parser, NULL);												// No, so it should be an identifier (global variable)
	}
	
	if (ret->childs == NULL) {
		free(ret);																							// Failed...
		return NULL;
	}
	
	return ret;
}

static node_t *parser_parse_typecast(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_QUESTION);											// Expect the ? sign
	
	if (tok == NULL) {
		return NULL;																						// ...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(typecast_node_t), tok);										// Create the node
	typecast_node_t *tc = (typecast_node_t*)ret;
	
	if (ret == NULL) {
		return NULL;																						// Faiiled
	}
	
	ret->type = NODE_TYPE_TYPECAST;																			// Set the type
	
	if (parser_check_noval(parser, TOK_TYPE_TYPE)) {														// Valid dest type?
		tc->type = parser_parse_identifier(parser, NULL);													// Yes :)
	} else {
		token_t *tk = parser->position;																		// No :(
		printf("%s: %d: %d: expected ttype %d, got ttype %d\n", tk->filename, tk->line, tk->col,
			   													TOK_TYPE_TYPE, tk->type);
		free(ret);
		return NULL;
	}
	
	if (parser_expect_noval(parser, TOK_TYPE_OPEN_PAREN) == NULL) {											// Expect the opening parentheses
		free(ret);
		return NULL;
	} else if (parser_accept_noval(parser, TOK_TYPE_AT)) {													// Method?
		ret->childs = parser_parse_identifier(parser, NULL);												// Yes, parse it!
		
		if (ret->childs == NULL) {
			free(ret);
			return NULL;
		}
			
		ret->childs->type = NODE_TYPE_METHOD;
	} else if (parser_accept_noval(parser, TOK_TYPE_PERCENT)) {												// Number?
		ret->childs = parser_parse_number(parser, NULL);													// Yes, parse it!
		
		if (ret->childs == NULL) {																			// Failed?
			ret->childs = parser_parse_float(parser, NULL);													// Yes, try to parse as float
		}
		
		if (ret->childs == NULL) {
			free(ret);
			return NULL;
		}
	} else if (parser_check_noval(parser, TOK_TYPE_DOLLAR)) {												// Variable?
		ret->childs = parser_parse_variable(parser, NULL);													// Yes, parse it!
		
		if (ret->childs == NULL) {
			free(ret);
			return NULL;
		}
	} else if (parser_accept_noval(parser, TOK_TYPE_HASH)) {													// String?
		ret->childs = parser_parse_string(parser, NULL);													// Yes, parse it!
		
		if (ret->childs == NULL) {
			free(ret);
			return NULL;
		}
	} else {
		token_t *tk = parser->position;																		// Invalid/unimplemented...
		printf("%s: %d: %d: invalid/unimplemented ttype %d\n", tk->filename, tk->line,
			   tk->col, tk->type);
		free(ret);
		return NULL;
	}
	
	if (parser_expect_noval(parser, TOK_TYPE_CLOSE_PAREN) == NULL) {										// Expect the closing parentheses
		free(ret);
		return NULL;
	}
	
	return ret;
}

static int parser_parse_chain(parser_t *parser, node_t **listp, int lastt) {
	node_t *list = *listp;
	node_t *cur = NULL;
	
	while (parser->position != NULL && !(lastt != -1 ? parser_check_noval(parser, lastt) : 0)) {			// Let's go!
		token_t *tok = parser->position;
		
		if (parser_accept_noval(parser, TOK_TYPE_AT)) {														// Method definition?
			cur = parser_new_node(cur, sizeof(method_def_node_t), tok);										// Yes, create the ast node
			method_def_node_t *def = (method_def_node_t*)cur;
			
			if (cur == NULL) {
				return 0;																					// Failed :(
			} else if (list == NULL) {
				list = cur;
			}
			
			cur->type = NODE_TYPE_METHOD_DEF;
			def->name = parser_parse_identifier(parser, cur);												// Get the name
			
			if (def->name == NULL) {
				free(def);
				return 0;
			}
			
			if (parser_expect_noval(parser, TOK_TYPE_OPEN_PAREN) == NULL) {									// Expect opening parentheses
				free(def);
				return 0;
			} else if (parser_check_noval(parser, TOK_TYPE_TYPE)) {											// Type?
				def->type = parser_parse_identifier(parser, cur);											// Yes :)
				
				if (def->type == NULL) {
					free(def);
					return 0;
				}
			}
			
			if (parser_expect_noval(parser, TOK_TYPE_CLOSE_PAREN) == NULL) {								// Expect closing parentheses
				free(def);
				return 0;
			} else if (parser_expect_noval(parser, TOK_TYPE_OPEN_BRACE) == NULL) {							// Expect opening brace
				free(def);
				return 0;
			}
			
			parser_parse_chain(parser, &cur->childs, TOK_TYPE_CLOSE_BRACE);
			
			if (parser_expect_noval(parser, TOK_TYPE_CLOSE_BRACE) == NULL) {								// Expect closing brace
				free(def);
				return 0;
			}
		} else if (parser_check_noval(parser, TOK_TYPE_INSTRUCTION)) {										// Instruction?
			cur = parser_new_node(cur, sizeof(instruction_node_t), tok);									// Yes, create the ast node
			instruction_node_t *instr = (instruction_node_t*)cur;
			
			if (cur == NULL) {
				return 0;																					// Failed :(
			} else if (list == NULL) {
				list = cur;
			}
			
			cur->type = NODE_TYPE_INSTRUCTION;
			instr->name = parser_parse_identifier(parser, cur);												// Get the name
			
			if (instr->name == NULL) {
				free(instr);
				return 0;
			}
			
			if (!parser_check_noval(parser, TOK_TYPE_SEMICOLON)) {											// Arguments?
				node_t *curr = cur->childs;																	// Yes, let's parse them!
				
				do {
					if (parser_accept_noval(parser, TOK_TYPE_CONDITION)) {									// Condition?
						curr = parser_parse_identifier(parser, curr);										// Yes, parse it!
						
						if (curr == NULL) {
							free(instr);
							return 0;
						} else if (cur->childs == NULL) {
							cur->childs = curr;
						}
						
						curr->type = NODE_TYPE_CONDITION;
					} else if (parser_accept_noval(parser, TOK_TYPE_AT)) {									// Method?
						curr = parser_parse_identifier(parser, curr);										// Yes, parse it!
						
						if (curr == NULL) {
							free(instr);
							return 0;
						} else if (cur->childs == NULL) {
							cur->childs = curr;
						}
						
						curr->type = NODE_TYPE_METHOD;
					} else if (parser_check_noval(parser, TOK_TYPE_QUESTION)) {								// Typecast?
						curr = parser_parse_typecast(parser, curr);											// Yes, parse it!
						
						if (curr == NULL) {
							free(instr);
							return 0;
						} else if (cur->childs == NULL) {
							cur->childs = curr;
						}
					} else if (parser_accept_noval(parser, TOK_TYPE_PERCENT)) {								// Number?
						node_t *tmp = parser_parse_number(parser, curr);									// Yes, parse it!
						
						if (tmp == NULL) {																	// Failed?
							curr = parser_parse_float(parser, curr);										// Yes, try to parse as float
						} else {
							curr = tmp;
						}
						
						if (curr == NULL) {
							free(instr);
							return 0;
						} else if (cur->childs == NULL) {
							cur->childs = curr;
						}
					} else if (parser_check_noval(parser, TOK_TYPE_DOLLAR)) {								// Variable?
						curr = parser_parse_variable(parser, curr);											// Yes, parse it!
						
						if (curr == NULL) {
							free(instr);
							return 0;
						} else if (cur->childs == NULL) {
							cur->childs = curr;
						}
					} else if (parser_accept_noval(parser, TOK_TYPE_HASH)) {								// String?
						curr = parser_parse_string(parser, curr);											// Yes, parse it!
						
						if (curr == NULL) {
							free(instr);
							return 0;
						} else if (cur->childs == NULL) {
							cur->childs = curr;
						}
					} else {
						printf("%s: %d: %d: invalid/unimplemented ttype %d\n", tok->filename, tok->line,
							   tok->col, tok->type);														// Invalid/unimplemented...
						free(instr);
						return 0;
					}
				} while (parser_accept_noval(parser, TOK_TYPE_COMMA));
			}
			
			if (parser_expect_noval(parser, TOK_TYPE_SEMICOLON) == NULL) {									// Expect semicolon in the end
				free(instr);
				return 0;
			}
		} else {
			printf("%s: %d: %d: invalid/unimplemented ttype %d\n", tok->filename, tok->line, tok->col,		// Invalid/unimplemented...
				   tok->type);
			return 0;
		}
	}
	
	*listp = list;
	
	return 1;
}

node_t *parser_parse(parser_t *parser) {
	if (parser == NULL || parser->tokens == NULL) {															// Null pointer checks
		return NULL;
	}
	
	node_t *list = NULL;
	
	parser->position = parser->tokens;																		// Set the current token to the start of the list
	
	if (!parser_parse_chain(parser, &list, -1)) {															// GO!
		node_free_list(list);																				// :(
	}
	
	return list;
}
