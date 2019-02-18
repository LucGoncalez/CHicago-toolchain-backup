// File author is Ítalo Lima Marconato Matias
//
// Created on February 16 of 2019, at 20:21 BRT
// Last edited on February 18 of 2019, at 19:01 BRT

#include <exec.h>
#include <script.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *read_file(char *fname) {
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
	if (argc < 2) {																								// Check if we have any arguments
		printf("Usage: %s [options] files\n", argv[0]);															// We don't have any, just print the usage
		return 1;
	}
	
	int inputs = 0;
	char *format = NULL;
	char *script = NULL;
	context_t *context = context_new();																			// Let's create the context
	
	if (context == NULL) {
		printf("Error: couldn't create the main context\n");													// ...
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																			// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {											// Help
			printf("Usage: %s [options] file...\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\n");
			printf("    -v or --version       Show the version of this program\n");
			printf("    -T or --script        Set the linker script\n");
			printf("Supported formats: "); exec_list_all();
			context_free(context);
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {								// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Linker Version 1.0\n");
			context_free(context);
			return 0;
		} else if ((!strcmp(argv[i], "-T")) || (!strcmp(argv[i], "--script"))) {								// Set the script file
			if ((i + 1) >= argc) {
				printf("Error: expected filename after '%s'\n", argv[i]);
				return 1;
			} else {
				script = argv[++i];
			}
		} else {
			char *file = read_file(argv[i]);																	// Input file! Try to read it
			
			if (file == NULL) {
				printf("Error: couldn't open '%s'\n", argv[i]);													// ...
				context_free(context);
				return 1;
			}
			
			context_t *context2 = context_new();
			
			if (context2 == NULL) {
				printf("Error: couldn't create the sub context\n");
				free(file);
				context_free(context);
				return 1;
			} else if (!exec_load(context2, argv[i], file, format == NULL ? &format : NULL)) {					// Load it!
				context_free(context2);																			// Failed...
				free(file);
				context_free(context);
				return 1;
			} else if (!context_merge(context, context2)) {														// Merge the contexts
				context_free(context2);																			// Failed...
				free(file);
				context_free(context);
				return 1;
			}
			
			context_free(context2);
			free(file);
			
			inputs++;
		}
	}
	
	if (inputs == 0) {																							// We have at least one input file?
		printf("Error: expected at least one input file\n");													// No...
		context_free(context);
		return 1;
	} else if (script != NULL) {																				// We have a linker script?
		char *code = read_file(script);																			// Yes, open it :)
		
		if (code == NULL) {
			printf("Error: couldn't open '%s'\n", script);														// ...
			context_free(context);
			return 1;
		}
		
		context_t *context2 = parse_script(context, script, code);												// Parse the script
		
		if (context2 == NULL) {
			context_free(context);																				// Failed
			return 1;
		} else {
			context_free(context);
			context = context2;
		}
		
		free(code);
	}
	
	int newline = 0;
	
	if (context->sections != NULL) {																			// We have sections?
		printf("Sections:\nName\t\tSize              Virtual Address\n");										// Yes, print them!
		
		for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {
			if (strlen(sect->name) > 7) {
				printf("%s\t%016llX  %016llX\n", sect->name, sect->size, sect->virt);
			} else {
				printf("%s\t\t%016llX  %016llX\n", sect->name, sect->size, sect->virt);
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
			if (rel->name != NULL && (strlen(rel->name) > 7)) {
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
	
	context_free(context);
	
	return 0;
}
