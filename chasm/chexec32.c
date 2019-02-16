// File author is Ítalo Lima Marconato Matias
//
// Created on January 27 of 2019, at 13:52 BRT
// Last edited on February 16 of 2019, at 13:49 BRT

#include <arch.h>
#include <chexec32.h>
#include <exec.h>
#include <stdlib.h>
#include <string.h>

extern arch_t *arch_current;

static void chexec32_help(void) { }
static int chexec32_option(int argc, char **argv, int i) { (void)argc; (void)argv; (void)i; return 0; }

static uint16_t chexec32_get_machine(char *name) {
	if (!strcmp(name, "x86")) {																	// x86?
		return CHEXEC32_HEADER_FLAGS_ARCH_X86;													// Yes
	} else {
		return 0;																				// Unsupported
	}
}

static uint32_t chexec32_write_hdr(codegen_t *codegen, FILE *out) {
	uintptr_t ret = 0;
	chexec32_header_t hdr;																		// Let's fill the chexec header!
	
	memset(&hdr, 0, sizeof(chexec32_header_t));													// Fill the header with 0
	
	hdr.magic = 0x58454843;																		// First the magic number in the start
	hdr.flags = chexec32_get_machine(arch_current->name);										// Set the machine
	
	if (hdr.flags == 0) {																		// Unsupported?
		return 0;																				// >:(
	}
	
	hdr.sh_start = sizeof(chexec32_header_t);													// Set the start of the section, symbol and relocation headers
	hdr.st_start = sizeof(chexec32_header_t);
	hdr.rel_start = sizeof(chexec32_header_t);
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Let's get the amount of sections
		hdr.sh_count++;
		hdr.st_start += sizeof(chexec32_section_t) + strlen(cur->name) * sizeof(wchar_t);
		hdr.rel_start += sizeof(chexec32_section_t) + strlen(cur->name) * sizeof(wchar_t);
	}
	
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {				// Let's get the amount of symbols
		if (cur->local_resolved) {
			hdr.st_count++;
			hdr.rel_start += sizeof(chexec32_sym_t) + strlen(cur->name) * sizeof(wchar_t);
		}
	}
	
	ret = hdr.rel_start;																		// Set the base return value
	
	for (codegen_reloc_t *cur = codegen->relocs; cur != NULL; cur = cur->next) {				// And the amount of relocs
		hdr.rel_count++;
		ret += sizeof(chexec32_rel_t) + (cur->name != NULL ? strlen(cur->name) *
										 					 sizeof(wchar_t) : 0);
	}
	
	if (!fwrite(&hdr, sizeof(chexec32_header_t), 1, out)) {										// Write the header!
		return 0;																				// Failed
	}
	
	return ret;
}

static int chexec32_write_section(FILE *out, char *n, uint32_t v, uint32_t o, uint32_t s, int b) {
	chexec32_section_t shdr;																	// Let's fill the section header
	
	memset(&shdr, 0, sizeof(chexec32_section_t));												// Fill the header with 0
	
	shdr.flags = b ? CHEXEC32_SECTION_FLAGS_ZEROINIT : CHEXEC32_SECTION_FLAGS_NONE;				// Set the flags
	shdr.offset = o;																			// Set the offset in the file
	shdr.virt = v;																				// Set the virtual address
	shdr.size = s;																				// Set the size
	shdr.name_len = strlen(n);																	// Set the length of the name
	
	if (!fwrite(&shdr, sizeof(chexec32_section_t), 1, out)) {									// Write the header!
		return 0;																				// Failed
	}
	
	for (size_t i = 0; i < shdr.name_len; i++) {												// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																			// Failed
		}
	}
	
	return 1;
}

