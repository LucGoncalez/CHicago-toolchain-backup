// File author is Ítalo Lima Marconato Matias
//
// Created on February 15 of 2019, at 15:09 BRT
// Last edited on February 20 of 2019, at 18:22 BRT

#include <chexec32.h>
#include <exec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int chexec32_out_type = CHEXEC32_HEADER_FLAGS_EXECUTABLE;

static void chexec32_help(void) {
	printf("    -exec                 Create a executable as the output (default)\n");
	printf("    -lib                  Create a library as the output\n");
	printf("    -drv                  Create a kernel driver as the output\n");
	printf("    -krnl                 Create a kernel (for the CHicago Boot Manager) as the output\n");
}

static int chexec32_option(int argc, char **argv, int i) {
	(void)argc;
	
	if (argv == NULL) {																									// Null pointer check
		return 0;
	}
	
	if ((strlen(argv[i]) == strlen("-exec")) && !strcmp(argv[i], "-exec")) {											// Output is a executable
		chexec32_out_type = CHEXEC32_HEADER_FLAGS_EXECUTABLE;
		return -2;
	} else if ((strlen(argv[i]) == strlen("-lib")) && !strcmp(argv[i], "-lib")) {										// Output is a library
		chexec32_out_type = CHEXEC32_HEADER_FLAGS_LIBRARY;
		return -2;
	} else if ((strlen(argv[i]) == strlen("-drv")) && !strcmp(argv[i], "-drv")) {										// Output is a kernel driver
		chexec32_out_type = CHEXEC32_HEADER_FLAGS_DRIVER;
		return -2;
	} else if ((strlen(argv[i]) == strlen("-krnl")) && !strcmp(argv[i], "-krnl")) {										// Output is a kernel
		chexec32_out_type = CHEXEC32_HEADER_FLAGS_KERNEL;
		return -2;
	}
	
	return 0;
}

static char *chexec32_get_sect(context_t *context, uintptr_t loc, uintptr_t *out) {
	for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {								// Let's search in what section 'loc' is
		if (loc >= sect->virt && loc < sect->virt + sect->size) {														// Found?
			*out = loc - sect->virt;																					// Yes!
			return sect->name;
		}
	}
	
	return NULL;
}

static char *chexec32_get_arch_name(uint32_t flags) {
	if (flags & CHEXEC32_HEADER_FLAGS_ARCH_X86) {																		// x86
		return "x86";
	} else {
		return "unknown";																								// unknown, it's going to fault later lol
	}
}

