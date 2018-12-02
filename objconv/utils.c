// The author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 10:11 BRT
// Last edited on November 15 of 2018, at 13:56 BRT

#define __UTILS__

#include <stdio.h>
#include <utils.h>
#include <chexec.h>
#include <stdlib.h>
#include <string.h>

section_t *sections = NULL;
uintptr_t section_count = 0;
symbol_t *symbols = NULL;
uintptr_t symbol_count = 0;
relocation_t *relocations = NULL;
uintptr_t relocation_count = 0;
dependency_t *dependencies = NULL;
uintptr_t dependency_count = 0;

static section_t *get_section(char *name) {
	for (section_t *s = sections; s != NULL; s = s->next) {												// Let's search!
		if (!strcmp(s->name, name)) {																	// Same name?
			return s;																					// Yes :)
		}
	}
	
	return NULL;
}

static symbol_t *get_symbol(char *name) {
	for (symbol_t *s = symbols; s != NULL; s = s->next) {												// Let's search!
		if (!strcmp(s->name, name)) {																	// Same name?
			return s;																					// Yes :)
		}
	}
	
	return NULL;
}

section_t *add_section(char *name, uintptr_t vaddr, uintptr_t size, uint8_t flags) {
	if (sections == NULL) {																				// First entry?
		sections = malloc(sizeof(section_t));															// Yes! Alloc it
		sections->name = strdup(name);																	// Duplicate the name
		sections->vaddr = vaddr;
		sections->flags = flags;
		sections->size = size;
		sections->data = malloc(1);																		// Alloc the initial data space
		sections->data_max = 1;
		sections->data_count = 0;
		sections->next = NULL;
		section_count++;																				// Increase the section count!
		
		return sections;
	} else if (get_section(name) != NULL) {																// This section already exists?
		printf("Error: section '%s' redefined\n", name);												// Yes :(
		exit(1);
		return NULL;
	} else {
		section_t *last;																				// Let's find the last entry!
		
		for (last = sections; last->next != NULL; last = last->next) ;
		
		last->next = malloc(sizeof(section_t));															// Ok! Alloc some space
		last->next->name = strdup(name);																// Duplicate the name
		last->next->vaddr = vaddr;
		last->next->flags = flags;
		last->next->size = size;
		last->next->data = malloc(1);																	// Alloc the initial data space
		last->next->data_max = 1;
		last->next->data_count = 0;
		last->next->next = NULL;
		section_count++;																				// Increase the section count!
		
		return last->next;
	}
}

symbol_t *add_symbol(char *name, uintptr_t vaddr, uint8_t flags) {
	if (symbols == NULL) {																				// First entry?
		symbols = malloc(sizeof(symbol_t));																// Yes! Alloc it
		symbols->name = strdup(name);																	// Duplicate the name
		symbols->vaddr = vaddr;
		symbols->flags = flags;
		symbols->next = NULL;
		symbol_count++;																					// Increase the symbol count!
		
		return symbols;
	} else if (get_symbol(name) != NULL) {																// This symbol already exists?
		printf("Error: symbol '%s' redefined\n", name);													// Yes :(
		exit(1);
		return NULL;
	} else {
		symbol_t *last;																					// Let's find the last entry!
		
		for (last = symbols; last->next != NULL; last = last->next) ;
		
		last->next = malloc(sizeof(symbol_t));															// Ok! Alloc some space
		last->next->name = strdup(name);																// Duplicate the name
		last->next->vaddr = vaddr;
		last->next->flags = flags;
		last->next->next = NULL;
		symbol_count++;																					// Increase the symbol count!
		
		return last->next;
	}
}

relocation_t *add_relocation(char *name, uint8_t op, uintptr_t vaddr) {
	if (relocations == NULL) {																			// First entry?
		relocations = malloc(sizeof(relocation_t));														// Yes! Alloc it
		
		if (name != NULL) {																				// Duplicate the name?
			relocations->name = strdup(name);															// Yes!
		} else {
			relocations->name = NULL;
		}
		
		relocations->op = op;
		relocations->vaddr = vaddr;
		relocations->next = NULL;
		relocation_count++;																				// Increase the relocation count!
		
		return relocations;
	} else {
		relocation_t *last;																				// Let's find the last entry!
		
		for (last = relocations; last->next != NULL; last = last->next) ;
		
		last->next = malloc(sizeof(relocation_t));														// Ok! Alloc some space
		
		if (name != NULL) {																				// Duplicate the name?
			last->next->name = strdup(name);															// Yes!
		} else {
			last->next->name = NULL;
		}
		
		last->next->op = op;
		last->next->vaddr = vaddr;
		last->next->next = NULL;
		relocation_count++;																				// Increase the relocation count!
		
		return last->next;
	}
}

