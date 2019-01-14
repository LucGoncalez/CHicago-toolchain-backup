// File author is Ítalo Lima Marconato Matias
//
// Created on January 14 of 2019, at 12:56 BRT
// Last edited on January 14 of 2019, at 14:37 BRT

#include <exec.h>
#include <string.h>
#include <inttypes.h>

static uintmax_t bin_entry = 0;

static void bin_help(void) {
	printf("    -e or --entry         Set the entry point of the executable\n");								// Just print all our options
}

static int bin_option(int argc, char **argv, int i) {
	if (argv == NULL || argv == NULL || i >= argc) {															// Sanity checks
		return 0;
	}
	
	int start = i;																								// Save the initial i
	char *endptr = NULL;
	
	if ((!strcmp(argv[i], "-e")) || (!strcmp(argv[i], "--entry"))) {											// Set the entry point
		if ((i + 1) >= argc) {
			printf("Expected a number after '%s'\n", argv[i]);
			return -1;
		} else {
			bin_entry = strtoumax(argv[++i], &endptr, 0);
		}
	}
	
	return i - start;
}

static void bin_write_data(codegen_t *codegen, uintmax_t val, int size) {
	if (size == 1) {																							// Byte
		codegen_write_byte(codegen, val);
	} else if (size == 2) {																						// Word
		codegen_write_word(codegen, val);
	} else if (size == 4) {																						// DWord
		codegen_write_dword(codegen, val);
	} else if (size == 8) {																						// QWord
		codegen_write_qword(codegen, val);
	}
}

static uintptr_t bin_read_data(codegen_t *codegen, int size) {
	uint8_t *data = codegen->current_section->data;
	uintptr_t loc = codegen->current_section->size;
	
	if (size == 1) {																							// Byte
		return data[loc];
	} else if (size == 2) {																						// Word
		return data[loc] | (uint16_t)(data[loc + 1] << 8);
	} else if (size == 4) {																						// DWord
		return data[loc] | (data[loc + 1] << 8) | (data[loc + 2] << 16) | (data[loc + 3] << 24);
	} else if (size == 8) {																						// QWord
		uint32_t w1 = data[loc] | (data[loc + 1] << 8) | (data[loc + 2] << 16) | (data[loc + 3] << 24);
		return w1 | (((uint64_t)(data[loc + 4] | (data[loc + 5] << 8) | (data[loc + 6] << 16) |
								(data[loc + 7] << 24))) << 32);
	} else {
		return 0;
	}
}

static int bin_fix_rel(codegen_t *codegen, char *sect, char *sym, uintptr_t loc, int inc, int rel, int size) {
	codegen_select_section(codegen, sect);																		// First, select the section where this reloc is
	
	if (sym == NULL) {																							// Add the symbols's location?
		uintptr_t old = codegen->current_section->size;															// No, save the size of the section
		
		codegen->current_section->size = loc;																	// Move it to the location of the reloc
		inc += bin_read_data(codegen, size);																	// inc = inc + data already in this loc
		
		if (rel) {																								// Relative?
			bin_write_data(codegen, bin_entry + inc - loc, size);												// Yes
		} else {
			bin_write_data(codegen, bin_entry + inc, size);														// Nope
		}
		
		codegen->current_section->size = old;																	// Restore the old size
		
		return 1;
	}
	
	codegen_label_t *lbl = codegen_get_label(codegen, sym);														// Get the symbol
	
	if (lbl == NULL || !lbl->local_resolved || lbl->type == CODEGEN_LABEL_EXTERN) {								// Valid?
		printf("undefined reference to '%s'\n", sym);															// Nope :(
		return 0;
	}
	
	uintptr_t old = codegen->current_section->size;																// Save the size of the section
	uintptr_t lloc = bin_entry + codegen_get_section_start(codegen, lbl->sect) + lbl->loc;						// Get the location of the label (absolute)
	
	codegen->current_section->size = loc;																		// Move it to the location of the reloc
	inc += bin_read_data(codegen, size);																		// inc = inc + data already in this loc
	
	if (rel) {																									// Relative?
		bin_write_data(codegen, lloc + inc - loc, size);														// Yes
	} else {
		bin_write_data(codegen, lloc + inc, size);																// Nope
	}
	
	codegen->current_section->size = old;																		// Restore the old size
	
	return 1;
}

static int bin_gen(codegen_t *codegen, FILE *out) {
	if (codegen == NULL || out == NULL) {																		// Sanity checks
		return 0;
	}
	
	for (codegen_reloc_t *rel = codegen->relocs; rel != NULL; rel = rel->next) {								// First, let's fix the relocations
		if (!bin_fix_rel(codegen, rel->sect, rel->name, rel->loc, rel->increment, rel->relative, rel->size)) {	// Fix this relocation
			return 0;																							// Failed...
		}
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {							// Now, let's write the section data!
		if (!fwrite(cur->data, cur->size, 1, out) && (cur->size != 0)) {										// Write the data from this section
			return 0;																							// Failed
		}
	}
	
	return 1;
}

REGISTER_EXEC(bin, "bin", bin_help, bin_option, bin_gen);														// Register the executable format