static int chexec32_load(context_t *context, char *file) {
	if (context == NULL || file == NULL) {																				// Sanity check
		return 0;
	}
	
	chexec32_header_t *hdr = (chexec32_header_t*)file;																	// Get the CHEXEC32 header
	uintptr_t incr = 0;
	
	if (hdr->magic != 0x58454843 || (hdr->flags & CHEXEC32_HEADER_FLAGS_LIBRARY) ||
		(hdr->flags & CHEXEC32_HEADER_FLAGS_EXECUTABLE) || (hdr->flags & CHEXEC32_HEADER_FLAGS_DRIVER) ||
		(hdr->flags & CHEXEC32_HEADER_FLAGS_KERNEL)){																	// Check the magic number and the flags
		return 0;
	}
	
	context->arch = chexec32_get_arch_name(hdr->flags);																	// Get the arch name
	
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

static int chexec32_add_dep(context_t *context, char *fname, char *file) {
	if (context == NULL || fname == NULL || file == NULL) {																// Sanity check
		return 0;
	}
	
	chexec32_header_t *hdr = (chexec32_header_t*)file;																	// Get the CHEXEC32 header
	uintptr_t incr = 0;
	
	if (hdr->magic != 0x58454843 || !(hdr->flags & CHEXEC32_HEADER_FLAGS_LIBRARY)) {									// Check the magic number and the flags
		return 0;
	}
	
	context_add_dep(context, fname);																					// Add this dep
	
	for (uint32_t i = 0; i < hdr->st_count; i++) {																		// Now, let's add all the symbols!
		chexec32_sym_t *sym = (chexec32_sym_t*)(file + hdr->st_start + i * sizeof(chexec32_sym_t) + incr);				// Get the location (offset) of this one
		
		if (!(sym->flags & CHEXEC32_SYM_FLAGS_NONE)) {																	// Global symbol?
			incr += sym->name_len * sizeof(wchar_t);																	// Nope :(
			continue;
		}
		
		char *name = malloc(sym->name_len + 1);																			// Let's convert the name from wchar_t to char
		
		if (name == NULL) {
			return -1;																									// Failed to alloc
		}
		
		for (uint32_t j = 0; j < sym->name_len; j++) {
			name[j] = (char)sym->name[j];
		}
		
		name[sym->name_len] = 0;																						// Zero end it
		
		if (!context_add_dep_sym(context, fname, name)) {																// Add the symbol
			return -1;																									// :(
		}
		
		incr += sym->name_len * sizeof(wchar_t);																		// Save the name length
	}
	
	return 1;
}

static uint32_t chexec32_get_arch_flags(char *arch) {
	if (!strcmp(arch, "x86")) {																							// x86?
		return CHEXEC32_HEADER_FLAGS_ARCH_X86;																			// Yes
	} else {
		return 0;																										// Unsupported
	}
}

static uint32_t chexec32_write_hdr(context_t *context, FILE *out) {
	uintptr_t ret = 0;
	chexec32_header_t hdr;																								// Let's fill the chexec header!
	
	memset(&hdr, 0, sizeof(chexec32_header_t));																			// Fill the header with 0
	
	hdr.magic = 0x58454843;																								// First the magic number in the start
	hdr.flags = chexec32_get_arch_flags(context->arch);																	// Set the machine
	
	if (hdr.flags == 0) {																								// Unsupported?
		return 0;																										// >:(
	}
	
	hdr.flags |= chexec32_out_type;																						// Set the output type
	hdr.sh_start = sizeof(chexec32_header_t);																			// Set the start of the section, symbol and relocation headers
	hdr.st_start = sizeof(chexec32_header_t);
	hdr.rel_start = sizeof(chexec32_header_t);
	hdr.dep_start = sizeof(chexec32_header_t);
	
	for (context_section_t *cur = context->sections; cur != NULL; cur = cur->next) {									// Let's get the amount of sections
		hdr.sh_count++;
		hdr.st_start += sizeof(chexec32_section_t) + strlen(cur->name) * sizeof(wchar_t);
		hdr.rel_start += sizeof(chexec32_section_t) + strlen(cur->name) * sizeof(wchar_t);
		hdr.dep_start += sizeof(chexec32_section_t) + strlen(cur->name) * sizeof(wchar_t);
	}
	
	for (context_symbol_t *cur = context->symbols; cur != NULL; cur = cur->next) {										// Let's get the amount of symbols
		hdr.st_count++;
		hdr.rel_start += sizeof(chexec32_sym_t) + strlen(cur->name) * sizeof(wchar_t);
		hdr.dep_start += sizeof(chexec32_sym_t) + strlen(cur->name) * sizeof(wchar_t);
	}
	
	for (context_reloc_t *cur = context->relocs; cur != NULL; cur = cur->next) {										// The amount of relocs
		hdr.rel_count++;
		hdr.dep_start += sizeof(chexec32_rel_t) + (cur->name != NULL ? strlen(cur->name) * sizeof(wchar_t) : 0);
	}
	
	ret = hdr.dep_start;																								// Set the base return value
	
	for (context_dep_t *cur = context->deps; cur != NULL; cur = cur->next) {											// And the amount of deps
		hdr.dep_count++;
		ret += sizeof(chexec32_dep_t) + strlen(cur->name) * sizeof(wchar_t);
	}
	
	if (!fwrite(&hdr, sizeof(chexec32_header_t), 1, out)) {																// Write the header!
		return 0;																										// Failed
	}
	
	return ret;
}

static int chexec32_write_section(FILE *out, char *n, uint32_t v, uint32_t o, uint32_t s, int b) {
	chexec32_section_t shdr;																							// Let's fill the section header
	
	memset(&shdr, 0, sizeof(chexec32_section_t));																		// Fill the header with 0
	
	shdr.flags = b ? CHEXEC32_SECTION_FLAGS_ZEROINIT : CHEXEC32_SECTION_FLAGS_NONE;										// Set the flags
	shdr.offset = o;																									// Set the offset in the file
	shdr.virt = v;																										// Set the virtual address
	shdr.size = s;																										// Set the size
	shdr.name_len = strlen(n);																							// Set the length of the name
	
	if (!fwrite(&shdr, sizeof(chexec32_section_t), 1, out)) {															// Write the header!
		return 0;																										// Failed
	}
	
	for (size_t i = 0; i < shdr.name_len; i++) {																		// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																									// Failed
		}
	}
	
	return 1;
}

