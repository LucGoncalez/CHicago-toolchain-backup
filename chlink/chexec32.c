// File author is Ítalo Lima Marconato Matias
//
// Created on February 15 of 2019, at 15:09 BRT
// Last edited on February 18 of 2019, at 18:41 BRT

#include <chexec32.h>
#include <exec.h>
#include <stdlib.h>

static char *chexec32_get_sect(context_t *context, uintptr_t loc, uintptr_t *out) {
	for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {								// Let's search in what section 'loc' is
		if (loc >= sect->virt && loc < sect->virt + sect->size) {														// Found?
			*out = loc - sect->virt;																					// Yes!
			return sect->name;
		}
	}
	
	return NULL;
}

static int chexec32_load(context_t *context, char *file) {
	if (context == NULL || file == NULL) {																				// Sanity check
		return 0;
	}
	
	chexec32_header_t *hdr = (chexec32_header_t*)file;																	// Get the CHEXEC32 header
	uintptr_t incr = 0;
	
	if (hdr->magic != 0x58454843) {																						// Check the magic number
		return 0;
	}
	
	for (uint32_t i = 0; i < hdr->sh_count; i++) {																		// First, let's add all the sections!
		chexec32_section_t *sect = (chexec32_section_t*)(file + hdr->sh_start + i * sizeof(chexec32_section_t) + incr);	// Get the location (offset) of this one
		char *name = malloc(sect->name_len + 1);																		// Let's convert the name from wchar_t to char
		
		if (name == NULL) {
			return -1;																									// Failed to alloc
		}
		
		for (uint32_t j = 0; j < sect->name_len; j++) {
			name[j] = (char)sect->name[j];
		}
		
		name[sect->name_len] = 0;																						// Zero end it
		
		context_add_section(context, name, sect->size, sect->virt, (uint8_t*)(file + sect->offset));					// Add the section
		
		incr += sect->name_len * sizeof(wchar_t);																		// Save the name length
	}
	
	incr = 0;																											// Clear the incr
	
	for (uint32_t i = 0; i < hdr->st_count; i++) {																		// Now, let's add all the symbols!
		chexec32_sym_t *sym = (chexec32_sym_t*)(file + hdr->st_start + i * sizeof(chexec32_sym_t) + incr);				// Get the location (offset) of this one
		char *name = malloc(sym->name_len + 1);																			// Let's convert the name from wchar_t to char
		
		if (name == NULL) {
			return -1;																									// Failed to alloc
		}
		
		for (uint32_t j = 0; j < sym->name_len; j++) {
			name[j] = (char)sym->name[j];
		}
		
		name[sym->name_len] = 0;																						// Zero end it
		
		uint8_t type = sym->flags & CHEXEC32_SYM_FLAGS_UNDEF ? CONTEXT_SYMBOL_EXTERN :
					   (sym->flags & CHEXEC32_SYM_FLAGS_NONE ? CONTEXT_SYMBOL_GLOBAL : CONTEXT_SYMBOL_LOCAL);
		uintptr_t loc = 0;																								// Let's get the section of this symbol (and address of the symbol INSIDE the section)
		char *sect = chexec32_get_sect(context, sym->virt, &loc);
		
		if (!context_add_symbol(context, name, sect, type, loc)) {														// Add the symbol
			return -1;																									// :(
		}
		
		incr += sym->name_len * sizeof(wchar_t);																		// Save the name length
	}
	
	incr = 0;																											// Clear the incr
	
	for (uint32_t i = 0; i < hdr->rel_count; i++) {																		// Now, let's add all the relocations!
		chexec32_rel_t *rel = (chexec32_rel_t*)(file + hdr->rel_start + i * sizeof(chexec32_rel_t) + incr);				// Get the location (offset) of this one
		char *name = NULL;
		
		if (rel->name_len != 0) {																						// Have a name?
			name = malloc(rel->name_len + 1);																			// Yes, let's convert it from wchar_t to char

			if (name == NULL) {
				return -1;																								// Failed to alloc
			}

			for (uint32_t j = 0; j < rel->name_len; j++) {
				name[j] = (char)rel->name[j];
			}

			name[rel->name_len] = 0;																					// Zero end it
		}
		
		uint8_t size = rel->op & CHEXEC32_REL_OP_BYTE ? 1 : (rel->op & CHEXEC32_REL_OP_WORD ? 2 : 4);					// Get the size
		int rela = (rel->op & (CHEXEC32_REL_OP_REL | CHEXEC32_REL_OP_REL_SYM)) != 0;									// And if this is relative
		uintptr_t loc = 0;																								// Let's get the section of this relocation (and address of the relocation INSIDE the section)
		char *sect = chexec32_get_sect(context, rel->virt, &loc);
		
		context_add_relocation(context, name, sect, size, loc, (int)rel->incr, rela);									// Add the relocation
		
		incr += rel->name_len * sizeof(wchar_t);																		// Save the name length
	}
	
	return 1;
}

REGISTER_EXEC(chexec32, "chexec32", chexec32_load);																		// Register the executable format