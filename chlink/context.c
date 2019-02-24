// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:48 BRT
// Last edited on February 24 of 2019, at 15:40 BRT

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
			free(sect->data);																					// Free the data
			free(sect->name);																					// Free the name
			free(sect);																							// Free the section struct
		}
		
		for (context_symbol_t *sym = context->symbols, *next; sym != NULL; sym = next) {						// Let's free the symbols
			next = sym->next;																					// Set the next entry
			free(sym->sect);																					// Free the sect
			free(sym->name);																					// Free the name
			free(sym);																							// Free the symbol struct
		}
		
		for (context_reloc_t *rel = context->relocs, *next; rel != NULL; rel = next) {							// Let's free the relocations
			next = rel->next;																					// Set the next entry
			
			if (rel->name != NULL) {																			// Free the name?
				free(rel->name);																				// Yes
			}
			
			free(rel->sect);																					// Free the sect
			free(rel);																							// Free the reloc struct
		}
		
		for (context_dep_t *dep = context->deps, *next; dep != NULL; dep = next) {								// Let's free the dependencies
			next = dep->next;																					// Set the next entry
			
			for (context_dep_sym_t *sym = dep->syms, *nexts; sym != NULL; sym = nexts) {						// Free the symbols
				nexts = sym->next;																				// Set the next entry
				free(sym->name);																				// Free the name
				free(sym);																						// Free the dep sym struct
			}
			
			free(dep->name);																					// Free the name
			free(dep);																							// Free the dep struct
		}
		
		free(context);																							// And the context struct!
	}
}

void context_add_section(context_t *context, char *name, uintptr_t size, uintptr_t virt, uint8_t *data) {
	if (context == NULL || name == NULL || (size > 0 && data == NULL)) {										// Null pointer check
		return;
	}
	
	context_section_t *cur = context->sections;																	// Let's search!
	context_section_t *last = NULL;
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			uint8_t *new = realloc(cur->data, cur->size + size);												// Yes, append!
			
			if (new != NULL) {
				memcpy((char*)(new + cur->size), (char*)data, size);
				cur->data = new;																				// Set the new data pointer
				cur->size += size;																				// And set the new size
			}
			
			free(name);
			return;
		} else if (cur->next == NULL) {																			// We need to create this section?
			break;																								// Yes
		}
	}
	
	if (cur != NULL) {																							// First section?
		cur->next = calloc(1, sizeof(context_section_t));														// Nope, alloc it and save as the last entry
		last = cur;
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_section_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		free(name);
		return;																									// Failed to alloc
	}
	
	cur->name = strdup(name);																					// Set the name
	cur->size = size;																							// Set the size
	cur->virt = virt;																							// Set the virtual address
	
	if (cur->name == NULL) {
		if (last != NULL) {																						// Failed to strdup the name...
			last->next = NULL;
		}
		
		free(cur);
		free(name);
		
		return;
	}
	
	if (size > 0) {
		cur->data = malloc(size);																				// And the data

		if (cur->data == NULL) {
			if (last != NULL) {																					// Failed :(
				last->next = NULL;
			}
			
			free(cur->name);
			free(cur);
			free(name);
			
			return;
		}

		memcpy((char*)cur->data, (char*)data, size);
	}
	
	if (context->sections == NULL) {																			// First entry?
		context->sections = cur;																				// Yeah
	}
}

int context_add_symbol(context_t *context, char *name, char *sect, uint8_t type, uintptr_t loc) {
	if (context == NULL || name == NULL || sect == NULL) {														// Null pointer check
		return 0;
	}
	
	context_symbol_t *cur = context->symbols;																	// Let's search!
	context_symbol_t *last = NULL;
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			if (cur->type == CONTEXT_SYMBOL_EXTERN && type == CONTEXT_SYMBOL_GLOBAL) {							// Resolving external?
				cur->sect = sect;																				// Yes :)
				cur->type = CONTEXT_SYMBOL_GLOBAL;
				cur->loc = loc;
				
				free(name);
				free(sect);
				
				return 1;
			} else if (cur->type == CONTEXT_SYMBOL_EXTERN || type == CONTEXT_SYMBOL_EXTERN) {
				free(name);
				free(sect);
				return 1;
			} else {
				printf("Error: redefinition of '%s' at section '%s'\n", name, sect);							// Redefinition :(
				free(name);
				free(sect);
				return 0;
			}
		} else if (cur->next == NULL) {																			// We need to create this section?
			break;																								// Yes
		}
	}
	
	for (; cur != NULL && cur->next != NULL; cur = cur->next) ;
	
	if (cur != NULL) {																							// First label?
		cur->next = calloc(1, sizeof(context_symbol_t));														// Nope, alloc it and save as the last entry
		last = cur;
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_symbol_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		free(name);
		free(sect);
		return 0;																								// Failed to alloc
	}
	
	cur->sect = strdup(sect);																					// Fill in the section name, the name, type and location
	cur->name = strdup(name);
	cur->type = type;
	cur->loc = loc;
	
	if (cur->sect == NULL || cur->name == NULL) {
		if (last != NULL) {																						// Failed to strdup the name (or sect)...
			last->next = NULL;
		}
		
		if (cur->sect != NULL) {
			free(cur->sect);
		}
		
		if (cur->name != NULL) {
			free(cur->name);
		}
		
		free(name);
		free(sect);
		free(cur);
		
		return 0;
	}
	
	if (context->symbols == NULL) {																				// First entry?
		context->symbols = cur;																					// Yeah
	}
	
	return 1;
}