static int chexec32_write_sym(FILE *out, char *n, uint32_t v, int b) {
	chexec32_sym_t sym;																			// Let's fill the sym header
	
	memset(&sym, 0, sizeof(chexec32_sym_t));													// Fill the header with 0
	
	sym.flags = b == CODEGEN_LABEL_EXTERN ? CHEXEC32_SYM_FLAGS_UNDEF :
				(b == CODEGEN_LABEL_LOCAL ? CHEXEC32_SYM_FLAGS_LOC : CHEXEC32_SYM_FLAGS_NONE);	// Set the flags
	sym.virt = v;																				// Set the virtual address
	sym.name_len = strlen(n);																	// Set the length of the name
	
	if (!fwrite(&sym, sizeof(chexec32_sym_t), 1, out)) {										// Write the header!
		return 0;																				// Failed
	}
	
	for (size_t i = 0; i < sym.name_len; i++) {													// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																			// Failed
		}
	}
	
	return 1;
}

static int chexec32_write_rel(FILE *out, char *n, uint32_t virt, int incr, int size, int r) {
	chexec32_rel_t rel;																			// Let's fill the rel header
	
	memset(&rel, 0, sizeof(chexec32_rel_t));													// Fill the header with 0
	
	rel.op = n == NULL && !r ? CHEXEC32_REL_OP_ABS :
			 (n == NULL && r ? CHEXEC32_REL_OP_REL :
			 (n != NULL && !r ? CHEXEC32_REL_OP_SYM : CHEXEC32_REL_OP_REL_SYM));				// Set the type of the relocation
	rel.op |= size == 1 ? CHEXEC32_REL_OP_BYTE :
			  (size == 2 ? CHEXEC32_REL_OP_WORD : CHEXEC32_REL_OP_DWORD);						// Set the size of the relocation
	rel.incr = incr;																			// Set the increment
	rel.virt = virt;																			// Set the virtual address of the relocation
	rel.name_len = strlen(n);																	// Set the length of the name
	
	if (!fwrite(&rel, sizeof(chexec32_rel_t), 1, out)) {										// Write the header!
		return 0;																				// Failed
	}
	
	for (size_t i = 0; i < rel.name_len; i++) {													// Write the name!
		wchar_t ch = (wchar_t)n[i];
		if (!fwrite(&ch, sizeof(wchar_t), 1, out)) {
			return 0;																			// Failed
		}
	}
	
	return 1;
}

static int chexec32_gen(codegen_t *codegen, FILE *out) {
	if (codegen == NULL || out == NULL) {														// Sanity check
		return 0;
	}
	
	uint32_t vaddr = 0;
	uint32_t off = chexec32_write_hdr(codegen, out);											// Write the chexec header
	
	if (!off) {
		return 0;																				// Failed
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Write the section headers
		uint32_t sz = (uint32_t)cur->size;
		char *n = cur->name;
		
		if (!chexec32_write_section(out, n, vaddr, off, sz, !strcmp(n, ".bss"))) {				// Write this section header
			return 0;																			// Failed
		}
		
		vaddr += cur->size;																		// Get the next vaddr
		off += cur->size;																		// And the next offset
	}
	
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {				// Write the symbols
		if (cur->local_resolved) {																// Valid symbol?
			if (!chexec32_write_sym(out, cur->name,
									   cur->type == CODEGEN_LABEL_EXTERN ? 0 :
									   codegen_get_section_start(codegen, cur->sect) + cur->loc,
									   cur->type)) {											// Yes!
				return 0;																		// Failed
			}
		}
	}

	for (codegen_reloc_t *rel = codegen->relocs; rel != NULL; rel = rel->next) {				// Write the relocations
		if (!chexec32_write_rel(out, rel->name, codegen_get_section_start(codegen, rel->sect) +
								rel->loc, rel->increment, rel->size, rel->relative)) {			// Write this relocation!
			return 0;																			// Failed
		}
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Now, write the section data!
		if (!fwrite(cur->data, (uint32_t)cur->size, 1, out) && (cur->size != 0)) {				// Write the data from this section
			return 0;																			// Failed
		}
	}
	
	return 1;
}

REGISTER_EXEC(chexec32, "chexec32", chexec32_help, chexec32_option, chexec32_gen);				// Register the executable format
