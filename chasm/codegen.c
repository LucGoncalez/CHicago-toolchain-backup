// File author is Ítalo Lima Marconato Matias
//
// Created on December 28 of 2018, at 17:15 BRT
// Last edited on January 14 of 2019, at 14:19 BRT

#include <arch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

codegen_t *codegen_new(node_t *ast) {
	if (ast == NULL) {																							// Null pointer check
		return NULL;
	}
	
	codegen_t *codegen = calloc(1, sizeof(codegen_t));															// Alloc space
	
	if (codegen != NULL) {																						// Failed?
		codegen->ast = ast;																						// No! Set the ast
	}
	
	return codegen;
}

void codegen_free(codegen_t *codegen) {
	if (codegen != NULL) {																						// Check if our argument is valid
		for (codegen_section_t *sect = codegen->sections, *next; sect != NULL; sect = next) {					// Let's free the sections
			next = sect->next;																					// Set the next entry
			free(sect->data);																					// Free the data
			free(sect);																							// Free the section struct
		}
		
		for (codegen_label_t *lbl = codegen->labels, *next; lbl != NULL; lbl = next) {							// Let's free the labels
			next = lbl->next;																					// Set the next entry
			free(lbl);																							// Free the label struct
		}
		
		for (codegen_reloc_t *rel = codegen->relocs, *next; rel != NULL; rel = next) {							// Let's free the relocations
			next = rel->next;																					// Set the next entry
			free(rel);																							// Free the reloc struct
		}
		
		node_free_list(codegen->ast);																			// Free the ast
		free(codegen);																							// And the codegen struct!
	}
}

void codegen_write_byte(codegen_t *codegen, uint8_t data) {
	if (codegen == NULL || codegen->current_section == NULL) {													// Null pointer checks
		return;
	} else if (codegen->current_section->size >= codegen->current_section->max) {								// Alloc more data memory?
		uint8_t *new = realloc(codegen->current_section->data, codegen->current_section->max * 2);				// Yes!
		
		if (new == NULL) {
			return;																								// Failed...
		}
		
		codegen->current_section->data = new;																	// Set the new data
		codegen->current_section->max *= 2;																		// And increase the max capacity
	}
	
	codegen->current_section->data[codegen->current_section->size++] = data;									// Write the byte!
}

void codegen_write_word(codegen_t *codegen, uint16_t data) {
	if (codegen == NULL || codegen->current_section == NULL) {													// Null pointer checks
		return;
	}
	
	codegen_write_byte(codegen, (uint8_t)data);																	// Call codegen_write_byte to write all the 2 bytes
	codegen_write_byte(codegen, (uint8_t)(data >> 8));
}

void codegen_write_dword(codegen_t *codegen, uint32_t data) {
	if (codegen == NULL || codegen->current_section == NULL) {													// Null pointer checks
		return;
	}
	
	codegen_write_byte(codegen, (uint8_t)data);																	// Call codegen_write_byte to write all the 4 bytes
	codegen_write_byte(codegen, (uint8_t)(data >> 8));
	codegen_write_byte(codegen, (uint8_t)(data >> 16));
	codegen_write_byte(codegen, (uint8_t)(data >> 24));
}

void codegen_write_qword(codegen_t *codegen, uint64_t data) {
	if (codegen == NULL || codegen->current_section == NULL) {													// Null pointer checks
		return;
	}
	
	codegen_write_byte(codegen, (uint8_t)data);																	// Call codegen_write_byte to write all the 8 bytes
	codegen_write_byte(codegen, (uint8_t)(data >> 8));
	codegen_write_byte(codegen, (uint8_t)(data >> 16));
	codegen_write_byte(codegen, (uint8_t)(data >> 24));
	codegen_write_byte(codegen, (uint8_t)(data >> 32));
	codegen_write_byte(codegen, (uint8_t)(data >> 40));
	codegen_write_byte(codegen, (uint8_t)(data >> 48));
	codegen_write_byte(codegen, (uint8_t)(data >> 56));
}

