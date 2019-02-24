// File author is Ítalo Lima Marconato Matias
//
// Created on February 16 of 2019, at 20:21 BRT
// Last edited on February 24 of 2019, at 15:30 BRT

#include <exec.h>
#include <script.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SYSROOT
#define SYSROOT ""
#endif

#define SYSROOT_PATH(path) SYSROOT path

typedef struct lib_search_path_s {
	char *path;
	struct lib_search_path_s *next;
} lib_search_path_t;

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

static lib_search_path_t *create_search_paths(char *first) {
	lib_search_path_t *paths = calloc(1, sizeof(lib_search_path_t));											// Alloc space for the first search path
	
	if (paths != NULL) {																						// Ok?
		paths->path = first;																					// Yes, so set it!
	}
	
	return paths;
}

static void free_search_paths(lib_search_path_t *paths) {
	for (lib_search_path_t *path = paths, *next; path != NULL; path = next) {
		next = path->next;																						// Set the next entry
		free(path->path);																						// Free the path name
		free(path);																								// And the path struct
	}
}

static void add_search_path(lib_search_path_t *paths, char *path) {
	lib_search_path_t *cur = paths;																				// Let's find the last entry
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(cur->path) == strlen(path)) && !strcmp(cur->path, path)) {									// This path already exists?
			return;																								// Yes
		} else if (cur->next == NULL) {
			break;																								// End!
		}
	}
	
	cur->next = calloc(1, sizeof(lib_search_path_t));															// Alloc
	
	if (cur->next == NULL) {
		return;																									// Failed >:(
	}
	
	cur->next->path = path;																						// And set the path
}

static char *find_in_search_path(lib_search_path_t *paths, char *file) {
	for (lib_search_path_t *cur = paths; cur != NULL; cur = cur->next) {										// Let's search in the search paths!
		char *path = malloc(strlen(cur->path) + strlen(file) + 2);												// Let's concat the path and the file name
		
		if (path != NULL) {
			strcpy(path, cur->path);
			strcat(path, "/");
			strcat(path, file);
			
			char *readed = read_file(path);																		// And try to read the file
			
			free(path);
			
			if (readed != NULL) {																				// Ok?
				return readed;																					// Yes, we found it :)
			}
		}
	}
	
	return NULL;																								// Not found :(
}

