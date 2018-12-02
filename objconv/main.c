// File author is Ítalo Lima Marconato Matias
//
// Created on November 12 of 2018, at 20:11 BRT
// Last edited on November 16 of 2018, at 10:00 BRT

#include <elf.h>
#include <stdio.h>
#include <utils.h>
#include <chexec.h>
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>

int main(int argc, char *argv[]) {
	char *input = NULL;
	char *output = NULL;
	int library = 0;
	
	if (argc == 1) {																					// Check if we have any arguments
		printf("Usage: %s [options] file\n", argv[0]);													// We don't have any, just print the usage
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																	// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {									// Help
			printf("Usage: %s [options] file...\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\n");
			printf("    -v or --version       Show the version of this program\n");
			printf("    -o or --output        Set the output file\n");
			printf("    -l or --library       Output an library file\n");
			printf("    -e or --executable    Output an executable file\n");
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {						// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Object Converter Version 1.0.0\n");
			return 0;
		} else if ((!strcmp(argv[i], "-o")) || (!strcmp(argv[i], "--output"))) {						// Set the output
			if ((i + 1) >= argc) {
				printf("Expected filename after '%s'\n", argv[i]);
				return 1;
			} else {
				output = argv[++i];
			}
		} else if ((!strcmp(argv[i], "-l")) || (!strcmp(argv[i], "--library"))) {						// Set the library flag
			library = 1;
		} else if ((!strcmp(argv[i], "-e")) || (!strcmp(argv[i], "--executable"))) {					// Unset the library flag
			library = 0;
		} else {
			if (input == NULL) {																		// It's the input?
				input = argv[i];																		// Yes!
			} else {
				printf("Error: unrecognized option: '%s'\n", argv[i]);									// No, so it's a unrecognized option
				return 1;
			}
		}
	}
	
	if (input == NULL) {																				// We have any input file?
		printf("Error: expected input file\n");															// No...
		return 1;
	} else if ((output == NULL) && library) {															// Set the output name (as .chl)?
		output = replace_extension(input, ".chl");														// Yes!
	} else if (output == NULL) {																		// Set the output name (as .che)?
		output = replace_extension(input, ".che");														// Yes
	}
	
	void *buf = read_file(input);																		// Try to read the source file
	elf32_header_t *hdr = buf;
	
	if (buf == NULL) {
		printf("Error: couldn't open '%s'\r\n", input);													// Failed to read it...
		return 1;
	}
	
	if ((hdr->ident[ELF_IDENT_MAGIC_0] != ELF_MAGIC_0) ||												// First, check if it's a valid elf file
		(hdr->ident[ELF_IDENT_MAGIC_1] != ELF_MAGIC_1) ||
		(hdr->ident[ELF_IDENT_MAGIC_2] != ELF_MAGIC_2) ||
		(hdr->ident[ELF_IDENT_MAGIC_3] != ELF_MAGIC_3)) {
		printf("Error: invalid ELF file\n");															// ...
		free(buf);
		return 1;
	} else if (hdr->ident[ELF_IDENT_VERSION] != 1) {													// Check the version
		printf("Error: invalid ELF version\n");
		free(buf);
		return 1;
	}
	
	int is64 = hdr->ident[ELF_IDENT_CLASS] == ELF_CLASS_64;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	int swap = hdr->ident[ELF_IDENT_DATA] != ELF_DATA_MSB;
#else
	int swap = hdr->ident[ELF_IDENT_DATA] != ELF_DATA_LSB;
#endif
	uintptr_t res = is64 ? conv_elf64(buf, swap) : conv_elf32(buf, swap);								// Let's go!
	
	if (res == 0) {
		return 1;																						// Failed...
	}
	
	FILE *out = fopen(output, "wb");																	// Try to open the output file
	
	if (out == NULL) {
		free(buf);																						// Failed
		return 1;
	}
	
	if (is64) {
		printf("Error: unsupported target architecture\n");
		free(buf);
		return 1;
	}
	
	uint32_t magic = swap ? bswap_32(CHEXEC_HEADER_MAGIC) : CHEXEC_HEADER_MAGIC;
	uint32_t flags = CHEXEC_HEADER_FLAGS_ARCH_X86;
	uint32_t entry = (uint32_t)res;
	uint32_t shcnt = (uint32_t)section_count;
	uint32_t shstr = swap ? bswap_32(sizeof(chexec32_header_t)) : sizeof(chexec32_header_t);
	uint32_t stcnt = (uint32_t)symbol_count;
	uint32_t ststr = swap ? bswap_32(shstr + get_sh_size32()) : shstr + get_sh_size32();
	uint32_t relcn = (uint32_t)relocation_count;
	uint32_t relst = swap ? bswap_32(ststr + get_st_size32()) : ststr + get_st_size32();
	uint32_t depcn = (uint32_t)dependency_count;
	uint32_t depst = swap ? bswap_32(relst + get_rel_size32()) : relst + get_rel_size32();
	uint32_t secst = swap ? bswap_32(depst + get_dep_size32()) : depst + get_dep_size32();
	
	if (library) {																						// Library?
		flags |= CHEXEC_HEADER_FLAGS_LIBRARY;															// Yes
	} else {
		flags |= CHEXEC_HEADER_FLAGS_EXECUTABLE;														// No, set the executable flag
	}
	
	if (swap) {
		flags = bswap_32(flags);
	}
	
	fwrite(&magic, sizeof(uint32_t), 1, out);															// Write the chexec header
	fwrite(&flags, sizeof(uint32_t), 1, out);
	fwrite(&entry, sizeof(uint32_t), 1, out);
	fwrite(&shcnt, sizeof(uint32_t), 1, out);
	fwrite(&shstr, sizeof(uint32_t), 1, out);
	fwrite(&stcnt, sizeof(uint32_t), 1, out);
	fwrite(&ststr, sizeof(uint32_t), 1, out);
	fwrite(&relcn, sizeof(uint32_t), 1, out);
	fwrite(&relst, sizeof(uint32_t), 1, out);
	fwrite(&depcn, sizeof(uint32_t), 1, out);
	fwrite(&depst, sizeof(uint32_t), 1, out);
	
	for (section_t *s = sections; s != NULL; s = s->next) {												// Write all the section headers!
		uint32_t flags = s->flags;
		uint32_t vaddr = (uint32_t)s->vaddr;
		uint32_t nl = strlen(s->name) + 1;
		
		fwrite(&flags, sizeof(uint32_t), 1, out);
		fwrite(&secst, sizeof(uint32_t), 1, out);
		fwrite(&vaddr, sizeof(uint32_t), 1, out);
		
		if (s->flags & CHEXEC_SECTION_FLAGS_ZEROINIT) {													// Zero init?
			uint32_t size = (uint32_t)s->size;															// Yup
			fwrite(&size, sizeof(uint32_t), 1, out);
		} else {
			uint32_t dcnt = (uint32_t)s->data_count;													// No, so we need to add to secst the data count
			fwrite(&dcnt, sizeof(uint32_t), 1, out);
			secst += dcnt;
		}
		
		fwrite(&nl, sizeof(uint32_t), 1, out);
		fwrite(s->name, nl, 1, out);
	}
	
	for (symbol_t *s = symbols; s != NULL; s = s->next) {												// Write all the symbols
		uint32_t flags = s->flags;
		uint32_t vaddr = (uint32_t)s->vaddr;
		uint32_t nl = strlen(s->name) + 1;
		
		fwrite(&flags, sizeof(uint32_t), 1, out);
		fwrite(&vaddr, sizeof(uint32_t), 1, out);
		fwrite(&nl, sizeof(uint32_t), 1, out);
		fwrite(s->name, nl, 1, out);
	}
	
	for (relocation_t *r = relocations; r != NULL; r = r->next) {										// Write all the relocations
		uint32_t op = r->op;
		uint32_t vaddr = (uint32_t)r->vaddr;
		uint32_t nl = r->name != NULL ? (strlen(r->name) + 1) : 0;
		
		fwrite(&op, sizeof(uint32_t), 1, out);
		fwrite(&vaddr, sizeof(uint32_t), 1, out);
		fwrite(&nl, sizeof(uint32_t), 1, out);
		
		if (r->name != NULL) {																			// We need to write the name?
			fwrite(r->name, nl, 1, out);																// Yup
		}
	}
	
	for (dependency_t *d = dependencies; d != NULL; d = d->next) {										// Write all the dependencies
		uint32_t nl = strlen(d->name) + 1;
		
		fwrite(&nl, sizeof(uint32_t), 1, out);
		fwrite(d->name, nl, 1, out);
	}
	
	for (section_t *s = sections; s != NULL; s = s->next) {												// Write the data of the sections!
		fwrite(s->data, s->data_count, 1, out);
	}
	
	fclose(out);																						// Close the output file
	free(buf);																							// Free the buffer
	
	return 0;
}