dependency_t *add_dependency(char *name) {
	if (dependencies == NULL) {																			// First entry?
		dependencies = malloc(sizeof(dependency_t));													// Yes! Alloc it
		dependencies->name = strdup(name);																// Duplicate the name
		dependencies->next = NULL;
		dependency_count++;																				// Increase the relocation count!
		
		return dependencies;
	} else {
		dependency_t *last;																				// Let's find the last entry!
		
		for (last = dependencies; last->next != NULL; last = last->next) ;
		
		last->next = malloc(sizeof(dependency_t));														// Ok! Alloc some space
		last->next->name = strdup(name);																// Duplicate the name
		last->next->next = NULL;
		dependency_count++;																				// Increase the relocation count!
		
		return last->next;
	}
}

void write_section_data(char *name, uint8_t data) {
	section_t *s = get_section(name);																	// Try to get the section
	
	if (s == NULL) {
		printf("Invalid section '%s'\n", name);															// Failed...
		exit(1);
	}
	
	if ((s->data_count + 1) >= s->data_max) {															// Need to alloc more space?
		s->data = realloc(s->data, s->data_max + 1);													// Yes, realloc the data buffer
		s->data_max++;
		
		if (s->data == NULL) {
			printf("Out of memory\n");																	// Failed :(
			exit(1);
		}
	}
	
	s->data[s->data_count++] = data;
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

uint32_t get_sh_size32(void) {
	uint32_t ret = 0;
	
	for (section_t *s = sections; s != NULL; s = s->next) {												// Let's add the size of each section header!
		ret += sizeof(chexec32_section_t) + strlen(s->name) + 1;										// Size = size of the section header itself + size of then name
	}
	
	return ret;
}

uint64_t get_sh_size64(void) {
	uint64_t ret = 0;																					// 64-bits variant of get_sh_size32
	
	for (section_t *s = sections; s != NULL; s = s->next) {
		ret += sizeof(chexec64_section_t) + strlen(s->name) + 1;
	}
	
	return ret;
}

uint32_t get_st_size32(void) {
	uint32_t ret = 0;
	
	for (symbol_t *s = symbols; s != NULL; s = s->next) {												// Let's add the size of each symbol!
		ret += sizeof(chexec32_symbol_t) + strlen(s->name) + 1;											// Size = size of the symbol struct + size of the name
	}
	
	return ret;
}

uint64_t get_st_size64(void) {
	uint64_t ret = 0;																					// 64-bits variant of get_st_size32
	
	for (symbol_t *s = symbols; s != NULL; s = s->next) {
		ret += sizeof(chexec64_symbol_t) + strlen(s->name) + 1;
	}
	
	return ret;
}

uint32_t get_rel_size32(void) {
	uint32_t ret = 0;
	
	for (relocation_t *r = relocations; r != NULL; r = r->next) {										// Let's add the size of each relocation!
		ret += sizeof(chexec32_relocation_t) + (r->name != 0 ? (strlen(r->name) + 1) : 0);				// Size = size of the relocation struct + size of the name (if the name is not NULL)
	}
	
	return ret;
}

uint64_t get_rel_size64(void) {
	uint64_t ret = 0;																					// 64-bits variant of get_rel_size32
	
	for (relocation_t *r = relocations; r != NULL; r = r->next) {
		ret += sizeof(chexec64_relocation_t) + (r->name != 0 ? (strlen(r->name) + 1) : 0);
	}
	
	return ret;
}

uint32_t get_dep_size32(void) {
	uint32_t ret = 0;
	
	for (dependency_t *d = dependencies; d != NULL; d = d->next) {										// Let's add the size of each dependency!
		ret += sizeof(chexec32_dependency_t) + strlen(d->name) + 1;										// Size = size of the dependency struct + size of the name
	}
	
	return ret;
}

uint64_t get_dep_size64(void) {
	uint64_t ret = 0;																					// 64-bits variant of get_dep_size32
	
	for (dependency_t *d = dependencies; d != NULL; d = d->next) {
		ret += sizeof(chexec64_dependency_t) + strlen(d->name) + 1;
	}
	
	return ret;
}