int main(int argc, char **argv) {
	if (argc < 2) {																								// Check if we have any arguments
		printf("Usage: %s [options] files\n", argv[0]);															// We don't have any, just print the usage
		return 1;
	}
	
	int temp = 0;
	int inputs = 0;
	char *output = NULL;
	char *format = NULL;
	char *script = NULL;
	lib_search_path_t *paths = create_search_paths(strdup(SYSROOT_PATH("/Libraries")));							// Let's create the search path struct
	
	if (paths == NULL) {
		printf("Error: couldn't create the search path struct\n");												// ...
		return 1;
	}
	
	context_t *context = context_new();																			// Let's create the context
	
	if (context == NULL) {
		printf("Error: couldn't create the main context\n");													// ...
		free_search_paths(paths);
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {																			// Let's parse the arguments!
		if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {											// Help
			printf("Usage: %s [options] file...\n", argv[0]);
			printf("Options:\n");
			printf("    -h or --help          Show this help dialog\n");
			printf("    -v or --version       Show the version of this program\n");
			printf("    -o or --output        Set the output filename\n");
			printf("    -T or --script        Set the linker script\n");
			printf("    -L or --library-path  Add a path to the library search path\n");
			printf("    -l or --library       Add a library as dependency\n");
			printf("Supported formats: "); exec_list_all();
			exec_help_all();
			context_free(context);
			free_search_paths(paths);
			return 0;
		} else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {								// Version
			printf("CHicago Operating System Project\n");
			printf("CHicago Linker Version 1.0\n");
			context_free(context);
			free_search_paths(paths);
			return 0;
		} else if ((!strcmp(argv[i], "-o")) || (!strcmp(argv[i], "--output"))) {								// Set the output
			if ((i + 1) >= argc) {
				printf("Expected filename after '%s'\n", argv[i]);
				context_free(context);
				free_search_paths(paths);
				return 1;
			} else {
				output = argv[++i];
			}
		} else if ((!strcmp(argv[i], "-T")) || (!strcmp(argv[i], "--script"))) {								// Set the script file
			if ((i + 1) >= argc) {
				printf("Error: expected filename after '%s'\n", argv[i]);
				context_free(context);
				free_search_paths(paths);
				return 1;
			} else {
				script = argv[++i];
			}
		} else if ((!strcmp(argv[i], "-L")) || (!strcmp(argv[i], "--library-path"))) {							// Add path to the lib search paths
			if ((i + 1) >= argc) {
				printf("Error: expected path after '%s'\n", argv[i]);
				context_free(context);
				free_search_paths(paths);
				return 1;
			} else {
				add_search_path(paths, argv[++i]);
			}
		} else if ((!strcmp(argv[i], "-l")) || (!strcmp(argv[i], "--library"))) {								// Add lib as dep
			if ((i + 1) >= argc) {
				printf("Error: expected library name after '%s'\n", argv[i]);
				context_free(context);
				free_search_paths(paths);
				return 1;
			} else {
				char *fnam = argv[++i];
				char *file = find_in_search_path(paths, fnam);
				
				if (file == NULL) {
					printf("Error: couldn't open '%s'\n", fnam);												// ...
					context_free(context);
					free_search_paths(paths);
					return 1;
				} else if (!exec_add_dep(context, fnam, file)) {												// Load it!
					free(file);
					context_free(context);
					free_search_paths(paths);
					return 1;
				}
				
				free(file);
			}
		} else if ((temp = exec_option(format, argc, argv, i)) != 0) {											// Executable format-specific option?
			if (temp == -1) {
				context_free(context);																			// Failed...
				free_search_paths(paths);
				return 1;
			} else if (temp != -2) {
				i += temp;
			}
		} else {
			char *file = read_file(argv[i]);																	// Input file! Try to read it
			
			if (file == NULL) {
				printf("Error: couldn't open '%s'\n", argv[i]);													// ...
				context_free(context);
				free_search_paths(paths);
				return 1;
			}
			
			context_t *context2 = context_new();
			
			if (context2 == NULL) {
				printf("Error: couldn't create the sub context\n");
				free(file);
				context_free(context);
				free_search_paths(paths);
				return 1;
			} else if (!exec_load(context2, argv[i], file, format == NULL ? &format : NULL)) {					// Load it!
				context_free(context2);																			// Failed...
				free(file);
				context_free(context);
				free_search_paths(paths);
				return 1;
			} else if (!context_merge(context, context2)) {														// Merge the contexts
				context_free(context2);																			// Failed...
				free(file);
				context_free(context);
				free_search_paths(paths);
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
		free_search_paths(paths);
		return 1;
	} else if (output == NULL) {																				// Set the output name?
		output = "a.out";																						// Yeah
	} else if (script != NULL) {																				// We have a linker script?
		char *code = read_file(script);																			// Yes, open it :)
		
		if (code == NULL) {
			printf("Error: couldn't open '%s'\n", script);														// ...
			context_free(context);
			free_search_paths(paths);
			return 1;
		}
		
		context_t *context2 = parse_script(context, script, code);												// Parse the script
		
		if (context2 == NULL) {
			context_free(context);																				// Failed
			free_search_paths(paths);
			return 1;
		} else {
			context_free(context);
			context = context2;
		}
		
		free(code);
	}
	
	FILE *out = fopen(output, "wb");																			// Try to open the output file
	
	if (out == NULL) {
		printf("Error: couldn't open the output file\n");														// Failed
		context_free(context);
		free_search_paths(paths);
		return 1;
	}
	
	int res = exec_gen(format, context, out);																	// Generate the output!
	
	fclose(out);
	context_free(context);
	free_search_paths(paths);
	
	return !res;
}