void codegen_add_relocation(codegen_t *codegen, char *name, char *sect, uint8_t size, uintptr_t loc, int inc, int rel) {
	if (codegen == NULL || sect == NULL || size == 0) {															// Null pointer check
		return;
	}
	
	if (name != NULL && !codegen_have_label(codegen, name)) {													// This symbol is already "defined"?
		codegen_add_label(codegen, name, CODEGEN_LABEL_EXTERN, 0);												// Nope, define it
	}
	
	codegen_reloc_t *cur = codegen->relocs;																		// Let's get the last entry
	
	for (; cur != NULL && cur->next != NULL; cur = cur->next) ;
	
	if (cur != NULL) {																							// First relocation?
		cur->next = calloc(1, sizeof(codegen_reloc_t));															// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(codegen_reloc_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		return;																									// Failed to alloc
	}
	
	cur->name = name;																							// Ok, set everything!
	cur->sect = sect;
	cur->size = size;
	cur->loc = loc;
	cur->increment = inc;
	cur->relative = rel;
	
	if (codegen->relocs == NULL) {																				// First entry?
		codegen->relocs = cur;																					// Yeah
	}
}

void codegen_select_section(codegen_t *codegen, char *name) {
	if (codegen == NULL || name == NULL) {																		// Null pointer check
		return;
	}
	
	codegen_section_t *cur = codegen->sections;																	// Let's search!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			codegen->current_section = cur;																		// Yes, set it
			return;
		} else if (cur->next == NULL) {																			// We need to create this section?
			break;																								// Yes
		}
	}
	
	if (cur != NULL) {																							// First section?
		cur->next = calloc(1, sizeof(codegen_section_t));														// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(codegen_section_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		return;																									// Failed to alloc
	}
	
	cur->name = name;																							// Set the name
	cur->max = 1;																								// Set the default max
	cur->data = malloc(cur->max);																				// And (try to) alloc it
	
	if (codegen->sections == NULL) {																			// First entry?
		codegen->sections = cur;																				// Yeah
	}
	
	codegen->current_section = cur;																				// Set it as the current section
}

uintptr_t codegen_get_section_start(codegen_t *codegen, char *name) {
	if (codegen == NULL || name == NULL) {																		// Null pointer check
		return 0;
	}
	
	uintptr_t start = 0;
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {							// Let's search for the section
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			return start;																						// Yes!
		}
		
		start += cur->size;																						// Increase the start/base
	}
	
	return 0;																									// Not found...
}

void codegen_add_label(codegen_t *codegen, char *name, uint8_t type, uintptr_t loc) {
	if (codegen == NULL || name == NULL) {																		// Null pointer check
		return;
	} else if (codegen_have_label(codegen, name)) {																// Duplicated label?
		return;																									// Yes >:(
	}
	
	codegen_label_t *cur = codegen->labels;																		// Let's get the last entry
	
	for (; cur != NULL && cur->next != NULL; cur = cur->next) ;
	
	if (cur != NULL) {																							// First label?
		cur->next = calloc(1, sizeof(codegen_label_t));															// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(codegen_label_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		return;																									// Failed to alloc
	}
	
	cur->sect = codegen->current_section != NULL ? codegen->current_section->name : NULL;						// Fill in the sectio name, the name, type and location
	cur->name = name;
	cur->type = type;
	cur->loc = loc;
	
	if (cur->type == CODEGEN_LABEL_LOCAL || cur->type == CODEGEN_LABEL_EXTERN) {								// This one is already resolved?
		cur->local_resolved = 1;																				// Yeah
	}
	
	if (codegen->labels == NULL) {																				// First entry?
		codegen->labels = cur;																					// Yeah
	}
}

codegen_label_t *codegen_get_label(codegen_t *codegen, char *name) {
	if (codegen == NULL || name == NULL) {																		// Null pointer check
		return NULL;
	}
	
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {								// Let's search!
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			return cur;																							// Yes!
		}
	}
	
	return NULL;																								// Nope :(
}

int codegen_have_label(codegen_t *codegen, char *name) {
	if (codegen == NULL || name == NULL) {																		// Null pointer check
		return 0;
	}
	
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {								// Let's search!
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			return 1;																							// Yes!
		}
	}
	
	return 0;																									// Nope :(
}

