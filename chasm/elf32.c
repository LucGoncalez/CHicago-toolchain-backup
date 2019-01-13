// File author is Ítalo Lima Marconato Matias
//
// Created on January 10 of 2018, at 10:39 BRT
// Last edited on January 12 of 2019, at 22:07 BRT

#include <arch.h>
#include <exec.h>
#include <elf32.h>
#include <stdlib.h>
#include <string.h>

extern arch_t *arch_current;

static void elf32_help(void) { }
static int elf32_option(int argc, char **argv, int i) { (void)argc; (void)argv; (void)i; return 0; }

static uint16_t elf32_get_machine(char *name) {
	if (!strcmp(name, "x86")) {																	// 386?
		return ELF32_MACHINE_386;																// Yes
	} else {
		return ELF32_MACHINE_NONE;																// Unsupported
	}
}

static uint32_t elf32_get_flags(char *name, uint32_t *align) {
	if (!strcmp(name, ".bss") || !strcmp(name, ".data")) {										// .bss or .data section?
		*align = 4;																				// Yes, align is 4 bytes, and we're in rw memory
		return ELF32_SHF_WRITE | ELF32_SHF_ALLOC;
	} else if (!strcmp(name, ".rodata")) {														// .rodata section?
		*align = 4;																				// Yes, align is also 4 bytes, and we're in ro memory
		return ELF32_SHF_ALLOC;
	} else if (!strcmp(name, ".text")) {														// .text section?
		*align = 16;																			// Yes, align is 16 bytes, and we're in ro memory with exec permissions
		return ELF32_SHF_ALLOC | ELF32_SHF_EXECINSTR;
	} else {
		*align = 1;																				// Unknown section, align = 1, ro memory
		return ELF32_SHF_ALLOC;
	}
}

static int elf32_get_sect(codegen_t *codegen, char *name) {
	int cur = 1;																				// Let's get the section offset!
	
	for (codegen_section_t *sect = codegen->sections; sect != NULL; sect = sect->next) {
		if (!strcmp(sect->name, name)) {														// Found?
			return cur;
		}
		
		cur++;																					// Go to the next offset
	}
	
	return 0;
}

static int elf32_get_name(codegen_t *codegen, char *name) {
	if (name == NULL) {																			// Valid name?
		return 0;																				// Nope
	}
	
	int cur = 1;																				// Let's get the name offset in strtab!
	
	for (codegen_label_t *lbl = codegen->labels; lbl != NULL; lbl = lbl->next) {
		if (!strcmp(lbl->name, name) && lbl->local_resolved) {									// Found?
			return cur;
		} else if (lbl->local_resolved) {
			cur += strlen(lbl->name) + 1;														// Go to the next offset
		}
	}
	
	return 0;
}

static int elf32_get_sym(codegen_t *codegen, char *name) {
	int cur = 1;																				// Let's get the symbol offset in symtab!
	
	for (codegen_label_t *lbl = codegen->labels; lbl != NULL; lbl = lbl->next) {
		if (!strcmp(lbl->name, name) && lbl->local_resolved) {									// Found?
			return cur;
		} else if (lbl->local_resolved) {
			cur++;																				// Go to the next offset
		}
	}
	
	return 0;
}

static int elf32_have_rel(codegen_t *codegen, char *name) {
	for (codegen_reloc_t *cur = codegen->relocs; cur != NULL; cur = cur->next) {				// Let's go!
		if (!strcmp(cur->sect, name) && !cur->increment) {										// Is the section that we want? And is unresolved? And don't have addend?
			return 1;																			// Yeah!
		}
	}
	
	return 0;
}

static int elf32_get_relc(codegen_t *codegen, char *name) {
	int ret = 0;
	
	for (codegen_reloc_t *cur = codegen->relocs; cur != NULL; cur = cur->next) {				// Let's go!
		if (!strcmp(cur->sect, name) && !cur->increment) {										// Is the section that we want? And is unresolved? And don't have addend?
			ret++;																				// Yeah!
		}
	}
	
	return ret;
}

