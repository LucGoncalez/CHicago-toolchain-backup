// File author is Ítalo Lima Marconato Matias
//
// Created on November 13 of 2018, at 19:22 BRT
// Last edited on November 15 of 2018, at 13:57 BRT

#include <byteswap.h>
#include <chexec.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <utils.h>
#include <elf.h>

static void swap_hdr(elf32_header_t *hdr) {
	hdr->type = bswap_16(hdr->type);																					// Swap all the fields
	hdr->machine = bswap_16(hdr->machine);
	hdr->version = bswap_32(hdr->version);
	hdr->entry = bswap_32(hdr->entry);
	hdr->ph_offset = bswap_32(hdr->ph_offset);
	hdr->sh_offset = bswap_32(hdr->sh_offset);
	hdr->flags = bswap_32(hdr->flags);
	hdr->eh_size = bswap_16(hdr->eh_size);
	hdr->ph_ent_size = bswap_16(hdr->ph_ent_size);
	hdr->ph_num = bswap_16(hdr->ph_num);
	hdr->sh_ent_size = bswap_16(hdr->sh_ent_size);
	hdr->sh_num = bswap_16(hdr->sh_num);
	hdr->sh_str_ndx = bswap_16(hdr->sh_str_ndx);
}

static void swap_ph(elf32_program_header_t *ph) {
	ph->type = bswap_32(ph->type);																						// Swap all the fields
	ph->offset = bswap_32(ph->offset);
	ph->vaddr = bswap_32(ph->vaddr);
	ph->paddr = bswap_32(ph->paddr);
	ph->file_size = bswap_32(ph->file_size);
	ph->mem_size = bswap_32(ph->mem_size);
	ph->flags = bswap_32(ph->flags);
	ph->align = bswap_32(ph->align);
}

static void swap_sh(elf32_section_header_t *sh) {
	sh->name = bswap_32(sh->name);																						// Swap all the fields
	sh->type = bswap_32(sh->type);
	sh->flags = bswap_32(sh->flags);
	sh->vaddr = bswap_32(sh->vaddr);
	sh->offset = bswap_32(sh->offset);
	sh->size = bswap_32(sh->size);
	sh->link = bswap_32(sh->link);
	sh->info = bswap_32(sh->info);
	sh->addr_align = bswap_32(sh->addr_align);
	sh->ent_size = bswap_32(sh->ent_size);
}

static void swap_sym(elf32_symbol_t *sym) {
	sym->name = bswap_32(sym->name);																					// Swap all the fields
	sym->value = bswap_32(sym->value);
	sym->size = bswap_32(sym->size);
	sym->shndx = bswap_16(sym->shndx);
}

static void swap_rel(elf32_relocation_t *rel) {
	rel->offset = bswap_32(rel->offset);																				// Swap all the fields
	rel->info = bswap_32(rel->info);
}

static void swap_dyn(elf32_dynamic_t *dyn) {
	dyn->tag = bswap_32(dyn->tag);																						// Swap all the fields
	dyn->val = bswap_32(dyn->val);
}

static int find_ph(elf32_header_t *hdr, int idx, uint32_t type, int swap) {
	for (int i = idx; i < hdr->ph_num; i++) {																			// Let's try to find the program header (starting the search at i)
		elf32_program_header_t *ph = ELF_PH_GET32(hdr, i);
		
		if (swap) {																										// Swap?
			swap_ph(ph);																								// Yup
		}
		
		if (ph->type == type) {																							// Found?
			return i;																									// Yes :)
		}
	}
	
	return -1;
}

static char *get_shstrtab(elf32_header_t *hdr, int swap) {
	elf32_section_header_t *sh = ELF_SH_GET32(hdr, hdr->sh_str_ndx);													// Get the section header
	
	if (swap) {																											// Swap
		swap_sh(sh);
	}
	
	return (char*)hdr + sh->offset;																						// And return
}

static elf32_dynamic_t *get_dyn(elf32_header_t *hdr, int dynph, int swap) {
	elf32_program_header_t *ph = ELF_PH_GET32(hdr, dynph);																// Get the program header
	
	if (swap) {																											// Swap
		swap_ph(ph);
	}
	
	return (elf32_dynamic_t*)ELF_PH_CONTENT(hdr, ph);																	// And return
}

