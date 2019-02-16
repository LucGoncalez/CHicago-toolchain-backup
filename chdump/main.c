// File author is Ítalo Lima Marconato Matias
//
// Created on January 28 of 2019, at 16:47 BRT
// Last edited on February 16 of 2019, at 13:57 BRT

#include <exec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *read_file(char *fname) {
	FILE *file = fopen(fname, "rb");																			// Try to open the file
	
	if (file == NULL) {
		return NULL;																							// Failed...
	}
	
	fseek(file, 0, SEEK_END);																					// Go to the end of the file (to get the length)
	
	long length = ftell(file);																					// Get the current position!
	void *buf = malloc(length);																					// Try to alloc our buffer
	
	if (buf == NULL) {
		fclose(file);
		return NULL;
	}
	
	rewind(file);																								// Rewind it back to the beginning
	
	if (!fread(buf, length, 1, file)) {																			// Try to read it!
		free(buf);																								// Failed...
		fclose(file);
		return NULL;
	}
	
	fclose(file);																								// Close the file
	
	return buf;																									// Return the buffer!
}

int main(int argc, char **argv) {
	char *input = NULL;
	
	if (argc < 2) {																								// Check if we have any arguments
		printf("Usage: %s [options] file\n", argv[0]);															// We don't have any, just print the usage
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																			// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {											// Help
			printf("Usage: %s [options] file...\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\n");
			printf("    -v or --version       Show the version of this program\n");
			printf("Supported executable formats: "); exec_list_all();
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {								// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Dumper Version 1.0\n");
			return 0;
		} else if (input == NULL) {																				// It's the input?
			input = argv[i];																					// Yes!
		} else {
			printf("Error: unrecognized option: '%s'\n", argv[i]);												// No, so it's a unrecognized option
			return 1;
		}
	}
	
	if (input == NULL) {																						// We have any input file?
		printf("Error: expected input file\n");																	// No...
		return 1;
	}
	
	char *file = read_file(input);																				// Try to read the input file
	
	if (file == NULL) {
		printf("Error: couldn't open '%s'\n", input);															// Failed to read it...
		return 1;
	}
	
	context_t *context = context_new();																			// Create a context for reading the file sections, symbols and relocations
	
	if (context == NULL) {
		printf("Couldn't dump '%s'\n", input);																	// Failed
		return 1;
	} else if (!exec_gen(context, file)) {																		// Let's gen!
		printf("Couldn't dump '%s'\n", input);																	// Failed
		context_free(context);
		return 1;
	}
	
	int newline = 0;
	
	if (context->sections != NULL) {																			// We have sections?
		printf("Sections:\nName\t\tSize              Virtual Address   File Offset\n");							// Yes, print them!
		
		for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {
			if (strlen(sect->name) > 7) {
				printf("%s\t%016llX  %016llX  %016llX\n", sect->name, sect->size, sect->virt, sect->off);
			} else {
				printf("%s\t\t%016llX  %016llX  %016llX\n", sect->name, sect->size, sect->virt, sect->off);
			}
		}
		
		newline = 1;
	}
	
	if (context->symbols != NULL) {																				// We have symbols?
		if (newline) {																							// Yes, print newline first?
			printf("\n");																						// Yes
		}
		
		printf("Symbols:\nName\t\tSection\t\tType    Section Offset\n");										// Let's print them!
		
		for (context_symbol_t *sym = context->symbols; sym != NULL; sym = sym->next) {
			if (strlen(sym->name) > 7) {
				printf("%s\t", sym->name);
			} else {
				printf("%s\t\t", sym->name);
			}
			
			if (strlen(sym->sect) > 7) {
				printf("%s\t", sym->sect);
			} else {
				printf("%s\t\t", sym->sect);
			}
			
			printf("%s  %016llX\n", sym->type == 1 ? "Extern" : (sym->type == 0 ? "Global" : "Local "),
				   					sym->loc);
		}
		
		newline = 1;
	}
	
	if (context->relocs != NULL) {																				// We have relocations?
		if (newline) {																							// Yes, print newline first?
			printf("\n");																						// Yes
		}
		
		printf("Relocations:\nName\t\tSection\t\tSize  Virtual Address   Increment\tRelative\n");				// Let's print them
		
		for (context_reloc_t *rel = context->relocs; rel != NULL; rel = rel->next) {
			if (strlen(rel->name) > 7) {
				printf("%s\t", rel->name);
			} else {
				printf("%s\t\t", rel->name);
			}
			
			if (strlen(rel->sect) > 7) {
				printf("%s\t", rel->sect);
			} else {
				printf("%s\t\t", rel->sect);
			}
			
			printf("%02X    %016llX  %lld\t\t%s\n", rel->size, rel->loc, rel->increment, rel->relative ?
				   									"Yes" : "No");
		}
	}
	
	free(file);																									// Free the file, we don't need it anymore
	context_free(context);																						// Free the context
	
	return 0;
}