static int elf32_have_rela(codegen_t *codegen, char *name) {
	for (codegen_reloc_t *cur = codegen->relocs; cur != NULL; cur = cur->next) {				// Let's go!
		if (!strcmp(cur->sect, name) && cur->increment) {										// Is the section that we want? And is unresolved? And have addend?
			return 1;																			// Yeah!
		}
	}
	
	return 0;
}

static int elf32_get_relac(codegen_t *codegen, char *name) {
	int ret = 0;
	
	for (codegen_reloc_t *cur = codegen->relocs; cur != NULL; cur = cur->next) {				// Let's go!
		if (!strcmp(cur->sect, name) && cur->increment) {										// Is the section that we want? And is unresolved? And have addend?
			ret++;																				// Yeah!
		}
	}
	
	return ret;
}

static int elf32_write_hdr(FILE *out, int sects) {
	elf32_header_t hdr;																			// Let's fill the elf header!
	
	memset(&hdr, 0, sizeof(elf32_header_t));													// Fill the header with 0
	
	hdr.ident[0] = 0x7F;																		// First the magic number in the start
	hdr.ident[1] = 'E';
	hdr.ident[2] = 'L';
	hdr.ident[3] = 'F';
	hdr.ident[4] = 1;																			// Now let's indicate that this is a 32-bits file
	hdr.ident[5] = 1;																			// For now we only support LSB
	hdr.ident[6] = 1;																			// Set the version (current)
	hdr.type = 1;																				// Relocatable file
	hdr.machine = elf32_get_machine(arch_current->name);										// Set the machine
	
	if (hdr.machine == ELF32_MACHINE_NONE) {													// Unsupported?
		return 0;																				// >:(
	}
	
	hdr.version = 1;																			// Set the version (current)
	hdr.sh_offset = sizeof(elf32_header_t);														// Set the start of the section headers
	hdr.eh_size = sizeof(elf32_header_t);														// Set the size of the header
	hdr.ph_ent_size = 32;																		// Set the size of one program header
	hdr.ph_num = 0;																				// We don't have any program header
	hdr.sh_ent_size = sizeof(elf32_section_header_t);											// Set the size of one section header
	hdr.sh_num = sects + 4;																		// Set the amount of section headers we have
	hdr.sh_str_ndx = sects + 3;																	// Set the index of the section name string table section
	
	if (!fwrite(&hdr, sizeof(elf32_header_t), 1, out)) {										// Write the header!
		return 0;																				// Failed
	}
	
	return 1;
}

static int elf32_write_section(FILE *out, char *n, uint32_t v, uint32_t o, uint32_t i, uint32_t sz, int b, int s, int c) {
	elf32_section_header_t shdr;																// Let's fill the section header
	
	memset(&shdr, 0, sizeof(elf32_section_header_t));											// Fill the header with 0
	
	shdr.name = i;																				// Set the name index in shstrtab
	
	if (b != 2) {																				// Set the type?
		shdr.type = b == 1 ? ELF32_SHT_NOBITS : (b == 3 ? ELF32_SHT_REL :
										   (b == 4 ? ELF32_SHT_RELA :
										   (s == 1 ? ELF32_SHT_STRTAB :
										   (s == 2 ? ELF32_SHT_SYMTAB : ELF32_SHT_PROGBITS))));	// Set the type
	}
	
	if (n != NULL) {
		shdr.flags = elf32_get_flags(n, &shdr.addr_align);										// Set the flags and the memory alignment
	}
	
	shdr.vaddr = v;																				// Set the virtual address
	shdr.offset = o;																			// Set the offset in the file
	shdr.size = sz;																				// And the size
	
	if (b == 3 || b == 4) {																		// Set the link, info and entsize field (for rel)?
		shdr.link = c;																			// Yes
		shdr.info = s;
		shdr.ent_size = b == 3 ? sizeof(elf32_rel_t) : sizeof(elf32_rela_t);
	} else if (s == 2) {																		// Set the link, info and entsize field (for sym)?
		shdr.link = c;																			// Yes
		shdr.info = sz / sizeof(elf32_symbol_t);
		shdr.ent_size = sizeof(elf32_symbol_t);
	}
	
	if (!fwrite(&shdr, sizeof(elf32_section_header_t), 1, out)) {								// Write the header!
		return 0;																				// Failed
	}
	
	return 1;
}