void context_add_relocation(context_t *context, char *name, char *sect, uint8_t size, uintptr_t loc, int inc, int rel) {
	if (context == NULL || sect == NULL || size == 0) {															// Null pointer check
		return;
	}
	
	context_reloc_t *cur = context->relocs;																		// Let's get the last entry
	context_reloc_t *last = NULL;
	
	for (; cur != NULL && cur->next != NULL; cur = cur->next) ;
	
	if (cur != NULL) {																							// First relocation?
		cur->next = calloc(1, sizeof(context_reloc_t));															// Nope, alloc it and save as the last entry
		last = cur;
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
	
	if (name != NULL) {																							// Ok, set everything!
		cur->name = strdup(name);
	}
	
	cur->sect = strdup(sect);
	cur->size = size;
	cur->loc = loc;
	cur->increment = inc;
	cur->relative = rel;
	
	if (cur->sect == NULL || (name != NULL && cur->name == NULL)) {
		if (last != NULL) {																						// Failed to strdup the sect (or name)...
			last->next = NULL;
		}
		
		if (cur->sect != NULL) {
			free(cur->sect);
		}
		
		if (cur->name != NULL) {
			free(cur->name);
		}
		
		if (name != NULL) {
			free(name);
		}
		
		free(sect);
		free(cur);
		
		return;
	}
	
	if (context->relocs == NULL) {																				// First entry?
		context->relocs = cur;																					// Yeah
	}
}

void context_add_dep(context_t *context, char *name) {
	if (context == NULL || name == NULL) {																		// Null pointer check
		return;
	}
	
	context_dep_t *cur = context->deps;																			// Let's search!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			free(name);
			return;
		} else if (cur->next == NULL) {																			// We need to create this dep?
			break;																								// Yes
		}
	}
	
	if (cur != NULL) {																							// First dep?
		cur->next = calloc(1, sizeof(context_dep_t));															// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_dep_t));																	// Yes, alloc it
	}
	
	if (cur == NULL) {
		free(name);
		return;																									// Failed to alloc
	}
	
	cur->name = name;																							// Ok, set the name
	
	if (context->deps == NULL) {																				// First entry?
		context->deps = cur;																					// Yeah
	}
}

int context_add_dep_sym(context_t *context, char *dep, char *name) {
	if (context == NULL || dep == NULL || name == NULL) {														// Null pointer checks
		return 0;
	}
	
	context_dep_t *deps = context->deps;																		// Let's search!
	
	for (; deps != NULL; deps = deps->next) {
		if ((strlen(name) == strlen(deps->name)) && !strcmp(name, deps->name)) {								// Found?
			break;
		}
	}
	
	if (deps == NULL) {
		free(name);
		return 0;																								// Not found :(
	}
	
	context_dep_sym_t *cur = deps->syms;																		// Let's search!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {									// Found?
			printf("Error: redefinition of '%s'\n", name);														// Redefinition :(
			free(name);
			return 0;
		} else if (cur->next == NULL) {																			// We need to create this sym?
			break;																								// Yes
		}
	}
	
	if (cur != NULL) {																							// First sym?
		cur->next = calloc(1, sizeof(context_dep_sym_t));														// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(context_dep_sym_t));																// Yes, alloc it
	}
	
	if (cur == NULL) {
		free(name);
		return 0;																								// Failed to alloc
	}
	
	cur->name = name;																							// Ok, set the name
	
	if (deps->syms == NULL) {																					// First entry?
		deps->syms = cur;																						// Yeah
	}
	
	return 1;
}

