// File author is Ítalo Lima Marconato Matias
//
// Created on December 01 of 2018, at 10:11 BRT
// Last edited on December 02 of 2018, at 08:31 BRT

#include <codegen.h>
#include <stdlib.h>
#include <string.h>

codegen_arch_list_t *codegen_arch_list = NULL;
codegen_arch_t *codegen_arch_current = NULL;

codegen_arch_t *codegen_create_arch(char *name) {
	codegen_arch_t *arch = calloc(1, sizeof(codegen_arch_t));											// Alloc some space
	
	if (arch != NULL) {																					// Failed?
		arch->name = name;																				// No, set the name
	}
	
	return arch;
}

static codegen_arch_t *codegen_find_arch(char *name) {
	codegen_arch_list_t *cur = codegen_arch_list;
	
	while (cur != NULL) {																				// Let's search!
		if ((strlen(cur->arch->name) == strlen(name)) && !strcmp(cur->arch->name, name)) {				// Found?
			return cur->arch;																			// Yes!
		}
		
		cur = cur->next;																				// No, go to the next entry
	}
	
	return NULL;
}

int codegen_register_arch(codegen_arch_t *arch) {
	if (arch != NULL && codegen_find_arch(arch->name) == NULL) {										// Sanity checks
		if (codegen_arch_list == NULL) {																// First entry?
			codegen_arch_list = calloc(1, sizeof(codegen_arch_list_t));									// Yes, alloc space
			
			if (codegen_arch_list == NULL) {
				return 0;																				// Failed
			}
			
			codegen_arch_list->arch = arch;																// Set the arch struct
			
			return 1;
		}
		
		codegen_arch_list_t *cur = codegen_arch_list;													// No, let's find the last entry
		
		while (cur->next != NULL) {
			cur = cur->next;
		}
		
		cur->next = calloc(1, sizeof(codegen_arch_list_t));												// Alloc space
		
		if (cur->next == NULL) {
			return 0;																					// Failed
		}
		
		cur->next->arch = arch;																			// And set the arch struct
		
		return 1;
	}
	
	return 0;
}

int codegen_select_arch(char *name) {
	codegen_arch_t *arch = NULL;
	
	if (name != NULL && (arch = codegen_find_arch(name)) != NULL) {										// Try to find this arch
		codegen_arch_current = arch;																	// Found! Set it as the current one
		return 1;
	}
	
	return 0;
}

char *codegen_gen_instr(ir_module_t *module, ir_method_t *method, ir_instr_t *instr) {
	if (codegen_arch_current != NULL && instr != NULL) {												// Sanity checks
		return codegen_arch_current->gen_instr(module, method, instr);									// Redirect
	}
	
	return NULL;
}

char *codegen_gen_method(ir_module_t *module, ir_method_t *method) {
	if (codegen_arch_current != NULL && method != NULL) {												// Sanity checks
		return codegen_arch_current->gen_method(module, method);									// Redirect
	}
	
	return NULL;
}

char *codegen_gen_module(ir_module_t *module) {
	if (codegen_arch_current != NULL && module != NULL) {												// Sanity checks
		return codegen_arch_current->gen_module(module);									// Redirect
	}
	
	return NULL;
}