static int elf32_write_symbol(FILE *out, uint32_t n, uint32_t v, int b, int s, int ss) {
	elf32_symbol_t sym;																			// Let's fill the sym header
	
	memset(&sym, 0, sizeof(elf32_symbol_t));													// Fill the header with 0
	
	sym.name = n;																				// Set the name
	sym.value = v;																				// Set the value
	sym.info = ELF32_ST_INFO(b == CODEGEN_LABEL_EXTERN ? CODEGEN_LABEL_GLOBAL : b,
							 !ss ? 0 : 3);														// Set the info field
	sym.shndx = s == -1 ? 0 : s;																// And the shndx field
	
	if (!fwrite(&sym, sizeof(elf32_symbol_t), 1, out)) {										// Write the header!
		return 0;																				// Failed
	}
	
	return 1;
}

static int elf32_write_rel(FILE *out, uint32_t name, uint32_t off, int size, int r) {
	elf32_rel_t rel;																			// Let's fill the rel header
	
	memset(&rel, 0, sizeof(elf32_rel_t));														// Fill the header with 0
	
	rel.offset = off;																			// Set the offset of the relocation in the section
	rel.info = ELF32_R_INFO(name, size == 1 && !r ? ELF32_R_386_8 :
							(size == 1 && r ? ELF32_R_386_PC8 :
							(size == 2 && !r ? ELF32_R_386_16 :
							(size == 2 && r ? ELF32_R_386_PC16 :
							(!r ? ELF32_R_386_32 : ELF32_R_386_PC32)))));						// Set the info field
	
	if (!fwrite(&rel, sizeof(elf32_rel_t), 1, out)) {											// Write the header!
		return 0;																				// Failed
	}
	
	return 1;
}

static int elf32_write_rela(FILE *out, uint32_t name, uint32_t off, int incr, int size, int r) {
	elf32_rela_t rel;																			// Let's fill the rel header
	
	memset(&rel, 0, sizeof(elf32_rela_t));														// Fill the header with 0
	
	rel.offset = off;																			// Set the offset of the relocation in the section
	rel.info = ELF32_R_INFO(name, size == 1 && !r ? ELF32_R_386_8 :
							(size == 1 && r ? ELF32_R_386_PC8 :
							(size == 2 && !r ? ELF32_R_386_16 :
							(size == 2 && r ? ELF32_R_386_PC16 :
							(!r ? ELF32_R_386_32 : ELF32_R_386_PC32)))));						// Set the info field
	rel.addend = incr;																			// Set the addend field
	
	if (!fwrite(&rel, sizeof(elf32_rela_t), 1, out)) {											// Write the header!
		return 0;																				// Failed
	}
	
	return 1;
}

