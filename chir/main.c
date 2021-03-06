// File author is Ítalo Lima Marconato Matias
//
// Created on February 23 of 2019, at 16:57 BRT
// Last edited on February 24 of 2019, at 14:39 BRT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser.h>

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
	char *input = NULL;
	char *output = NULL;
	int ofre = 0;
	
	if (argc < 2) {																						// Check if we have any arguments
		printf("Usage: %s [options] file\n", argv[0]);													// We don't have any, just print the usage
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																	// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {									// Help
			printf("Usage: %s [options] file...\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\n");
			printf("    -v or --version       Show the version of this program\n");
			printf("    -o or --output        Set the output filename\n");
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {						// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Assembler Version 1.0\n");
			return 0;
		} else if ((!strcmp(argv[i], "-o")) || (!strcmp(argv[i], "--output"))) {						// Set the output
			if ((i + 1) >= argc) {
				printf("Expected filename after '%s'\n", argv[i]);
				return 1;
			} else {
				output = argv[++i];
			}
		} else if (input == NULL) {																		// It's the input?
			input = argv[i];																			// Yes!
		} else {
			printf("Error: unrecognized option: '%s'\n", argv[i]);										// No, so it's a unrecognized option
			return 1;
		}
	}
	
	if (input == NULL) {																				// We have any input file?
		printf("Error: expected input file\n");															// No...
		return 1;
	} else if (output == NULL) {																		// Set the output name?
		output = replace_extension(input, ".o");														// Yeah
		ofre = 1;
	}
	
	char *code = read_file(input);																		// Try to read the source code
	
	if (code == NULL) {
		printf("Error: couldn't open '%s'\n", input);													// Failed to read it...
		
		if (ofre) {																						// Free the output?
			free(output);																				// Yes
		}
		
		return 1;
	}
	
	lexer_t *lexer = lexer_new(input, code);															// Create the lexer
	
	if (lexer == NULL) {
		printf("compilation failed\n");																	// Failed...
		free(code);
		
		if (ofre) {																						// Free the output?
			free(output);																				// Yes
		}
		
		return 1;
	}
	
	token_t *toks = lexer_lex(lexer);																	// Lex!
	
	if (toks == NULL) {
		printf("compilation failed\n");																	// Failed to lex...
		lexer_free(lexer);
		
		if (ofre) {																						// Free the output?
			free(output);																				// Yes
		}
		
		return 1;
	}
	
	parser_t *parser = parser_new(toks);																// Create the parser
	
	if (parser == NULL) {
		printf("compilation failed\n");																	// Failed...
		lexer_free(lexer);
		
		if (ofre) {																						// Free the output?
			free(output);																				// Yes
		}
		
		return 1;
	}
	
	node_t *ast = parser_parse(parser);																	// Parse!
	
	if (ast == NULL) {
		printf("compilation failed\n");																	// Failed to parse...
		parser_free(parser);
		lexer_free(lexer);
		
		if (ofre) {																						// Free the output?
			free(output);																				// Yes
		}
		
		return 1;
	}
	
	parser_free(parser);																				// Free the parser struct
	lexer_free(lexer);																					// Free the lexer struct
	
	if (ofre) {																							// Free the output?
		free(output);																					// Yes
	}
	
	return 0;
}