static int add_symbols(char *symtab, char *strtab, int size, uint32_t syment, int swap) {
	for (int i = 1; i < size; i++) {																					// Let's add all the symbols!
		elf32_symbol_t *sym = (elf32_symbol_t*)(symtab + i * syment);
		
		if (swap) {																										// Swap
			swap_sym(sym);
		}
		
		if (sym->shndx == ELF_SHN_UNDEF) {																				// Not defined in this module?
			if (!add_symbol(strtab + sym->name, 0, CHEXEC_SYMBOL_FLAGS_UNDEF)) {										// Yup, set the UNDEF flag
				return 0;
			}
			
			continue;
		} else if (sym->shndx < ELF_SHN_LORESERVE) {
			sym->shndx = ELF_SHN_ABS;
		} else if (sym->shndx != ELF_SHN_ABS) {
			printf("Error: unimplemented shndx %d\n", sym->shndx);														// Unsupported type...
			return 0;
		}
		
		if (ELF_ST_BIND(sym->info) & ELF_STB_GLOBAL) {																	// Global?
			if (!add_symbol(strtab + sym->name, sym->value, CHEXEC_SYMBOL_FLAGS_NONE)) {								// Yes, add it!
				return 0;
			}
		}
	}
}

static int add_sections(elf32_header_t *hdr, char *shstrtab, int swap) {
	for (int i = 0; i < hdr->sh_num; i++) {																				// Let's add all the sections!
		elf32_section_header_t *sh = ELF_SH_GET32(hdr, i);
		
		if (swap) {																										// Swap
			swap_sh(sh);
		}
		
		if (sh->type == ELF_SHT_PROGBITS) {																				// Zero initialized data?
			add_section(shstrtab + sh->name, sh->vaddr, 0, CHEXEC_SECTION_FLAGS_NONE);									// No, so add this section
			
			for (uint32_t i = 0; i < sh->size; i++) {																	// And add the contents!
				write_section_data(shstrtab + sh->name, ((char*)ELF_SH_CONTENT(hdr, sh))[i]);
			}
		} else if (sh->type == ELF_SHT_NOBITS) {
			add_section(shstrtab + sh->name, sh->vaddr, sh->size, CHEXEC_SECTION_FLAGS_ZEROINIT);						// Yes, just add it with the ZEROINIT flag!
		}
	}
	
	return 1;
}

static int add_relocations(char *strtab, char *reltab, int entsz, int max, char *symtab, int syment, int swap) {
	for (char *end = reltab + max; reltab < end; reltab += entsz) {														// Let's add all the relocations!
		elf32_relocation_t *rel = (elf32_relocation_t*)reltab;
		elf32_symbol_t *sym = (elf32_symbol_t*)(symtab + ELF_R_SYM(rel->info) * syment);
		
		if (swap) {																										// Swap
			swap_rel(rel);
			swap_sym(sym);
		}
		
		switch (ELF_R_TYPE(rel->info)) {
		case 1: {																										// Symbol as base
			add_relocation(strtab + sym->name, 3, rel->offset);
			break;
		}
		case 2: {																										// Relatiive symbol
			add_relocation(strtab + sym->name, 2, rel->offset);
			break;
		}
		case 6:
		case 7: {																										// Symbol
			add_relocation(strtab + sym->name, 1, rel->offset);
			break;
		}
		case 8: {																										// Absolute
			add_relocation(NULL, 0, rel->offset);
			break;
		}
		default: {
			printf("Error: unsupported ELF relocation type %d\n", ELF_R_TYPE(rel->info));
			return 0;
		}
		}
	}
		
	return 1;
}

static int add_dependencies(elf32_header_t *hdr, char *strtab, int dynph, int swap) {	
	for (elf32_dynamic_t *dyn = get_dyn(hdr, dynph, swap); dyn->tag != ELF_DT_NULL; dyn++) {							// Let's add all the dependencies!
		if (swap) {																										// Swap
			swap_dyn(dyn);
		}
		
		if (dyn->tag == ELF_DT_NEEDED) {																				// Dependency?
			char *name = strtab + dyn->val;																				// Yes!
			
			if (!strcmp(name, "libchicago.so")) {																		// chicago.chl?
				add_dependency("chicago.chl");																			// Yes :)
			} else {
				char *new = strdup(name);																				// No, duplicate the original string
				
				new = realloc(new, strlen(new) + 2);																	// Realloc it
				
				new[strlen(name) - 2] = 'c';																			// Change the extension to .chl
				new[strlen(name) - 1] = 'h';
				new[strlen(name)] = 'l';
				new[strlen(name) + 1] = '\0';
				
				add_dependency(new);																					// Add this dependency!
				free(new);																								// Free the temp string
			}
		}
	}
	
	return 1;
}