static int elf32_gen(codegen_t *codegen, FILE *out) {
	if (codegen == NULL || out == NULL) {														// Sanity check
		return 0;
	}
	
	int sects = 0;																				// Let's get the section count
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next, sects++) ;
	
	int relocs = 0;																				// Let's get the amount of .rel sections
	int relocas = 0;																			// And of .rela sections
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {
		if (elf32_have_rel(codegen, cur->name)) {												// This section need a .rel section?
			relocs++;																			// Yes!
		}
		
		if (elf32_have_rela(codegen, cur->name)) {												// This section need a .rela section?
			relocas++;																			// Yes!
		}
	}
	
	int strtabsz = sects + 1;																	// Let's get the size of the strtab
	int syms = sects + 1;																		// And the size of the symtab
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {
		if (cur->local_resolved) {
			strtabsz += strlen(cur->name) + 1;	
			syms++;
		}
	}
	
	syms *= sizeof(elf32_symbol_t);
	
	if (!elf32_write_hdr(out, sects + relocs + relocas)) {										// Write the elf header
		return 0;																				// Failed
	}
	
	int name = 1;
	uint32_t vaddr = 0;
	uint32_t off = sizeof(elf32_header_t) + (sizeof(elf32_section_header_t) *
											 (sects + relocs + relocas + 4));
	
	elf32_write_section(out, 0, 0, 0, 0, 0, 2, 0, 0);											// Write the NULL section
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Now, write the section headers
		uint32_t sz = (uint32_t)cur->size;
		char *n = cur->name;
		
		if (!elf32_write_section(out, n, vaddr, off, name, sz, !strcmp(n, ".bss"), 0, 0)) {		// Write this section header
			return 0;																			// Failed
		}
		
		vaddr += cur->size;																		// Get the next vaddr
		off += cur->size;																		// And the next offset
		name += strlen(n) + 1;																	// And the next name offset in the shstrtab section
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Let's write all the .rel/.rela sections!
		int sect = elf32_get_sect(codegen, cur->name);											// Get the section index
		
		if (elf32_have_rel(codegen, cur->name)) {												// This section need a .rel section?
			int sz =  sizeof(elf32_rel_t) * elf32_get_relc(codegen, cur->name);					// Get the size of all the rels from this section
			
			if (!elf32_write_section(out, 0, 0, off, name, sz, 3, sect,
									 sects + relocs + relocas + 1)) {							// Yes, write it!
				return 0;																		// Failed...
			}
			
			off += sz;																			// Get the next offset
			name += strlen(cur->name) + 5;														// And the next name offset in the shstrtab section
		}
		
		if (elf32_have_rela(codegen, cur->name)) {												// This section need a .rel section?
			int sz =  sizeof(elf32_rela_t) * elf32_get_relac(codegen, cur->name);				// Get the size of all the relas from this section
			
			if (!elf32_write_section(out, 0, 0, off, name, sz, 4, sect,
									 sects + relocs + relocas + 1)) {							// Yes, write it!
				return 0;																		// Failed...
			}
			
			off += sz;																			// Get the next offset
			name += strlen(cur->name) + 6;														// And the next name offset in the shstrtab section
		}
	}
	
	if (!elf32_write_section(out, 0, 0, off, name, syms, 0, 2, sects + relocs + relocas + 2)) {	// Write the symtab section
		return 0;																				// Failed
	} else {
		off += syms;
	}
	
	if (!elf32_write_section(out, 0, 0, off, name + 8, strtabsz, 0, 1, 0)) {					// Write the strtab section
		return 0;																				// Failed
	} else {
		off += strtabsz;
	}
	
	if (!elf32_write_section(out, 0, 0, off, name + 16, name + 26, 0, 1, 0)) {					// Write the shstrtab section
		return 0;																				// Failed
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Now, write the section data!
		if (!fwrite(cur->data, (uint32_t)cur->size, 1, out) && (cur->size != 0)) {				// Write the data from this section
			return 0;																			// Failed
		}
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Let's write the contents/data of all the .rel/.rela sections!
		if (elf32_have_rel(codegen, cur->name)) {												// This section have a .rel section?
			for (codegen_reloc_t *rel = codegen->relocs; rel != NULL; rel = rel->next) {		// Yes, so let's write the section!
				if (strcmp(rel->sect, cur->name) || rel->increment) {							// Valid relocation?
					continue;																	// Nope	
				} else if (!elf32_write_rel(out, sects + elf32_get_sym(codegen, rel->name),
											rel->loc, rel->size, rel->relative)) {				// Write!
					return 0;																	// Failed
				}
			}
		}
		
		if (elf32_have_rela(codegen, cur->name)) {												// This section have a .rela section?
			for (codegen_reloc_t *rel = codegen->relocs; rel != NULL; rel = rel->next) {		// Yes, so let's write the section!
				if (strcmp(rel->sect, cur->name) || !rel->increment) {							// Valid relocation?
					continue;																	// Nope	
				} else if (!elf32_write_rela(out, sects + elf32_get_sym(codegen, rel->name),
											rel->loc, rel->increment, rel->size,
											rel->relative)) {									// Write!
					return 0;																	// Failed
				}
			}
		}
	}
	
	if (!elf32_write_symbol(out, 0, 0, 0, -1, 0)) {												// NULL symbol is the first one (0)
		return 0;
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Now, write the sections
		if (!elf32_write_symbol(out, 0, 0, 0, elf32_get_sect(codegen, cur->name), 1)) {
			return 0;																			// Failed
		}
	}
	
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {				// In the end, write the symbols
		if (cur->local_resolved) {																// Valid symbol?
			if (!elf32_write_symbol(out, sects + elf32_get_name(codegen, cur->name),
									cur->type == CODEGEN_LABEL_EXTERN ? 0 : cur->loc,
									cur->type, cur->type == CODEGEN_LABEL_EXTERN ? 0 :
									elf32_get_sect(codegen, cur->sect), 0)) {					// Yes!
				return 0;																		// Failed
			}
		}
	}
	
	for (int i = 0; i < sects + 1; i++) {
		if (!fwrite("", 1, 1, out)) {															// NULL symbol (and sections) doesn't have a "real" name...
			return 0;
		}
	}
	
	for (codegen_label_t *cur = codegen->labels; cur != NULL; cur = cur->next) {				// Write the names in the strtab section
		if (!fwrite(cur->name, strlen(cur->name) + 1, 1, out)) {								// Write the name of this symbol
			return 0;																			// Failed
		}
	}
	
	if (!fwrite("", 1, 1, out)) {																// NULL section doesn't have a name
		return 0;
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Write the names in the shstrtab section
		if (!fwrite(cur->name, strlen(cur->name) + 1, 1, out)) {								// Write the name of this section
			return 0;																			// Failed
		}
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {			// Let's write all the .rel/.rela section names!
		if (elf32_have_rel(codegen, cur->name)) {												// This section have a .rel section?
			char *n = malloc(strlen(cur->name) + 5);											// Yes, let's alloc memory for mounting the name!
			
			if (n == NULL) {
				return 0;																		// Failed
			}
			
			sprintf(n, ".rel%s", cur->name);													// Mount the name!
			
			if (!fwrite(n, strlen(n) + 1, 1, out)) {											// Write!
				return 0;																		// Failed
			}
		}
		
		if (elf32_have_rela(codegen, cur->name)) {												// This section have a .rela section?
			char *n = malloc(strlen(cur->name) + 6);											// Yes, let's alloc memory for mounting the name!
			
			if (n == NULL) {
				return 0;																		// Failed
			}
			
			sprintf(n, ".rela%s", cur->name);													// Mount the name!
			
			if (!fwrite(n, strlen(n) + 1, 1, out)) {											// Write!
				return 0;																		// Failed
			}
		}
	}
	
	if (!fwrite(".symtab", 8, 1, out)) {														// Write the name of the symtab name
		return 0;																				// Failed
	}
	
	if (!fwrite(".strtab", 8, 1, out)) {														// Write the name of the strtab name
		return 0;																				// Failed
	}
	
	if (!fwrite(".shstrtab", 10, 1, out)) {														// Write the name of the shstrtab name
		return 0;																				// Failed
	}
	
	return 1;
}

REGISTER_EXEC(elf32, "elf32", elf32_help, elf32_option, elf32_gen);								// Register the executable format
