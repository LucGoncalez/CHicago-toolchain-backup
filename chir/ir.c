// File author is Ítalo Lima Marconato Matias
//
// Created on November 19 of 2018, at 14:49 BRT
// Last edited on December 02 of 2018, at 08:28 BRT

#define _DEFAULT_SOURCE

#include <ir.h>
#include <str.h>
#include <ir-int.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static char *ir_types[] = {
	"Void",
	"Int8", "UInt8",
	"Int16", "UInt16",
	"Int32", "UInt32",
	"Int8*", "Local", "Global", "Method"
};

static char *ir_instrs[] = {
	"mov",
	"add", "sub", "mul", "div", "mod",
	"shl", "shr", "and", "or", "xor",
	"cmp",
	"br", "brc", "call", "callc",
	"return"
};

ir_oper_t *ir_create_oper(uint16_t type) {
	ir_oper_t *oper = calloc(1, sizeof(ir_oper_t));																	// Alloc space
	
	if (oper != NULL) {																								// Failed?
		oper->type = type;																							// No! Set the type!
	}
	
	return oper;
}

void ir_free_oper(ir_oper_t *oper) {
	if (oper != NULL) {																								// Valid?
		free(oper);																									// Yes, free it
	}
}

ir_arg_t *ir_create_arg(uint16_t type, char *name) {
	ir_arg_t *arg = calloc(1, sizeof(ir_arg_t));																	// Alloc space
	
	if (arg != NULL) {																								// Failed?
		arg->type = type;																							// No! Set the type!
		arg->name = strdup(name);																					// Duplicate the name
		
		if (arg->name == NULL) {
			free(arg);																								// Failed...
			return NULL;
		}
	}
	
	return arg;
}

void ir_free_arg(ir_arg_t *arg) {
	if (arg != NULL) {																								// Valid?
		ir_free_oper(arg->assign);																					// Yes, free the assign
		free(arg->name);																							// Free the name
		free(arg);																									// And the arg struct
	}
}

ir_instr_t *ir_create_instr(uint8_t op, uint32_t opers) {
	ir_instr_t *instr = calloc(1, sizeof(ir_instr_t));																// Alloc space
	
	if (instr != NULL) {																							// Failed?
		instr->op = op;																								// No! Set the operation
		
		if (opers != 0) {
			instr->operand_count = opers;																			// The amount of operands
			instr->operands = calloc(1, sizeof(ir_oper_t*) * opers);												// And alloc space for the operands

			if (instr->operands == NULL) {
				free(instr);																						// Failed...
				return NULL;
			}
		}
	}
	
	return instr;
}

void ir_free_instr(ir_instr_t *instr) {
	if (instr != NULL) {																							// Valid?
		for (uint32_t i = 0; i < instr->operand_count; i++) {														// Yes, let's free the operands
			ir_free_oper(instr->operands[i]);
		}
		
		free(instr);																								// And the instr struct itself
	}
}

static int ir_check_global(ir_module_t *module, char *name) {
	for (uint32_t i = 0; i < module->global_count; i++) {															// Let's search!
		ir_arg_t *g = module->globals[i];
		
		if ((strlen(g->name) == strlen(name)) && !strcmp(g->name, name)) {											// Found?
			return 1;																								// Yes :)
		}
	}
	
	return 0;
}