static context_symbol_t *context_get_sym(context_t *context, char *name) {
	for (context_symbol_t *sym = context->symbols; sym != NULL; sym = sym->next) {								// Let's search
		if ((strlen(sym->name) == strlen(name)) && !strcmp(sym->name, name)) {									// Found?
			return sym;																							// YES :)
		}
	}
	
	return NULL;
}

static context_section_t *context_get_sect(context_t *context, char *name) {
	for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {						// Let's search
		if ((strlen(sect->name) == strlen(name)) && !strcmp(sect->name, name)) {								// Found?
			return sect;																						// YES :)
		}
	}
	
	return NULL;
}

static uintptr_t context_get_sect_loc(context_t *dst, context_t *src, char *name) {
	context_section_t *dsect = context_get_sect(dst, name);														// Get the dest section
	context_section_t *ssect = context_get_sect(src, name);														// Get the source section
	
	if (dsect == NULL || ssect == NULL) {
		return 0;																								// Doesn't exists yet!
	} else if (dsect->last != (uintptr_t)-1) {
		return dsect->last;
	}
	
	dsect->last = dsect->size;
	
	return dsect->size;
}

static uintptr_t context_get_virt(context_t *context, char *name) {
	if (context_get_sect(context, name)) {																		// Already exists?
		return context_get_sect(context, name)->virt;															// Yes
	} else if (context->sections == NULL) {
		return 0;
	}
	
	context_section_t *sect = context->sections;
	
	for (; sect->next != NULL; sect = sect->next) ;
	
	return sect->virt + sect->size;
}

uintptr_t context_get_section_start(context_t *context, char *name) {
	if (context == NULL || name == NULL) {																		// Sanity checks
		return 0;
	}
	
	context_section_t *sect = context_get_sect(context, name);													// Try to get the section
	
	return sect != NULL ? sect->virt : 0;																		// And return the virt address
}

int context_merge(context_t *dst, context_t *src) {
	if (dst == NULL || src == NULL) {																			// Sanity checks
		return 0;
	} else if ((dst->arch != NULL && src->arch != NULL) &&
			   ((strlen(dst->arch) != strlen(src->arch) || strcmp(dst->arch, src->arch)))) {					// ...
		printf("Error: trying to link file from arch '%s' with file from arch '%s'\n", dst->arch, src->arch);
		return 0;
	}
	
	if (dst->arch == NULL) {																					// Set the arch (if required)
		dst->arch = src->arch;
	}
	
	for (context_section_t *sect = dst->sections; sect != NULL; sect = sect->next) {							// Clear the ->last
		sect->last = -1;
	}
	
	for (context_section_t *sect = src->sections; sect != NULL; sect = sect->next) {
		sect->last = -1;
	}
	
	for (context_symbol_t *sym = src->symbols; sym != NULL; sym = sym->next) {									// First, add the symbols
		if (sym->type != CONTEXT_SYMBOL_LOCAL) {																// Local?
			if (!context_add_symbol(dst, sym->name, sym->sect, sym->type,
								    sym->loc + context_get_sect_loc(dst, src, sym->sect))) {					// Nope, add it
				return 0;
			}
		}
	}
	
	for (context_section_t *sect = src->sections; sect != NULL; sect = sect->next) {							// Now, add/merge the sections
		context_add_section(dst, sect->name, sect->size, context_get_virt(dst, sect->name), sect->data);
	}
	
	for (context_reloc_t *rel = src->relocs; rel != NULL; rel = rel->next) {									// Now, add the relocations
		uintptr_t loc = context_get_sect_loc(dst, src, rel->sect);
		
		if (rel->name == NULL) {																				// Based on a symbol?
			context_add_relocation(dst, NULL, rel->sect, rel->size, rel->loc + loc,
								   rel->increment + loc, rel->relative);										// Nope
		} else {
			context_symbol_t *sym = context_get_sym(src, rel->name);											// Yes
			
			if (sym != NULL && sym->type == CONTEXT_SYMBOL_LOCAL) {												// Local symbol?
				context_section_t *sect = context_get_sect(dst, sym->sect);										// Yes
				context_add_relocation(dst, NULL, rel->sect, rel->size, rel->loc + loc,
									   rel->increment + sect->virt + sym->loc, rel->relative);
			} else {
				context_add_relocation(dst, rel->name, rel->sect, rel->size, rel->loc + loc,
									   rel->increment, rel->relative);											// nope
			}
		}
	}
	
	return 1;
}
