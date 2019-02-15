// File author is Ítalo Lima Marconato Matias
//
// Created on February 11 of 2019, at 16:33 BRT
// Last edited on February 15 of 2019, at 15:08 BRT

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

int exec_register(char *name, int (*gen)(context_t*, char*)) {
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
	cur->next->exec->gen = gen;
	
	return 1;
}

void exec_list_all() {
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Just print all the avaliable executable formats
		printf("%s%s", cur != exec_list ? ", " : "", cur->exec->name);
	}
	
	printf("\n");
}

int exec_gen(context_t *context, char *file) {
	if (context == NULL || file == NULL) {
		return 0;
	}
	
	for (exec_list_t *cur = exec_list; cur != NULL; cur = cur->next) {									// Let's try to find the right executable format
		if (cur->exec->gen != NULL) {																	// Let's check if this is the right one
			int res = cur->exec->gen(context, file);
			
			if (res != 0) {																				// Ok!
				return res < 0 ? 0 : res;
			}
		}
	}
	
	return 0;
}
