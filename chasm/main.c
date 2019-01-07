// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 10:46 BRT
// Last edited on December 29 of 2018, at 21:51 BRT

#include <arch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *read_file(char *fname) {
	FILE *file = fopen(fname, "rb");																	// Try to open the file
	
	if (file == NULL) {
		return NULL;																					// Failed...
	}
	
	fseek(file, 0, SEEK_END);																			// Go to the end of the file (to get the length)
	
	long length = ftell(file);																			// Get the current position!
	void *buf = malloc(length);																			// Try to alloc our buffer
	
	if (buf == NULL) {
		fclose(file);
		return NULL;
	}
	
	rewind(file);																						// Rewind it back to the beginning
	
	if (!fread(buf, length, 1, file)) {																	// Try to read it!
		free(buf);																						// Failed...
		fclose(file);
		return NULL;
	}
	
	fclose(file);																						// Close the file
	
	return buf;																							// Return the buffer!
}

char *replace_extension(char *fname, char *newext) {
	char *p;
	
	if ((p = strrchr(fname, '.')) == NULL) {															// Let's find the last '.' (the extension)
		p = malloc(strlen(fname) + strlen(newext) + 1);													// Not found, just add the new extension to the fname
		
		strcpy(p, fname);
		strcat(p, newext);
	} else {
		int n;																							// Ok, let's overwrite the extension!
		
		n = p - fname;
		p = malloc(n + strlen(newext) + 1);																// Alloc some space
		
		strncpy(p, fname, n);																			// Copy the original string
		
		p[n] = '\0';																					// Put the NULL-terminator
		
		strcat(p, newext);																				// Put the new extension
	}
	
	return p;
}

int main(int argc, char **argv) {
	char *arch = NULL;
	char *input = NULL;
	char *output = NULL;
	
	if (argc < 2) {																						// Check if we have any arguments
		printf("Usage: %s [options] file\n", argv[0]);													// We don't have any, just print the usage
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																	// Let's parse the arguments!
		int temp = 0;
		
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {									// Help
			printf("Usage: %s [options] file...\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\n");
			printf("    -v or --version       Show the version of this program\n");
			printf("    -o or --output        Set the output filename\n");
			printf("    -a or --arch          Set the output processor architecture\n");
			printf("Supported processor architectures: "); arch_list_all();
			arch_help_all();
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {						// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Intermediate Language Compiler Version 1.0\n");
			return 0;
		} else if ((!strcmp(argv[i], "-a")) || (!strcmp(argv[i], "--arch"))) {							// Set output processor architecture
			if ((i + 1) >= argc) {
				printf("Expected filename after '%s'\n", argv[i]);
				return 1;
			} else {
				arch = argv[++i];
			}
		} else if ((!strcmp(argv[i], "-o")) || (!strcmp(argv[i], "--output"))) {						// Set the output
			if ((i + 1) >= argc) {
				printf("Expected filename after '%s'\n", argv[i]);
				return 1;
			} else {
				output = argv[++i];
			}
		} else if ((temp = arch_option(argc, argv, i)) != 0) {											// Architecture-specific option
			i += temp;
		} else if (input == NULL) {																		// It's the input?
			input = argv[i];																			// Yes!
		} else {
			printf("Error: unrecognized option: '%s'\n", argv[i]);										// No, so it's a unrecognized option
			return 1;
		}
	}
	
	if (!arch_select(arch == NULL ? "x86" : arch)) {													// Try to select the output processor architecture
		printf("Error: invalid arch '%s'\n", arch == NULL ? "x86" : arch);								// Failed...
		return 1;
	} else if (input == NULL) {																			// We have any input file?
		printf("Error: expected input file\n");															// No...
		return 1;
	} else if (output == NULL) {																		// Set the output name?
		output = replace_extension(input, ".bin");														// Yeah
	}
	
	char *code = read_file(input);																		// Try to read the source code
	
	if (code == NULL) {
		printf("Error: couldn't open '%s'\n", input);													// Failed to read it...
		return 1;
	}
	
	lexer_t *lexer = lexer_new(input, code);															// Create the lexer
	
	if (lexer == NULL) {
		printf("compilation failed\n");																	// Failed...
		free(code);
		return 1;
	}
	
	token_t *toks = lexer_lex(lexer);																	// Lex!
	
	if (toks == NULL) {
		printf("compilation failed\n");																	// Failed to lex...
		lexer_free(lexer);
		return 1;
	}
	
	parser_t *parser = parser_new(toks);																// Create the parser
	
	if (parser == NULL) {
		printf("compilation failed\n");																	// Failed...
		parser_free(parser);
		lexer_free(lexer);
		return 1;
	}
	
	node_t *ast = parser_parse(parser);																	// Parse!
	
	if (ast == NULL) {
		printf("compilation failed\n");																	// Failed to parse...
		parser_free(parser);
		lexer_free(lexer);
		return 1;
	}
	
	codegen_t *codegen = codegen_new(ast);																// Create the code generator
	
	if (codegen == NULL) {
		printf("compilation failed\n");																	// Failed...
		parser_free(parser);
		lexer_free(lexer);
		return 1;
	} else if (!codegen_gen(codegen)) {																	// Generate!
		printf("compilation failed\n");																	// Failed to generate...
		codegen_free(codegen);
		parser_free(parser);
		lexer_free(lexer);
		return 1;
	}
	
	for (codegen_reloc_t *rel = codegen->relocs; rel != NULL; rel = rel->next) {						// Let's dump all the unresolved relocations!
		if (!rel->resolved) {																			// Resolved?
			printf("Relocation: section '%s', offset %ld, name '%s', inc %d\n",
				   rel->sect, rel->loc, rel->name, rel->increment);										// Nope, print it!
		}
	}
	
	FILE *out = fopen(output, "wb");																	// Try to open the output file
	
	if (out == NULL) {
		codegen_free(codegen);																			// Failed...
		parser_free(parser);
		lexer_free(lexer);
		return 1;
	}
	
	for (codegen_section_t *cur = codegen->sections; cur != NULL; cur = cur->next) {					// Let's write all sections!
		fwrite(cur->data, cur->size, 1, out);
	}
	
	fclose(out);																						// Close the output file
	codegen_free(codegen);																				// Free the codegen struct
	parser_free(parser);																				// Free the parser struct
	lexer_free(lexer);																					// Free the lexer struct
	
	return 0;
}