int codegen_gen(codegen_t *codegen) {
	if (codegen == NULL) {																						// Null pointer check
		return 0;
	}
	
	codegen_select_section(codegen, ".text");																	// Select the .code section
	
	for (node_t *node = codegen->ast; node != NULL; node = node->next) {										// Let's generate everything!
		if (node->type == NODE_TYPE_SECTION_DIRECTIVE) {														// Change the current section?
			codegen_select_section(codegen, ((section_directive_node_t*)node)->name);							// Yes!
		} else if (node->type == NODE_TYPE_GLOBAL_DIRECTIVE) {													// Make a symbol global?
			codegen_label_t *lbl = codegen_get_label(codegen, ((global_directive_node_t*)node)->name);			// Yes, try to get it
			
			if (lbl == NULL) {																					// Doesn't exists?
				codegen_add_label(codegen, ((global_directive_node_t*)node)->name, CODEGEN_LABEL_GLOBAL, 0);	// So let's create it!
			} else if (lbl->type == CODEGEN_LABEL_LOCAL) {														// We can make it global?
				lbl->type = CODEGEN_LABEL_GLOBAL;																// Yes!
			}
		} else if (node->type == NODE_TYPE_EXTERN_DIRECTIVE) {													// Import external symbol?
			if (codegen_get_label(codegen, ((extern_directive_node_t*)node)->name) == NULL) {					// Yes, it already exists?
				codegen_add_label(codegen, ((extern_directive_node_t*)node)->name, CODEGEN_LABEL_EXTERN, 0);	// Nope, so let's add it!
			}
		} else if (node->type == NODE_TYPE_DEFINE_DIRECTIVE) {													// Define byte/word/dword/qword?
			define_directive_node_t *def = (define_directive_node_t*)node;
			
			if (node->childs->type == NODE_TYPE_IDENTIFIER) {													// Put the address of a symbol?
				identifier_node_t *sym = (identifier_node_t*)node->childs;										// Yeah, add a relocation, as we don't know the right section of this symbol
				uintptr_t loc = codegen->current_section->size;													// Save the location of the relocation
				
				if (def->size == 1) {																			// Write zero to it (for now)
					codegen_write_byte(codegen, 0);
				} else if (def->size == 2) {
					codegen_write_word(codegen, 0);
				} else if (def->size == 4) {
					codegen_write_dword(codegen, 0);
				} else if (def->size == 8) {
					codegen_write_qword(codegen, 0);
				}
				
				codegen_add_relocation(codegen, sym->value, codegen->current_section->name, def->size, loc, 0,
									   0);																		// And add the relocation!
			} else if (node->childs->type == NODE_TYPE_NUMBER) {												// Put a integer?
				number_node_t *num = (number_node_t*)node->childs;												// Yeah!
				
				if (def->size == 1) {																			// Byte?
					if (num->value > UINT8_MAX) {																// Yes, we're going to truncate the data?
						printf("warning: value truncated to fit in a byte\n");									// Yes, warning the user
					}
					
					codegen_write_byte(codegen, (uint8_t)num->value);											// Write!
				} else if (def->size == 2) {																	// Word?
					if (num->value > UINT16_MAX) {																// Yes, we're going to truncate the data?
						printf("warning: value truncated to fit in a word\n");									// Yes, warning the user
					}
					
					codegen_write_word(codegen, (uint16_t)num->value);											// Write!
				} else if (def->size == 4) {																	// DWord?
					if (num->value > UINT32_MAX) {																// Yes, we're going to truncate the data?
						printf("warning: value truncated to fit in a dword\n");									// Yes, warning the user
					}
					
					codegen_write_dword(codegen, (uint32_t)num->value);											// Write!
				} else if (def->size == 8) {																	// QWord?
					if (num->value > UINT64_MAX) {																// Yes, we're going to truncate the data?
						printf("warning: value truncated to fit in a qword\n");									// Yes, warning the user
					}
					
					codegen_write_qword(codegen, (uint64_t)num->value);											// Write!
				} else if (def->size == 10) {																	// Extended precision floating point?
					codegen_write_qword(codegen, num->value);													// Yes, write!
					codegen_write_word(codegen, 0);
				}
			} else if (node->childs->type == NODE_TYPE_STRING) {												// Add all the bytes from the string?
				char *str = ((string_node_t*)node->childs)->value;												// Yes
				
				for (size_t i = 0; i < strlen(str); i++) {														// Write!
					codegen_write_byte(codegen, str[i]);
				}
			} else if (node->childs->type == NODE_TYPE_FLOAT) {													// Put a floating point number?
				float_node_t *num = (float_node_t*)node->childs;												// Yeah!
				
				if (def->size == 1) {																			// Minifloat?
					uint8_t value = 0;																			// Yes, let's mount the byte!
					int man = ((int)(num->value * 100) % 100);													// Get the mantissa
					int exp = abs((int)num->value);																// And the exponent
					
					if (num->value < 0) {																		// Set the sign bit?
						value |= (1 << 7);																		// Yes
					}
					
					value |= ((exp & 0xF) << 3) | (man & 0x07);													// Set the exponent and the mantissa
					
					codegen_write_byte(codegen, value);															// Write!
				} else if (def->size == 2) {																	// Half-precision?
					uint16_t value = 0;																			// Yes, let's mount the word!
					int man = ((int)(num->value * 1000) % 1000);												// Get the mantissa
					int exp = abs((int)num->value);																// And the exponent
					
					if (num->value < 0) {																		// Set the sign bit?
						value |= (1 << 15);																		// Yes
					}
					
					value |= ((exp & 0x1F) << 10) | (man & 0x3FF);												// Set the exponent and the mantissa
					
					codegen_write_word(codegen, value);															// Write!
				} else if (def->size == 4) {																	// Single-precision?
					float fvalue = (float)num->value;															// Yes, convert the long double to float
					uint32_t value = 0;
					
					memcpy(&value, &fvalue, 4);																	// The float to uint32_t
					codegen_write_dword(codegen, value);														// And write it!
				} else if (def->size == 8) {																	// Double-precision?
					double fvalue = (double)num->value;															// Yes, convert the long double to double
					uint64_t value = 0;
					
					memcpy(&value, &fvalue, 8);																	// The double to uint64_t
					codegen_write_qword(codegen, value);														// And write it!
				} else if (def->size == 10) {																	// Extended-precision?
					uint64_t value1 = 0;																		// Yes
					uint16_t value2 = 0;
					
					memcpy(&value1, &num->value, 8);															// First, convert the first 8 bytes to uint64_t
					memcpy(&value2, ((void*)&num->value) + 8, 2);												// And the last 2 bytes to uint16_t
					codegen_write_qword(codegen, value1);														// Now write everything!
					codegen_write_word(codegen, value2);
				}
			}
		} else if (node->type == NODE_TYPE_LABEL) {																// Create label?
			codegen_label_t *lbl = codegen_get_label(codegen, ((label_node_t*)node)->name);						// Yes, first, let's make sure we're not redefining it
			uintptr_t loc = codegen->current_section->size;
			
			if (lbl == NULL) {																					// It doesn't exists?
				codegen_add_label(codegen, ((label_node_t*)node)->name, CODEGEN_LABEL_LOCAL, loc);				// So let's add it!
			} else if (!lbl->local_resolved || lbl->type == CODEGEN_LABEL_EXTERN) {								// It's redefinition?
				lbl->sect = codegen->current_section->name;														// Nope, so let's set everything!
				lbl->local_resolved = 1;			
				lbl->loc = loc;
				
				if (lbl->type != CODEGEN_LABEL_GLOBAL) {
					lbl->type = CODEGEN_LABEL_LOCAL;
				}
			} else {
				printf("redefinition of '%s'\n", ((label_node_t*)node)->name);									// Redefinition...
				return 1;
			}
		} else {
			int err = arch_gen(codegen, node);																	// Well, i hope thats a arch-specific node
			
			if (err == -1) {
				return 1;																						// Failed...
			} else if (err == 0) {
				printf("invalid ntype %d\n", node->type);														// Invalid!
				return 0;
			}
		}
	}
	
	return 1;
}