int ir_check_instr(ir_module_t *module, ir_method_t *method, ir_instr_t *instr) {
	if (instr == NULL) {																							// Sanity check
		return 0;
	}
	
	int varg1 = 0;
	int varg2 = 0;
	int varg3 = 0;
	int vargv = 0;
	
	if (instr->op == IR_INSTR_MOV || instr->op == IR_INSTR_CMP || instr->op == IR_INSTR_BRC ||						// This instruction have 2 operands?
		instr->op == IR_INSTR_CALLC) {
		if (instr->operand_count != 2) {																			// Yes, but we have a valid amount of operands?
			if (!((instr->op == IR_INSTR_BRC || instr->op == IR_INSTR_CALLC) && instr->operand_count > 2)) {		// No, but this is a jump/call?
				return 0;																							// No...
			}
		}
		
		varg1 = (instr->op == IR_INSTR_MOV) ? (IR_TYPE_LOCAL | IR_TYPE_GLOBAL) :									// Set the valid types for the first op
				((instr->op == IR_INSTR_CMP) ? (IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
												IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
												IR_TYPE_LOCAL | IR_TYPE_GLOBAL) : IR_TYPE_METHOD);
		
		varg2 = (instr->op == IR_INSTR_BRC || instr->op == IR_INSTR_CALLC) ? varg2 :								// For the second op
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL);
		
		varg3 = (instr->op == IR_INSTR_BRC || instr->op == IR_INSTR_CALLC) ?										// For the third op
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL) : varg3;
		
		vargv = (instr->op == IR_INSTR_BRC || instr->op == IR_INSTR_CALLC) ?										// And for everything else
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL) : vargv;
	} else if (instr->op == IR_INSTR_BR || instr->op == IR_INSTR_CALL || instr->op == IR_INSTR_RETURN) {			// 1 operand?
		if (instr->operand_count != 1) {																			// Yes, but we have a valid amount of operands?
			if (!(instr->op == IR_INSTR_RETURN && instr->operand_count == 0) &&										// No, but jump/call can have more than 1 argument, and return can have 0 arguments
			   	!((instr->op == IR_INSTR_BR || instr->op == IR_INSTR_CALL) && instr->operand_count > 1)) {
				return 0;																							// ...
			}
		}
		
		varg1 = (instr->op == IR_INSTR_BR || instr->op == IR_INSTR_CALL) ? IR_TYPE_METHOD :							// Set the valid types for the first op
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL);
		
		varg2 = (instr->op == IR_INSTR_BR || instr->op == IR_INSTR_CALL) ?											// For the second op
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL) : varg2;
		
		varg3 = (instr->op == IR_INSTR_BR || instr->op == IR_INSTR_CALL) ?											// For the third op
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL) : varg3;
		
		vargv = (instr->op == IR_INSTR_BR || instr->op == IR_INSTR_CALL) ?											// And for everything else
				(IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL) : vargv;
	} else {
		if (instr->operand_count != 3) {																			// Ok, this is a 3-operands instruction, we have a valid amount of ops?
			return 0;																								// Nope
		}
		
		varg1 = IR_TYPE_LOCAL | IR_TYPE_GLOBAL;																		// Set the vaiid types for the first op
		
		varg2 = (IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |									// For the second op
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL);
		
		varg3 = (IR_TYPE_INT8 | IR_TYPE_UINT8 | IR_TYPE_INT16 | IR_TYPE_UINT16 |									// And for the third op
				 IR_TYPE_INT32 | IR_TYPE_UINT32 | IR_TYPE_STRING |
				 IR_TYPE_LOCAL | IR_TYPE_GLOBAL);
	}
	
	for (uint32_t i = 0; i < instr->operand_count; i++) {															// Now, check all the operands!
		ir_oper_t *oper = instr->operands[i];
		
		if (!(oper->type & ((i == 0) ? varg1 : ((i == 1) ? varg2 : ((i == 2) ? varg3 : vargv))))) {					// Valid op?
			return 0;																								// Invalid...
		} else if (oper->type == IR_TYPE_LOCAL && method != NULL &&													// Increase the local variable count?
				   method->local_count <= oper->val.uint32_val) {
			method->local_count = oper->val.uint32_val + 1;															// Yes
		} else if (oper->type == IR_TYPE_GLOBAL && module != NULL &&												// Invalid global?
				   !ir_check_global(module, oper->val.str_val)) {
			return 0;																								// Yes :(
		}
	}
	
	return 1;
}

ir_method_t *ir_create_method(char *name, uint32_t args, uint32_t locals, uint16_t type) {
	if (name == NULL) {																								// Valid name?
		return NULL;																								// Nope
	}
	
	ir_method_t *method = calloc(1, sizeof(ir_method_t));															// Alloc space
	
	if (method != NULL) {																							// Failed?
		method->name = strdup(name);																				// No! Set the name
		
		if (method->name == NULL) {
			free(method);																							// Failed...
			return NULL;
		}
		
		if (args != 0) {
			method->arg_count = args;																				// Set the amount of arguments
			method->args = calloc(1, args);																			// And alloc space for the arguments

			if (method->args == NULL) {
				free(method->name);																					// Failed...
				free(method);
				return NULL;
			}
		}
		
		method->local_count = locals;																				// Set the amount of locals in this method
		method->type = type;																						// And set the return type of this method
	}
	
	return method;
}

void ir_free_method(ir_method_t *method) {
	if (method != NULL) {																							// Valid?
		for (uint32_t i = 0; i < method->body_count; i++) {															// Yes, let's free the body
			free(method->body[i]);
		}
		
		free(method->args);																							// Free the arguments
		free(method->name);																							// Free the name
		free(method);																								// And the method struct itself
	}
}

int ir_check_method(ir_module_t *module, ir_method_t *method) {
	if (method != NULL) {																							// Sanity check
		for (uint32_t i = 0; i < method->body_count; i++) {															// Just check all the instructions in this method
			if (!ir_check_instr(module, method, method->body[i])) {
				return 0;
			}
		}
	}
	
	return 1;
}

int ir_add_instr(ir_method_t *method, ir_instr_t *instr) {
	if (method == NULL || instr == NULL) {																			// Check if our arguments are valid
		return 0;																									// ...
	}
	
	ir_instr_t **body = realloc(method->body, (method->body_count + 1) * sizeof(ir_instr_t));						// Realloc the body
	
	if (body == NULL) {
		return 0;																									// Failed..
	}
	
	body[method->body_count] = instr;																				// Add this instr
	method->body = body;																							// Set the new body
	method->body_count++;																							// And increase the instr count
	
	return 1;
}

