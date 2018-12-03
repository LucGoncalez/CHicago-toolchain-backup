// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 10:46 BRT
// Last edited on December 02 of 2018, at 19:48 BRT

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

int main(int argc, char **argv) {
	char *arch = NULL;
	char *input = NULL;
	
	if (argc < 2) {																						// Check if we have any arguments
		printf("Usage: %s [options] file\r\n", argv[0]);												// We don't have any, just print the usage
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																	// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {									// Help
			printf("Usage: %s [options] file...\r\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\r\n");
			printf("    -v or --version       Show the version of this program\r\n");
			printf("    -o or --output        Set the output filename\r\n");
			printf("    -a or --arch          Set the output processor architecture\r\n");
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {						// Version
			printf("CHicago Operating System Project\r\n");
			printf("CHicago Intermediate Language Compiler Version 1.0\r\n");
			return 0;
		} else if ((!strcmp(argv[i], "-a")) || (!strcmp(argv[i], "--arch"))) {							// Set output processor architecture
			if ((i + 1) >= argc) {
				printf("Expected filename after '%s'\n", argv[i]);
				return 1;
			} else {
				arch = argv[++i];
			}
		} else {
			if (input == NULL) {																		// It's the input?
				input = argv[i];																		// Yes!
			} else {
				printf("Error: unrecognized option: '%s'\r\n", argv[i]);								// No, so it's a unrecognized option
				return 1;
			}
		}
	}
	
	if (!arch_select(arch == NULL ? "x86" : arch)) {													// Try to select the output processor architecture
		printf("Error: invalid arch '%s'\r\n", arch == NULL ? "x86" : arch);							// Failed...
		return 1;
	} else if (input == NULL) {																			// We have any input file?
		printf("Error: expected input file\r\n");														// No...
		return 1;
	}
	
	char *code = read_file(input);																		// Try to read the source code
	
	if (code == NULL) {
		printf("Error: couldn't open '%s'\r\n", input);													// Failed to read it...
		return 1;
	}
	
	lexer_t *lexer = lexer_new(input, code);															// Create the lexer
	
	if (lexer == NULL) {
		free(code);																						// Failled...
		return 1;
	}
	
	token_t *toks = lexer_lex(lexer);																	// Lex!
	token_t *cur = toks;
	
	while (cur != NULL) {																				// Print all the tokens!
		token_print(cur);
		cur = cur->next;
	}
	
	token_free_list(toks);																				// Free the token list
	lexer_free(lexer);																					// And the lexer struct
	
	return 0;
}