static int chexec32_write_sym(FILE *out, char *n, uint32_t v, int b) {
	chexec32_sym_t sym;																									// Let's fill the sym header
	
	memset(&sym, 0, sizeof(chexec32_sym_t));																			// Fill the header with 0
	
	sym.flags = b == CONTEXT_SYMBOL_EXTERN ? CHEXEC32_SYM_FLAGS_UNDEF :
				(b == CONTEXT_SYMBOL_LOCAL ? CHEXEC32_SYM_FLAGS_LOC : CHEXEC32_SYM_FLAGS_NONE);							// Set the flags
	sym.virt = v;																										// Set the virtual address
	sym.name_len = strlen(n);																							// Set the length of the name
	
	if (!fwrite(&sym, sizeof(chexec32_sym_t), 1, out)) {																// Write the header!
		return 0;																										// Failed
	}
	
	for (size_t i = 0; i < sym.name_len; i++) {																			// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																									// Failed
		}
	}
	
	return 1;
}

static int chexec32_write_rel(FILE *out, char *n, uint32_t virt, int incr, int size, int r) {
	chexec32_rel_t rel;																									// Let's fill the rel header
	
	memset(&rel, 0, sizeof(chexec32_rel_t));																			// Fill the header with 0
	
	rel.op = n == NULL && !r ? CHEXEC32_REL_OP_ABS : (n == NULL && r ? CHEXEC32_REL_OP_REL :
			 (n != NULL && !r ? CHEXEC32_REL_OP_SYM : CHEXEC32_REL_OP_REL_SYM));										// Set the type of the relocation
	rel.op |= size == 1 ? CHEXEC32_REL_OP_BYTE : (size == 2 ? CHEXEC32_REL_OP_WORD : CHEXEC32_REL_OP_DWORD);			// Set the size of the relocation
	rel.incr = incr;																									// Set the increment
	rel.virt = virt;																									// Set the virtual address of the relocation
	rel.name_len = n != NULL ? strlen(n) : 0;																			// Set the length of the name
	
	if (!fwrite(&rel, sizeof(chexec32_rel_t), 1, out)) {																// Write the header!
		return 0;																										// Failed
	}
	
	for (size_t i = 0; i < rel.name_len; i++) {																			// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																									// Failed
		}
	}
	
	return 1;
}

static int chexec32_write_dep(FILE *out, char *n) {
	chexec32_dep_t dep;																									// Let's fill the dep header
	
	memset(&dep, 0, sizeof(chexec32_dep_t));																			// Fill the header with 0
	
	dep.name_len = strlen(n);																							// Set the length of the name
	
	if (!fwrite(&dep, sizeof(chexec32_dep_t), 1, out)) {																// Write the header!
		return 0;																										// Failed
	}
	
	for (size_t i = 0; i < dep.name_len; i++) {																			// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																									// Failed
		}
	}
	
	return 1;
}

static int chexec32_gen(context_t *context, FILE *out) {
	if (context == NULL || out == NULL) {																				// Null pointer check
		return 0;
	}
	
	uint32_t off = chexec32_write_hdr(context, out);																	// Write the header
	
	if (!off) {
		return 0;																										// Failed
	}
	
	for (context_section_t *cur = context->sections; cur != NULL; cur = cur->next) {									// First, let's write the section header
		uint32_t sz = (uint32_t)cur->size;
		uint32_t v = (uint32_t)cur->virt;
		char *n = cur->name;
		
		if (!chexec32_write_section(out, n, v, off, sz, !strcmp(n, ".bss"))) {											// Write this section header
			return 0;																									// Failed
		}
		
		off += cur->size;																								// And the next offset
	}
	
	for (context_symbol_t *cur = context->symbols; cur != NULL; cur = cur->next) {										// Write the symbols
		if (!chexec32_write_sym(out, cur->name, cur->type == CONTEXT_SYMBOL_EXTERN ? 0 :
								context_get_section_start(context, cur->sect) + cur->loc,
								cur->type)) {
			return 0;																									// Failed
		}
	}
	
	for (context_reloc_t *rel = context->relocs; rel != NULL; rel = rel->next) {										// Write the relocations
		if (!chexec32_write_rel(out, rel->name, context_get_section_start(context, rel->sect) + rel->loc,
								rel->increment, rel->size, rel->relative)) {
			return 0;																									// Failed
		}
	}
	
	for (context_dep_t *dep = context->deps; dep != NULL; dep = dep->next) {											// Write the dependencies
		if (!chexec32_write_dep(out, dep->name)) {
			return 0;																									// Failed
		}
	}
	
	for (context_section_t *cur = context->sections; cur != NULL; cur = cur->next) {									// Now, write the section data!
		if (!fwrite(cur->data, (uint32_t)cur->size, 1, out) && (cur->size != 0)) {
			return 0;																									// Failed
		}
	}
	
	return 1;
}

REGISTER_EXEC(chexec32, "chexec32", chexec32_help, chexec32_option, chexec32_load, chexec32_add_dep, chexec32_gen);		// Register the executable format