ir_module_t *ir_create_module(uint32_t globals) {
	ir_module_t *module = calloc(1, sizeof(ir_module_t));															// Alloc space
	
	if (module != NULL && globals > 0) {																			// Failed? We need to alloc space for the globals?
		module->global_count = globals;																				// Yes, we need! Set the globals count for this module
		module->globals = calloc(1, sizeof(ir_arg_t) * globals);													// And alloc space for the globals
		
		if (module->globals == NULL) {
			free(module);																							// Failed...
			return NULL;
		}
	}
	
	return module;
}

static int ir_type_from_string(char *str) {
	for (uint32_t i = 0; i < sizeof(ir_types) / sizeof(ir_types[0]); i++) {											// Let's search!
		if (!strcmp(str, ir_types[i])) {																			// Found?
			return i;																								// Yes!
		}
	}
	
	return -1;																										// No, return -1
}

static int ir_instr_from_string(char *str) {
	for (uint32_t i = 0; i < sizeof(ir_instrs) / sizeof(ir_instrs[0]); i++) {										// Let's search!
		if (!strcmp(str, ir_instrs[i])) {																			// Found?
			return i;																								// Yes!
		}
	}
	
	return -1;																										// No, return -1
}

ir_module_t *ir_module_from_text(char *text) {
	if (text == NULL) {																								// Check if our input is valid
		return NULL;
	}
	
	ir_lexer_t *lexer = ir_create_lexer(text);																		// Create our lexer
	ir_tok_t *tok = NULL;
	uint32_t globals = 0;
	uint32_t currglb = 0;
	
	if (lexer == NULL) {
		return NULL;																								// Failed...
	} else if (!ir_lex_check(lexer)) {																				// First, check if this code is lexically correct
		return NULL;
	}
	
	while ((tok = ir_lex_next_tok(lexer)) != NULL) {																// Now, let's get the amount of global variables
		if (tok->type == IR_TTYPE_GVARIABLE) {																		// Found global variable?
			free(tok->str_val);																						// Yes, free the name, we don't need it for now!
			
			tok = ir_lex_next_tok(lexer);																			// Try to get the next token
			
			if (tok != NULL && tok->type == IR_TTYPE_PAREN && tok->char_val == '(') {								// Found the var type start (open parentheses)?
				tok = ir_lex_next_tok(lexer);																		// Yes! Let's try to get the next token
				
				if (tok != NULL && tok->type == IR_TTYPE_IDENT) {													// Found the var type name?
					free(tok->str_val);																				// Yes!! Free the name, we don't need it for now!
					
					tok = ir_lex_next_tok(lexer);																	// Try to get the next token
					
					if (tok != NULL && tok->type == IR_TTYPE_PAREN && tok->char_val == ')') {						// Found the var type end (close parentheses)?
						tok = ir_lex_next_tok(lexer);																// Yes!!! Try to get the next token (time to discover if this is a global var def!)
						
						if (tok != NULL && (tok->type == IR_TTYPE_EOS || tok->type == IR_TTYPE_ASSIGN)) {			// Found semicolon or equal sign?
							globals++;																				// YES!!!! This is a global variable def
						}
					}
				}
			}
		}
	}
	
	ir_module_t *module = ir_create_module(globals);																// Create our module!
	
	if (module == NULL) {
		ir_free_lexer(lexer);																						// Failed :(
		return NULL;
	}
	
	ir_reset_lexer(lexer);																							// Reset the lexer
	
	while ((tok = ir_lex_next_tok(lexer)) != NULL) {																// Now, let's go!
		if (tok->type == IR_TTYPE_GVARIABLE) {																		// Global variable def?
			char *name = tok->str_val;																				// Yes, let's save the name
			
			if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN || tok->char_val != '(') {	// Check if we have our type start (open parentheses)
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			} else if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_IDENT) {						// Check if we have a valid type... type
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			char *type = tok->str_val;																				// Save the type!
			
			if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN || tok->char_val != ')') {	// Check if we have our type end (close parentheses)
				free(type);
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			ir_oper_t *assign = NULL;
			
			if ((tok = ir_lex_next_tok(lexer)) == NULL) {															// Try to get the next token
				free(type);
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			} else if (tok->type == IR_TTYPE_ASSIGN) {																// Found default assign?
				if ((tok = ir_lex_next_tok(lexer)) == NULL || (tok->type != IR_TTYPE_GVARIABLE &&
															   tok->type != IR_TTYPE_STRING &&
															   tok->type != IR_TTYPE_TYPE)) {						// Yes! It's a valid (string or type) start?
					free(type);																						// No >:(
					free(name);
					ir_free_module(module);
					ir_free_lexer(lexer);
					return NULL;
				} else if (tok->type == IR_TTYPE_GVARIABLE) {														// Default value = Global variable value?
					assign = ir_create_oper(IR_TYPE_GLOBAL);														// Yes! Let's create our assign
					
					if (assign == NULL) {
						free(tok->str_val);																			// Failed >:(
						free(type);
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					assign->val.str_val = tok->str_val;																// Assign the value!
				} else if (tok->type == IR_TTYPE_STRING) {															// Default value = Int8*?
					assign = ir_create_oper(IR_TYPE_STRING);														// Yes! Let's create our assign
					
					if (assign == NULL) {
						free(tok->str_val);																			// Failed >:(
						free(type);
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					assign->val.str_val = tok->str_val;																// Assign the value!
				} else {
					int8_t ttype = ir_type_from_string(tok->str_val);												// Try to get the type from the type name
					
					if (ttype == -1) {
						free(tok->str_val);																			// Failed >:(
						free(type);
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					free(tok->str_val);																				// Free the type name, we don't need it now!
					
					if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
							   tok->char_val != '(') {																// Valid val start?
						free(type);																					// Nope...
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					} else if ((tok = ir_lex_next_tok(lexer)) == NULL) {											// Try to get the next token
						free(type);
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					assign = ir_create_oper(ttype);																	// Try to create our assign
					
					if (assign == NULL) {
						free(type);																					// Failed...
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					if (tok->type == IR_TTYPE_SNUMBER) {															// Signed number?
						if (ttype == IR_TYPE_INT8 || ttype == IR_TYPE_UINT8) {										// Yes, 8-bits?
							assign->val.int8_val = (int8_t)tok->sint_val;
						} else if (ttype == IR_TYPE_INT16 || ttype == IR_TYPE_UINT16) {								// 16-bits?
							assign->val.int16_val = (int16_t)tok->sint_val;
						} else {																					// 32-bits?
							assign->val.int32_val = tok->sint_val;
						}
					} else if (tok->type == IR_TTYPE_UNUMBER) {														// Unsigned number?
						if (ttype == IR_TYPE_INT8 || ttype == IR_TYPE_UINT8) {										// Yes, 8-bits?
							assign->val.uint8_val = (uint8_t)tok->uint_val;
						} else if (ttype == IR_TYPE_INT16 || ttype == IR_TYPE_UINT16) {								// 16-bits?
							assign->val.uint16_val = (uint16_t)tok->uint_val;
						} else {																					// 32-bits?
							assign->val.uint32_val = tok->uint_val;
						}
					} else {
						ir_free_oper(assign);																		// INVALID!!!
						free(type);
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
							   tok->char_val != ')') {																// Valid val end?
						ir_free_oper(assign);																		// ...
						free(type);
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
				}
				
				if ((tok = ir_lex_next_tok(lexer)) == NULL) {														// Try to get the next token
					ir_free_oper(assign);
					free(type);
					free(name);
					ir_free_module(module);
					ir_free_lexer(lexer);
					return NULL;
				}
			}
			
			if (tok->type != IR_TTYPE_EOS) {																		// Found semicolon at the end?
				ir_free_oper(assign);																				// No >:(
				free(type);
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			int8_t ttype = ir_type_from_string(type);																// Convert the type
			
			if (ttype == -1) {
				ir_free_oper(assign);																				// Failed :(
				free(type);
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			free(type);																								// Free the type string
			
			module->globals[currglb] = ir_create_arg(ttype, name);													// Create our global
			
			if (module->globals[currglb] == NULL) {
				free(type);																							// Failed
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			module->globals[currglb++]->assign = assign;															// Assign the assign to it...
		} else if (tok->type == IR_TTYPE_METHOD) {																	// Method declaration?
			char *name = tok->str_val;																				// Yes, let's save the name
			char *type = NULL;																						// The default type is Void
			uint32_t args = 0;																						// And by default we have 0 arguments
			uint32_t curarg = 0;
			
			if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN || tok->char_val != '(') {	// We need an open parentheses
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			uint32_t start = lexer->pos;
			
			while ((tok = ir_lex_next_tok(lexer)) != NULL) {														// Let's parse the type and the get the amount of arguments
				if (tok->type == IR_TTYPE_PAREN && tok->char_val == ')') {											// Found the end?
					break;																							// Yes :)
				} else if (tok->type == IR_TTYPE_GVARIABLE) {														// Argument?
					args++;																							// Yes!
					
					if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
						tok->char_val != '(') {																		// Now we need an open parentheses
						if (type != NULL) {																			// ...
							free(type);
						}
							
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					} else if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_IDENT) {				// Now the type of this argument
						if (type != NULL) {																			// ...
							free(type);
						}
							
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					} else if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
							   tok->char_val != ')') {																// Now we need the closing parentheses
						if (type != NULL) {																			// ...
							free(type);
						}
							
						free(name);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					tok = ir_lex_next_tok(lexer);																	// Go to the next token
				} else if (tok->type == IR_TTYPE_IDENT && type == NULL) {											// Type?
					type = tok->str_val;																			// Yes! Save it
					tok = ir_lex_next_tok(lexer);																	// And go to the next token
				}
				
				if (tok == NULL || tok->type != IR_TTYPE_SEP) {														// End?
					break;																							// Yes
				}
			}
			
			if (tok == NULL || tok->type != IR_TTYPE_PAREN || tok->char_val != ')') {								// End of file?
				if (type != NULL) {																					// Yes...
					free(type);
				}
				
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			int8_t mtype = ir_type_from_string(type == NULL ? "Void" : type);										// Get the type of this method
			
			if (mtype == -1) {
				if (type != NULL) {																					// Failed
					free(type);
				}
				
				free(name);
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			ir_method_t *method = ir_create_method(name, args, 0, mtype);											// Create the method! We don't need to set the local variable count, as the ir_check_method set it for us!
			
			free(name);																								// Free the name, we don't need it anymore
			
			if (type != NULL) {																						// And free the type, we don't need it anymore!
				free(type);
			}
			
			if (method == NULL) {
				ir_free_module(module);																				// Failed to create the method :(
				ir_free_lexer(lexer);
				return NULL;
			}
			
			lexer->pos = start;																						// Go back, time to parse the arguments and add them
			
			while ((tok = ir_lex_next_tok(lexer)) != NULL) {														// Let's parse the type and the get the amount of arguments
				if (tok->type == IR_TTYPE_PAREN && tok->char_val == ')') {											// Found the end?
					break;																							// Yes :)
				} else if (tok->type == IR_TTYPE_GVARIABLE) {														// Argument?
					name = tok->str_val;																			// Yes! Save the name
					
					if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
						tok->char_val != '(') {																		// Now we need an open parentheses
						free(name);																					// ...
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					} else if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_IDENT) {				// Now the type of this argument
						free(name);																					// ...
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					type = tok->str_val;																			// Save the type
					
					if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
							   tok->char_val != ')') {																// Now we need the closing parentheses
						free(type);																					// ...
						free(name);
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					int8_t ttype = ir_type_from_string(type);														// Try to convert the type from string to IR_TYPE_*
					
					free(type);																						// Free the type, we don't need it anymore!
					
					if (ttype == -1) {
						free(type);																					// Failed...
						free(name);
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					method->args[curarg] = ir_create_arg(ttype, name);												// Let's add this type!
					
					free(name);																						// Now we can free the type!
					
					if (method->args[curarg++] == NULL) {
						ir_free_method(method);																		// Failed to create the arg...
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					tok = ir_lex_next_tok(lexer);																	// Go to the next token
				} else if (tok->type == IR_TTYPE_IDENT) {															// Type?
					free(tok->str_val);																				// Yes, we don't need to do anything with it!
					tok = ir_lex_next_tok(lexer);
				}
				
				if (tok == NULL || tok->type != IR_TTYPE_SEP) {														// End?
					break;																							// Yes
				}
			}
			
			if (tok == NULL || tok->type != IR_TTYPE_PAREN || tok->char_val != ')') {								// End of file?
				ir_free_method(method);																				// Yes...
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			} else if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_BRACE ||
					   tok->char_val != '{') {																		// Now, we need the starting brace!
				ir_free_method(method);																				// ...
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			while ((tok = ir_lex_next_tok(lexer)) != NULL) {														// Let's parse the instructions!
				if (tok->type == IR_TTYPE_BRACE && tok->char_val == '}') {											// Found the end?
					break;																							// Yes :)
				} else if (tok->type == IR_TTYPE_IDENT) {															// Instruction?
					name = tok->str_val;																			// Yes! Save the name
					start = lexer->pos;																				// Save the current position
					
					uint32_t opers = 0;
					uint32_t curoper = 0;
					
					while ((tok = ir_lex_next_tok(lexer)) != NULL) {												// Now, let's get the amount of operands
						if (tok->type == IR_TTYPE_EOS) {															// End?
							break;																					// Yes :)
						} else if (tok->type == IR_TTYPE_TYPE) {													// Type?
							free(tok->str_val);																		// Free the type name, we don't need to for now
							
							if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||			// Yes
								tok->char_val != '(') {																// Ok, we need a opening parentheses
								free(name);																			// ...
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							} else if ((tok = ir_lex_next_tok(lexer)) == NULL ||
									   (tok->type != IR_TTYPE_SNUMBER && tok->type != IR_TTYPE_UNUMBER)) {			// Valid argument?
								free(name);																			// ...
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							opers++;																				// Increase the operand count
							
							if (tok->type == IR_TTYPE_GVARIABLE || tok->type == IR_TTYPE_STRING) {					// Free the str val?
								free(tok->str_val);																	// Yes
							}
							
							if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
								tok->char_val != ')') {																// Ok, we need a closing parentheses
								free(name);																			// ...
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else if (tok->type == IR_TTYPE_LVARIABLE || tok->type == IR_TTYPE_GVARIABLE ||
								   tok->type == IR_TTYPE_METHOD || tok->type == IR_TTYPE_STRING) {					// Valid arg?
							opers++;																				// Yes, increase the operand count
							
							if (tok->type == IR_TTYPE_GVARIABLE || tok->type == IR_TTYPE_METHOD ||
								tok->type == IR_TTYPE_STRING) {														// Free the str val?
								free(tok->str_val);																	// Yes
							}
							
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else {
							free(name);																				// Invalid...
							ir_free_method(method);
							ir_free_module(module);
							ir_free_lexer(lexer);
							return NULL;
						}
						
						if (tok == NULL || tok->type != IR_TTYPE_SEP) {												// End?
							break;																					// Yes
						}
					}
					
					if (tok == NULL || tok->type != IR_TTYPE_EOS) {													// End of file?
						free(name);																					// Yes...
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					int8_t op = ir_instr_from_string(name);															// Try to convert the instr name
					
					if (op == -1) {
						free(name);																					// Failed...
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					ir_instr_t *instr = ir_create_instr(op, opers);													// Create the instr!
					
					free(name);																						// Free the name! We don't need it anymore
					
					if (instr == NULL) {
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					lexer->pos = start;																				// Go back, time to parse the operands!
					
					while ((tok = ir_lex_next_tok(lexer)) != NULL) {
						if (tok->type == IR_TTYPE_EOS) {															// End?
							break;																					// Yes :)
						} else if (tok->type == IR_TTYPE_TYPE) {													// Type?
							int8_t ttype = ir_type_from_string(tok->str_val);										// Yes! Convert from string to IR_TYPE_*
							
							free(tok->str_val);																		// Free the str val
							
							if (ttype == -1) {
								ir_free_method(method);																// ...
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							instr->operands[curoper] = ir_create_oper(ttype);										// Let's add this operand!
							
							if (instr->operands[curoper] == NULL) {													// Failed?
								ir_free_method(method);																// Yes...
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							} else if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
								tok->char_val != '(') {																// Ok, we need a opening parentheses
								ir_free_method(method);																// ...
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							} else if ((tok = ir_lex_next_tok(lexer)) == NULL ||
									   (tok->type != IR_TTYPE_SNUMBER && tok->type != IR_TTYPE_UNUMBER)) {			// Valid argument?
								ir_free_method(method);																// ...
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							if (tok->type == IR_TTYPE_SNUMBER) {													// Signed integer?
								if (ttype == IR_TYPE_INT8 || ttype == IR_TYPE_UINT8) {								// Yes, 8-bits?
									instr->operands[curoper]->val.int8_val = (int8_t)tok->sint_val;
								} else if (ttype == IR_TYPE_INT16 || ttype == IR_TYPE_UINT16) {						// 16-bits?
									instr->operands[curoper]->val.int16_val = (int16_t)tok->sint_val;
								} else {																			// 32-bits?
									instr->operands[curoper]->val.int32_val = tok->sint_val;
								}
							} else if (tok->type == IR_TTYPE_UNUMBER) {												// Unsigned integer?
								if (ttype == IR_TYPE_INT8 || ttype == IR_TYPE_UINT8) {								// Yes, 8-bits?
									instr->operands[curoper]->val.uint8_val = (uint8_t)tok->uint_val;
								} else if (ttype == IR_TYPE_INT16 || ttype == IR_TYPE_UINT16) {						// 16-bits?
									instr->operands[curoper]->val.uint16_val = (uint16_t)tok->uint_val;
								} else {																			// 32-bits?
									instr->operands[curoper]->val.uint32_val = tok->uint_val;
								}
							} else {
								ir_free_oper(instr->operands[curoper]);												// Invalid...
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							if ((tok = ir_lex_next_tok(lexer)) == NULL || tok->type != IR_TTYPE_PAREN ||
								tok->char_val != ')') {																// Ok, now we need the closing parentheses
								ir_free_oper(instr->operands[curoper]);
								ir_free_method(method);																// ...
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							curoper++;
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else if (tok->type == IR_TTYPE_LVARIABLE) {												// Local variable?
							instr->operands[curoper] = ir_create_oper(IR_TYPE_LOCAL);								// Yes! Let's add this operand!
							
							if (instr->operands[curoper] == NULL) {													// Failed?
								ir_free_instr(instr);																// Yes...
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							instr->operands[curoper++]->val.uint32_val = tok->uint_val;								// Set the local variable index!
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else if (tok->type == IR_TTYPE_GVARIABLE) {												// Global variable?
							instr->operands[curoper] = ir_create_oper(IR_TYPE_GLOBAL);								// Yes! Let's add this operand!
							
							if (instr->operands[curoper] == NULL) {													// Failed?
								free(tok->str_val);																	// Yes...
								ir_free_instr(instr);
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							instr->operands[curoper++]->val.str_val = tok->str_val;									// Set the variable name!
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else if (tok->type == IR_TTYPE_METHOD) {													// Method
							instr->operands[curoper] = ir_create_oper(IR_TYPE_METHOD);								// Yes! Let's add this operand!
							
							if (instr->operands[curoper] == NULL) {													// Failed?
								free(tok->str_val);																	// Yes...
								ir_free_instr(instr);
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							instr->operands[curoper++]->val.str_val = tok->str_val;									// Set the method name!
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else if (tok->type == IR_TTYPE_STRING) {													// String
							instr->operands[curoper] = ir_create_oper(IR_TYPE_STRING);								// Yes! Let's add this operand!
							
							if (instr->operands[curoper] == NULL) {													// Failed?
								free(tok->str_val);																	// Yes...
								ir_free_instr(instr);
								ir_free_method(method);
								ir_free_module(module);
								ir_free_lexer(lexer);
								return NULL;
							}
							
							instr->operands[curoper++]->val.str_val = tok->str_val;									// Set the string contents!
							tok = ir_lex_next_tok(lexer);															// Go to the next token!
						} else {
							ir_free_instr(instr);																	// Invalid...
							ir_free_method(method);
							ir_free_module(module);
							ir_free_lexer(lexer);
							return NULL;
						}
						
						if (tok == NULL || tok->type != IR_TTYPE_SEP) {												// End?
							break;																					// Yes
						}
					}
					
					if (tok == NULL || tok->type != IR_TTYPE_EOS) {													// End of file?
						ir_free_instr(instr);																		// Yes...
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
					
					if (!ir_add_instr(method, instr)) {																// Try to add the instruction!
						ir_free_instr(instr);																		// ...
						ir_free_method(method);
						ir_free_module(module);
						ir_free_lexer(lexer);
						return NULL;
					}
				} else {
					ir_free_method(method);																			// Invalid...
					ir_free_module(module);
					ir_free_lexer(lexer);
					return NULL;
				}
			}
			
			if (tok == NULL || tok->type != IR_TTYPE_BRACE || tok->char_val != '}') {								// End of file?
				ir_free_method(method);																				// Yes...
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
			
			if (!ir_add_method(module, method)) {																	// Try to add this method!
				ir_free_method(method);																				// Failed...
				ir_free_module(module);
				ir_free_lexer(lexer);
				return NULL;
			}
		} else {
			if (tok->type == IR_TTYPE_STRING || tok->type == IR_TTYPE_TYPE || tok->type == IR_TTYPE_IDENT) {		// Invalid!!!!!!! Free the str val?
				free(tok->str_val);																					// Yes
			}
			
			ir_free_module(module);
			ir_free_lexer(lexer);
			return NULL;
		}
	}
	
	ir_free_lexer(lexer);																							// Free the lexer!
	
	return module;
}

char *ir_module_to_text(ir_module_t *module) {
	if (module == NULL) {																							// Check if this module is valid
		return NULL;
	}
	
	string_t *str = str_create(NULL);																				// Create the temp string
	
	if (str == NULL) {
		return NULL;																								// Failed
	}
	
	for (uint32_t i = 0; i < module->global_count; i++) {															// First, add all the globals
		ir_arg_t *global = module->globals[i];
		
		if (!str_append_formated(str, "$%s(%s)", global->name, ir_types[global->type])) {							// Append the basic definition of this global ($name(Type))
			str_free(str);																							// Failed
			return NULL;
		}
		
		if (global->assign != NULL) {
			if (global->assign->type == IR_TYPE_INT8 && global->assign->val.int8_val != 0) {						// Append Int8 value?
				if (!str_append_formated(str, " = ?Int8(%%%d)", global->assign->val.int8_val)) {					// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_INT16 && global->assign->val.int16_val != 0) {				// Append Int16 value?
				if (!str_append_formated(str, " = ?Int16(%%%d)", global->assign->val.int16_val)) {					// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_INT32 && global->assign->val.int32_val != 0) {				// Append Int32 value?
				if (!str_append_formated(str, " = ?Int32(%%%d)", global->assign->val.int32_val)) {					// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_UINT8 && global->assign->val.uint8_val != 0) {				// Append Int8 value?
				if (!str_append_formated(str, " = ?UInt8(%%%u)", global->assign->val.uint8_val)) {					// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_UINT16 && global->assign->val.uint16_val != 0) {				// Append Int16 value?
				if (!str_append_formated(str, " = ?UInt16(%%%u)", global->assign->val.uint16_val)) {				// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_UINT32 && global->assign->val.uint32_val != 0) {				// Append Int32 value?
				if (!str_append_formated(str, " = ?UInt32(%%%u)", global->assign->val.uint32_val)) {				// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_STRING && global->assign->val.str_val != NULL) {				// Append string value?
				if (!str_append_formated(str, " = #\"%s\"", global->assign->val.str_val)) {							// Yes
					str_free(str);
					return NULL;
				}
			} else if (global->assign->type == IR_TYPE_GLOBAL && global->assign->val.str_val != NULL) {				// Append global variable name?
				if (!str_append_formated(str, " = $%s", global->assign->val.str_val)) {								// Yes
					str_free(str);
					return NULL;
				}
			}
		}
		
		if (i + 1 >= module->global_count) {																		// How many new lines we need?
			if (!str_append_string(str, ";\n\n")) {																	// Two
				str_free(str);
				return NULL;
			}
		} else {
			if (!str_append_string(str, ";\n")) {																	// Only one
				str_free(str);
				return NULL;
			}
		}
	}
	
	for (uint32_t i = 0; i < module->method_count; i++) {															// Now, add all the methods!
		ir_method_t *method = module->methods[i];
		
		if (!str_append_formated(str, "@%s(%s", method->name, ir_types[method->type])) {							// Append the basic definition of this method (@name(Type))
			str_free(str);																							// Failed
			return NULL;
		}
		
		for (uint32_t j = 0; j < method->arg_count; j++) {															// Append the arguments (, $name(Type))
			ir_arg_t *arg = method->args[j];
			
			if (!str_append_formated(str, ", $%s(%s)", arg->name, ir_types[arg->type])) {							// Append this one
				str_free(str);																						// Failed
				return NULL;
			}
		}
		
		if (!str_append_formated(str, ") {%s", method->body_count == 0 ? " " : "\n")) {								// Append the end
			str_free(str);																							// Failed
			return NULL;
		}
		
		for (uint32_t j = 0; j < method->body_count; j++) {															// Append the body
			ir_instr_t *instr = method->body[j];
			
			if (!str_append_formated(str, "\t%s", ir_instrs[instr->op])) {											// Append the name of this instr
				str_free(str);
				return NULL;
			}
			
			for (uint32_t k = 0; k < instr->operand_count; k++) {													// Append the operands
				ir_oper_t *oper = instr->operands[k];
				
				if (!str_append_formated(str, "%s ", k == 0 ? "" : ",")) {											// Add a comma if k != 0
					str_free(str);																					// Failed
					return NULL;
				} else if (oper->type == IR_TYPE_INT8) {															// Append Int8 value?
					if (!str_append_formated(str, "?Int8(%%%d)", oper->val.int8_val)) {								// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_INT16) {															// Append Int16 value?
					if (!str_append_formated(str, "?Int16(%%%d)", oper->val.int16_val)) {							// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_INT32) {															// Append Int32 value?
					if (!str_append_formated(str, "?Int32(%%%d)", oper->val.int32_val)) {							// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_UINT8) {															// Append Int8 value?
					if (!str_append_formated(str, "?UInt8(%%%d)", oper->val.uint8_val)) {							// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_UINT16) {															// Append Int16 value?
					if (!str_append_formated(str, "?UInt16(%%%d)", oper->val.uint16_val)) {							// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_UINT32) {															// Append Int32 value?
					if (!str_append_formated(str, "?UInt32(%%%d)", oper->val.uint32_val)) {							// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_STRING) {															// Append string value?
					if (!str_append_formated(str, "#\"%s\"", oper->val.str_val)) {									// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_LOCAL) {															// Append local variable index?
					if (!str_append_formated(str, "$%u", oper->val.uint32_val)) {									// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_GLOBAL) {															// Append global variable name?
					if (!str_append_formated(str, "$%s", oper->val.str_val)) {										// Yes
						str_free(str);
						return NULL;
					}
				} else if (oper->type == IR_TYPE_METHOD) {															// Append method name?
					if (!str_append_formated(str, "@%s", oper->val.str_val)) {										// Yes
						str_free(str);
						return NULL;
					}
				}
			}
			
			if (!str_append_formated(str, ";\n")) {																	// Go to the next line
				str_free(str);
				return NULL;
			}
		}
		
		if (!str_append_string(str, "}\n\n")) {																		// Go to the line
			str_free(str);
			return NULL;
		}
	}
	
	char *out = str->string;																						// Save the output
	str->string = NULL;																								// Set str->string to NULL, so str_free will not free it
	
	str_free(str);																									// Free the str struct
	
	return out;
}

void ir_free_module(ir_module_t *module) {
	if (module != NULL) {																							// Valid?
		for (uint32_t i = 0; i < module->method_count; i++) {														// Yes, free the methods
			ir_free_method(module->methods[i]);
		}
		
		for (uint32_t i = 0; i < module->global_count; i++) {														// Free the globals
			ir_free_arg(module->globals[i]);
		}
		
		free(module);																								// And the module struct itself
	}
}

static int ir_search_global(ir_module_t *module, char *name) {
	int found = 0;
	
	for (uint32_t i = 0; i < module->global_count; i++) {															// Let's search!
		ir_arg_t *g = module->globals[i];
		
		if ((strlen(g->name) == strlen(name)) && !strcmp(g->name, name)) {											// Found?
			if (found) {																							// Yes, skip it?
				return 1;																							// No...
			} else {
				found = 1;																							// Yes :)
			}
		}
	}
	
	return 0;																										// Not found!
}

int ir_check_module(ir_module_t *module) {
	if (module != NULL) {																							// Sanity check
		for (uint32_t i = 0; i < module->global_count; i++) {														// Check the globals
			if (ir_search_global(module, module->globals[i]->name)) {
				return 0;
			}
		}
		
		for (uint32_t i = 0; i < module->method_count; i++) {														// And check the methods
			if (!ir_check_method(module, module->methods[i])) {
				return 0;
			}
		}
	}
	
	return 1;
}

int ir_add_method(ir_module_t *module, ir_method_t *method) {
	if (module == NULL || method == NULL) {																			// Check if our arguments are valid
		return 0;																									// ...
	}
	
	ir_method_t **methods = realloc(module->methods, (module->method_count + 1) * sizeof(ir_method_t));				// Realloc the method array
	
	if (methods == NULL) {
		return 0;																									// Failed..
	}
	
	methods[module->method_count] = method;																			// Add this method
	module->methods = methods;																						// Set the new method array
	module->method_count++;																							// And increase the method count
	
	return 1;
}
