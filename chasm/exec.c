// File author is Ítalo Lima Marconato Matias
//
// Created on January 10 of 2018, at 10:46 BRT
// Last edited on January 10 of 2019, at 10:57 BRT

#include <exec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

exec_list_t *exec_list = NULL;
exec_t *exec_current = NULL;

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

int exec_register(char *name, void (*help)(), int (*option)(int, char**, int), int (*gen)(codegen_t*, FILE*)) {
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
	cur->next->exec->gen = gen;
	
	return 1;
}

int exec_select(char *name) {
	exec_t *exec = NULL;
	
	if (name != NULL && (exec = exec_find(name)) != NULL) {												// Try to find this exec!
		exec_current = exec;																			// :)
		return 1;
	}
	
	return 0;																							// :(
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

void exec_help() {
	if (exec_current != NULL && exec_current->help != NULL) {											// Check if the arguments are valid
		exec_current->help();																			// And redirect
	}
}

int exec_option(int argc, char **argv, int i) {
	if (exec_current != NULL && exec_current->option != NULL && argv != NULL) {							// Check if the arguments are valid
		return exec_current->option(argc, argv, i);														// And redirect
	}
	
	return 0;
}

int exec_gen(codegen_t *codegen, FILE *out) {
	if (exec_current != NULL && exec_current->gen != NULL && codegen != NULL && out != NULL) {			// Check if the arguments are valid
		return exec_current->gen(codegen, out);															// And redirect
	}
	
	return 0;
}
