// File author is Ítalo Lima Marconato Matias
//
// Created on December 01 of 2018, at 10:15 BRT
// Last edited on December 02 of 2018, at 08:36 BRT

#include <codegen.h>
#include <stdlib.h>
#include <stddef.h>
#include <str.h>

static char *codegenx86_gen_instr(ir_module_t *module, ir_method_t *method, ir_instr_t *instr) {
	return 0;																															// TODO!
}

static char *codegenx86_gen_method(ir_module_t *module, ir_method_t *method) {
	if (method == NULL) {																												// Valid method?
		return NULL;																													// Nope
	}
	
	string_t *str = str_create(NULL);																									// Create our temp string
	
	if (str == NULL) {
		return NULL;																													// Failed
	} else if (!str_append_formated(str, ".global %s\n%s:\n\tpush %%ebp\n\tmov %%esp, %%ebp\n", method->name, method->name)) {			// Append the name of the method, and the start of the method
		str_free(str);
		return NULL;
	} else if (method->local_count != 0 && !str_append_formated(str, "\tsub $%d, %%esp\n", method->local_count * 4)) {					// If required, alloc space for the locals
		str_free(str);
		return NULL;
	}
	
	if (!str_append_formated(str, "\tleave\n\tret\n")) {																				// Append the end of the method
		str_free(str);
		return NULL;
	}
	
	char *out = str->string;																											// Save the output string
	
	str->string = NULL;																													// We don't want str_free to free the output string
	str_free(str);																														// Free the str struct
	
	return out;
}

static char *codegenx86_gen_module(ir_module_t *module) {
	if (module == NULL) {																												// Valid module?
		return NULL;																													// Nope
	}
	
	string_t *str = str_create(NULL);																									// Create our temp string
	
	if (str == NULL) {
		return NULL;																													// Failed
	} else if (module->global_count != 0) {																								// Append the globals?
		if (!str_append_string(str, ".data\n\n")) {																						// Yes, enter in the .data section
			str_free(str);
			return NULL;
		}
		
		for (uint32_t i = 0; i < module->global_count; i++) {																			// And let's append the globals!
			ir_arg_t *g = module->globals[i];
			
			if (!str_append_formated(str, ".global %s\n%s: .int 0\n\n", g->name, g->name)) {											// TODO: instead of .int 0, init it with the right value!
				str_free(str);
				return NULL;
			}
		}
	}
	
	if (module->method_count != 0) {																									// Append the methods?
		if (!str_append_string(str, ".text\n\n")) {																						// Yes, enter in the .text section
			str_free(str);
			return NULL;
		}
		
		for (uint32_t i = 0; i < module->method_count; i++) {																			// And let's do it!
			char *method = codegenx86_gen_method(module, module->methods[i]);															// Generate the code for the method
			
			if (method == NULL) {
				str_free(str);																											// Failed...
				return NULL;
			} else if (!str_append_formated(str, "%s\n", method)) {																		// Append it!
				free(method);
				str_free(str);
				return NULL;
			}
			
			free(method);
		}
	}
	
	char *out = str->string;																											// Save the output string
	
	str->string = NULL;																													// We don't want str_free to free the output string
	str_free(str);																														// Free the str struct
	
	return out;
}

CODEGEN_REGISTER_ARCH void codegenx86_register(void) {
	codegen_arch_t *arch = codegen_create_arch("x86");																					// Try to create our arch
	
	if (arch != NULL) {
		arch->gen_instr = codegenx86_gen_instr;																							// Ok! Set all the functions
		arch->gen_method = codegenx86_gen_method;
		arch->gen_module = codegenx86_gen_module;
		
		codegen_register_arch(arch);																									// And register it!
	}
}
