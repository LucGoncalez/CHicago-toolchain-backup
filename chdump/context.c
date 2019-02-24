// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:48 BRT
// Last edited on February 24 of 2019, at 15:36 BRT

#include <context.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

context_t *context_new() {
	return calloc(1, sizeof(context_t));																		// Alloc space and return it
}

void context_free(context_t *context) {
	if (context != NULL) {																						// Check if our argument is valid
		for (context_section_t *sect = context->sections, *next; sect != NULL; sect = next) {					// Let's free the sections
			next = sect->next;																					// Set the next entry
			free(sect->name);																					// Free the section name
			free(sect);																							// Free the section struct
		}
		
		for (context_symbol_t *sym = context->symbols, *next; sym != NULL; sym = next) {						// Let's free the symbols
			next = sym->next;																					// Set the next entry
			free(sym->sect);																					// Free the symbol section name
			free(sym->name);																					// Free the symbol name
			free(sym);																							// Free the symbol struct
		}
		
		for (context_reloc_t *rel = context->relocs, *next; rel != NULL; rel = next) {							// Let's free the relocations
			next = rel->next;																					// Set the next entry
			
			if (rel->name != NULL) {																			// Free the reloc sym name?
				free(rel->name);																				// Yes
			}
			
			free(rel->sect);																					// Free the reloc sect name
			free(rel);																							// Free the reloc struct
		}
		
		free(context);																							// And the context struct!
	}
}

void context_add_section(context_t *context, char *name, uintptr_t size, uintptr_t virt, uintptr_t off, uint8_t *data) {
	if (context == NULL || name == NULL || (size > 0 && data == NULL)) {										// Null pointer check
		return;
	}
	
	context_section_t *cur = context->sections;																	// Let's search!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			free(name);
			return;																								// Yes, so it already exists
		} else if (cur->next == NULL) {																			// We need to create this section?
			break;																								// Yes
		}
	}
	
	if (cur != NULL) {																							// First section?
		cur->next = calloc(1, sizeof(context_section_t));														// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_section_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		free(name);
		return;																									// Failed to alloc
	}
	
	cur->name = name;																							// Set the name
	cur->size = size;																							// Set the size
	cur->virt = virt;																							// Set the virtual address
	cur->off = off;																								// Set the offset of the section in the file
	cur->data = data;																							// And the data
	
	if (context->sections == NULL) {																			// First entry?
		context->sections = cur;																				// Yeah
	}
}

void context_add_symbol(context_t *context, char *name, char *sect, uint8_t type, uintptr_t loc) {
	if (context == NULL || name == NULL || sect == NULL) {														// Null pointer check
		return;
	}
	
	context_symbol_t *cur = context->symbols;																	// Let's search!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			free(sect);
			free(name);
			return;																								// Yes, so it already exists
		} else if (cur->next == NULL) {																			// We need to create this section?
			break;																								// Yes
		}
	}
	
	for (; cur != NULL && cur->next != NULL; cur = cur->next) ;
	
	if (cur != NULL) {																							// First label?
		cur->next = calloc(1, sizeof(context_symbol_t));														// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_symbol_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		free(sect);
		free(name);
		return;																									// Failed to alloc
	}
	
	cur->sect = sect;																							// Fill in the section name, the name, type and location
	cur->name = name;
	cur->type = type;
	cur->loc = loc;
	
	if (context->symbols == NULL) {																				// First entry?
		context->symbols = cur;																					// Yeah
	}
}

void context_add_relocation(context_t *context, char *name, char *sect, uint8_t size, uintptr_t loc, int inc, int rel) {
	if (context == NULL || sect == NULL || size == 0) {															// Null pointer check
		return;
	}
	
	context_reloc_t *cur = context->relocs;																		// Let's get the last entry
	
	for (; cur != NULL && cur->next != NULL; cur = cur->next) ;
	
	if (cur != NULL) {																							// First relocation?
		cur->next = calloc(1, sizeof(context_reloc_t));															// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_reloc_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		if (name != NULL) {
			free(name);
		}
		
		free(sect);
		
		return;																									// Failed to alloc
	}
	
	cur->name = name;																							// Ok, set everything!
	cur->sect = sect;
	cur->size = size;
	cur->loc = loc;
	cur->increment = inc;
	cur->relative = rel;
	
	if (context->relocs == NULL) {																				// First entry?
		context->relocs = cur;																					// Yeah
	}
}
