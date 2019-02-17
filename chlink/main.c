// File author is Ítalo Lima Marconato Matias
//
// Created on February 16 of 2019, at 20:21 BRT
// Last edited on February 18 of 2019, at 18:00 BRT

#include <exec.h>
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
	
	char *format = NULL;
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
			printf("Supported formats: "); exec_list_all();
			context_free(context);
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {								// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Linker Version 1.0\n");
			context_free(context);
			return 0;
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
		}
	}
	
	context_free(context);
	
	return 0;
}
