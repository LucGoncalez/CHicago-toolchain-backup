// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:33 BRT
// Last edited on February 20 of 2019, at 17:40 BRT

#include <exec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

exec_list_t *exec_list = NULL;

static exec_t *exec_find(char *name) {
	exec_list_t *cur = exec_list;
	
	while (cur != NULL) {																				// Let's search!
		if ((strlen(cur->exec->name) == strlen(name)) && !strcmp(cur->exec->name, name)) {				// Found?
			return cur->exec;																			// Yes!
		}
		
		cur = cur->next;																				// No, go to the next entry
	}
	
	return NULL;
}

int exec_register(char *name, void (*help)(), int (*option)(int, char**, int), int (*load)(context_t*, char*), int (*add_dep)(context_t*, char*, char*), int (*gen)(context_t*, FILE*)) {
	if (name == NULL) {																					// We have everything we need?
		return 0;																						// No...
	} else if (exec_list == NULL) {																		// First entry?
		exec_list = calloc(1, sizeof(exec_list_t));														// Yes, alloc space
		
		if (exec_list == NULL) {
			return 0;																					// Failed
		}
		
		exec_list->exec = malloc(sizeof(exec_t));														// Alloc space for the exec struct
		
		if (exec_list->exec == NULL) {
			free(exec_list);																			// Failed
			return 0;
		}
		
		exec_list->exec->name = name;																	// Fill the fields!
		exec_list->exec->help = help;
		exec_list->exec->option = option;
		exec_list->exec->load = load;
		exec_list->exec->add_dep = add_dep;
		exec_list->exec->gen = gen;
		
		return 1;
	} else if (exec_find(name)) {																		// Redefinition?
		return 0;																						// >:(
	}
	
	exec_list_t *cur = exec_list;																		// Ok, let's find the last entry
	
	while (cur->next != NULL) {
		cur = cur->next;
	}
	
	cur->next = calloc(1, sizeof(exec_list_t));															// Alloc space
	
	if (cur->next == NULL) {
		return 0;																						// Failed
	}
	
	cur->next->exec = malloc(sizeof(exec_t));															// Alloc space for the exec struct
	
	if (cur->next->exec == NULL) {
		free(cur->next);																				// Failed
		return 0;
	}
	
	cur->next->exec->name = name;																		// Fill the fields!
	cur->next->exec->help = help;
	cur->next->exec->option = option;
	cur->next->exec->load = load;
	cur->next->exec->add_dep = add_dep;
	cur->next->exec->gen = gen;
	
	return 1;
}

void exec_list_all() {
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Just print all the avaliable executable formats
		printf("%s%s", cur != exec_list ? ", " : "", cur->exec->name);
	}
	
	printf("\n");
}

void exec_help_all() {
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Just print the help for all the avaliable executable formats
		printf("Options for %s:\n", cur->exec->name);
		
		if (cur->exec->help != NULL) {
			cur->exec->help();
		}
	}
}

int exec_option(char *format, int argc, char **argv, int i) {
	if (format == NULL || argv == NULL) {																// Null pointer check
		return 0;
	}
	
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Let's search for the format
		if ((strlen(cur->exec->name) == strlen(format)) && !strcmp(cur->exec->name, format)) {			// Found?
			if (cur->exec->option != NULL) {															// Yes, null pointer check
				return cur->exec->option(argc, argv, i);												// Ok!
			} else {
				return 0;																				// ...
			}
		}
	}
	
	return 0;
}

int exec_load(context_t *context, char *fname, char *file, char **format) {
	if (context == NULL || fname == NULL || file == NULL) {												// Null pointer check
		return 0;
	}
	
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Let's try to find the right executable format
		if (cur->exec->load != NULL) {																	// Let's check if this is the right one
			int res = cur->exec->load(context, file);
			
			if (res != 0) {																				// Ok!
				if (format != NULL) {																	// Save the format name?
					*format = cur->exec->name;															// Yes!
				}
				
				return res < 0 ? 0 : res;
			}
		}
	}
	
	printf("Error: couldn't determine the format of '%s'\n", fname);
	
	return 0;
}

int exec_add_dep(context_t *context, char *fname, char *file) {
	if (context == NULL || fname == NULL || file == NULL) {												// Null pointer check
		return 0;
	}
	
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Let's try to find the right executable format
		if (cur->exec->add_dep != NULL) {																// Let's check if this is the right one
			int res = cur->exec->add_dep(context, fname, file);
			
			if (res != 0) {																				// Ok!
				return res < 0 ? 0 : res;
			}
		}
	}
	
	printf("Error: couldn't determine the format of '%s'\n", fname);
	
	return 0;
}

int exec_gen(char *format, context_t *context, FILE *out) {
	if (format == NULL || context == NULL || out == NULL) {												// Null pointer check
		return 0;
	}
	
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Let's search for the format
		if ((strlen(cur->exec->name) == strlen(format)) && !strcmp(cur->exec->name, format)) {			// Found?
			if (cur->exec->gen != NULL) {																// Yes, null pointer check
				return cur->exec->gen(context, out);													// Ok!
			} else {
				return 0;																				// ...
			}
		}
	}
	
	return 0;
}