uintptr_t conv_elf32(void *buf, int swap) {
	if (buf == NULL) {																									// Sanity checks
		return 0;
	}
	
	elf32_header_t *hdr = (elf32_header_t*)buf;
	
	if (swap) {																											// Swap?
		swap_hdr(hdr);																									// Yup
	}
	
	if (hdr->type != ELF_TYPE_DYN) {																					// Check the type
		printf("Error: expected dynamic-linked ELF file\n");
		free(buf);
		return 0;
	} else if (hdr->machine != ELF_MACHINE_386) {																		// For now we only support x86
		printf("Error: unsupported ELF file (unsupported target machine)\n");
		free(buf);
		return 0;
	}
	
	int dynph = find_ph(hdr, 0, ELF_PT_DYNAMIC, swap);																	// Try to find the Dynamic ph
	
	if (dynph == -1) {
		printf("Error: broken dynamic-linked ELF file\n");																// Probably this file is broken
		free(buf);
		return 0;
	}
	
	uint32_t *hash = NULL;																								// Time to find some required components in the elf file
	char *rel = NULL;
	char *jmprel = NULL;
	char *strtab = NULL;
	char *symtab = NULL;
	char *shstrtab = get_shstrtab(hdr, swap);
	uint32_t strsz = 0;
	uint32_t syment = 0;
	uint32_t relsz = 0;
	uint32_t relent = 0;
	uint32_t pltrel = 0;
	uint32_t pltrelsz = 0;
	
	for (elf32_dynamic_t *dyn = get_dyn(hdr, dynph, swap); dyn->tag != ELF_DT_NULL; dyn++) {							// Let's search!
		if (swap) {																										// Swap
			swap_dyn(dyn);
		}
		
		if (dyn->tag == ELF_DT_PLTRELSZ) {																				// PLT Relocation Table Size
			pltrelsz = dyn->val;
		} else if (dyn->tag == ELF_DT_HASH) {																			// SysV Hash
			hash = buf + dyn->val;
		} else if (dyn->tag == ELF_DT_STRTAB) {																			// String Table
			strtab = buf + dyn->val;
		} else if (dyn->tag == ELF_DT_SYMTAB) {																			// Symbol Table
			symtab = buf + dyn->val;
		} else if (dyn->tag == ELF_DT_STRSZ) {																			// String Table Size
			strsz = dyn->val;
		} else if (dyn->tag == ELF_DT_SYMENT) {																			// Symbol Table Entry Size
			syment = dyn->val;
		} else if (dyn->tag == ELF_DT_REL) {																			// Relocation Table
			rel = buf + dyn->val;
		} else if (dyn->tag == ELF_DT_RELSZ) {																			// Relocation Table Size
			relsz = dyn->val;
		} else if (dyn->tag == ELF_DT_RELENT) {																			// Relocation Table Entry Size
			relent = dyn->val;
		} else if (dyn->tag == ELF_DT_PLTREL) {																			// PLT Relocation Type
			pltrel = dyn->val;
		} else if (dyn->tag == ELF_DT_JMPREL) {																			// Jump Relocation Table
			jmprel = buf + dyn->val;
		}
	}
	
	if ((!hash) || (!strtab) || (!symtab) || (!shstrtab) || (!syment) || (!strsz)) {									// Found everything?
		printf("Error: broken dynamic-linked ELF file\n");																// No, probably this file is broken
		free(buf);
		return 0;
	}
	
	if (!add_symbols(symtab, strtab, hash[1], syment, swap)) {															// Add all the symbols
		free(buf);
		return 0;
	} else if (!add_sections(hdr, shstrtab, swap)) {																	// Add all the sections
		free(buf);
		return 0;
	} else if (!add_relocations(strtab, rel, relent, relsz, symtab, syment, swap)) {									// Add all the relocations
		free(buf);
		return 0;
	} else if (jmprel) {																								// We have more relocations?
		if (!pltrelsz || !pltrel) {																						// Yes!
			printf("Error: broken dynamic-linked ELF file\n");															// No, probably this file is broken
			free(buf);
			return 0;
		} else if (!add_relocations(strtab, jmprel, sizeof(elf32_relocation_t), pltrelsz, symtab, syment, swap)) {		// Add them!
			free(buf);
			return 0;
		}
	}
	
	if (!add_dependencies(hdr, strtab, dynph, swap)) {																	// Add all the dependencies
		free(buf);
		return 0;
	}
	
	return hdr->entry;
}
