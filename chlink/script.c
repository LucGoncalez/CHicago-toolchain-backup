// File author is Ítalo Lima Marconato Matias
//
// Created on February 18 of 2019, at 17:08 BRT
// Last edited on February 24 of 2019, at 15:54 BRT

#include <context.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uintptr_t last_virt = 0;

static void consume_spaces(char **cur, int *line) {
	while (*(*cur) == ' ' || *(*cur) == '\r' || *(*cur) == '\v' || *(*cur) == '\t' || *(*cur) == '\n') {		// Consume the whitespaces
		if (*(*cur) == '\n') {																					// New line?
			(*line)++;																							// Yes
		}
		
		(*cur)++;
	}
}

static int expect_char(char *cur, char **out, char ch) {
	if (*cur != ch) {																							// Ok?
		return 0;																								// Nope >:(
	}
	
	cur++;																										// Do everything we need and return
	*out = cur;
	
	return 1;
}

static char *get_ident(char *cur, char **out) {
	if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
		*cur == '/' || *cur == '.' || *cur == '_' || *cur == '*') {												// Valid identifier?
		uintptr_t i = 1;																						// Yes, let's get the length!
		
		while ((*(cur + i) >= 'a' && *(cur + i) <= 'z') || (*(cur + i) >= 'A' && *(cur + i) <= 'Z') ||
			   (*(cur + i) >= '0' && *(cur + i) <= '9') || *(cur + i) == '/' || *(cur + i) == '.' ||
			   *(cur + i) == '_' || *(cur + i) == '*' || *(cur + i) == '-') {
			i++;
		}
		
		char *lexme = malloc(i + 1);																			// Let's copy it!
		
		if (lexme == NULL) {
			return NULL;																						// Failed
		}
		
		for (uint32_t j = 0; j < i; j++) {
			*(lexme + j) = *cur++;
		}
		
		*(lexme + i) = 0;																						// And finish the string
		*out = cur;
		
		return lexme;
	}
	
	return NULL;
}

static uintmax_t get_num(char *cur, char **out) {
	if (*cur >= '0' && *cur <= '9') {																			// Valid number?
		char *endptr = NULL;																					// Yes, convert from string to num
		uintptr_t val = strtoumax(cur, &endptr, 0);
		
		*out = endptr;																							// Set the new cur
		
		return val;																								// And return
	}
	
	return 0;
}

static context_symbol_t *get_sym(context_t *context, char *name) {
	for (context_symbol_t *sym = context->symbols; sym != NULL; sym = sym->next) {								// Let's search
		if ((strlen(sym->name) == strlen(name)) && !strcmp(sym->name, name)) {									// Found?
			return sym;																							// YES :)
		}
	}
	
	return NULL;
}

static context_section_t *get_sect_real(context_t *context, char *name) {
	for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {						// Let's search
		if ((strlen(sect->name) == strlen(name)) && !strcmp(sect->name, name)) {								// Found?
			return sect;																						// YES :)
		}
	}
	
	return NULL;
}

static context_section_t *add_sect(context_t *context, char *name) {
	context_add_section(context, name, 0, last_virt, NULL, 0);													// Add the section
	return get_sect_real(context, name);
}

static uintptr_t get_sect(context_t *context, char *name) {
	for (context_section_t *sect = context->sections; sect != NULL; sect = sect->next) {						// Let's search
		if ((strlen(sect->name) == strlen(name)) && !strcmp(sect->name, name)) {								// Found?
			return sect->virt;																					// YES :)
		}
	}
	
	return 0;
}

static int parse_section(context_t *src, context_t *context, char *script, char *cur, char **out, int *line, char *name) {
	consume_spaces(&cur, line);																					// Consume whitespaces
	
	if (!expect_char(cur, &cur, '{')) {																			// Expect an '{'
		printf("%s: %d: expected '{' in the SECTION definition\n", script, *line);								// ...
		return 0;
	}
	
	consume_spaces(&cur, line);																					// Consume whitespaces
	
	context_section_t *section = add_sect(context, name);														// Add this section
	
	if (section == NULL) {
		return 0;																								// Failed
	}
	
	while (*cur != '\0' && *cur != '}') {																		// Let's use the same way that we were using in the parse_sections
		if (*cur == '/' && *(cur + 1) == '*') {																	// Comment?
			cur += 2;																							// Yes, let's consume it!
			
			while (*cur != '\0' && !(*cur == '*' && *(cur + 1) == '/')) {
				if (*cur == '\n') {																				// New line?
					(*line)++;																					// Yes
				}
				
				cur++;
			}
			
			if (*cur != '\0') {																					// Consume the comment end tag?
				cur += 2;																						// Yes
			}
		} else if (expect_char(cur, &cur, '*')) {																// Section(s)?
			if (!expect_char(cur, &cur, '(')) {																	// Yes, expect a '('
				printf("%s: %d: expected a '(' after the '*'\n", script, *line);								// ...
				return 0;
			}
			
			consume_spaces(&cur, line);																			// Consume whitespaces
			
			while (*cur != '\0' && *cur != ')') {																// Let's get all the sections!
				char *sectn = get_ident(cur, &cur);																// Get the section name
				
				if (sectn == NULL) {
					printf("%s: %d: expected a section name\n", script, *line);									// ...
					return 0;
				}
				
				context_section_t *sect = get_sect_real(src, sectn);											// Get this section
				
				if (sect != NULL) {																				// Exists?
					context_add_section(context, section->name, sect->size, last_virt, sect->data, 0);			// Yes, append!
					
					for (context_symbol_t *sym = src->symbols; sym != NULL; sym = sym->next) {					// Let's add all the symbols from this section
						if ((strlen(sym->sect) == strlen(sectn)) && !strcmp(sym->sect, sectn)) {				// Same section?
							context_add_symbol(context, sym->name, section->name, sym->type,
											   sym->loc + (last_virt - section->virt), 0);						// Yes, add it!
						}
					}
					
					for (context_reloc_t *rel = src->relocs; rel != NULL; rel = rel->next) {
						if ((strlen(rel->sect) == strlen(sectn)) && !strcmp(rel->sect, sectn)) {				// Same section?
							context_add_relocation(context, rel->name, section->name, rel->size,
												   rel->loc + (last_virt - section->virt), rel->increment,
												   rel->relative);												// Yes, add it!
						}
					}
					
					last_virt += sect->size;																	// Increase the last_virt
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				free(sectn);																					// Free the 'sectn'
			}
			
			if (!expect_char(cur, &cur, ')')) {																	// Yes, expect a '('
				printf("%s: %d: expected a ')'\n", script, *line);												// ...
				return 0;
			}
		} else if (*cur == '\n') {																				// New line?
			cur++;																								// Yes
			(*line)++;
		} else if (*cur == ' ' || *cur == '\r' || *cur == '\v' || *cur == '\t') {								// White space?
			cur++;																								// Yes, consume it
		} else {
			printf("%s: %d: unexpected '%c'\n", script, *line, *cur);
			return 0;
		}
	}
	
	consume_spaces(&cur, line);																					// Consume whitespaces
	
	if (!expect_char(cur, &cur, '}')) {																			// Expect an '}'
		printf("%s: %d: expected '}' after the SECTION definition\n", script, *line);							// ...
		return 0;
	}
	
	*out = cur;
	
	return section->name == name ? 1 : 2;
}

static int parse_sections(context_t *src, context_t *context, char *script, char *cur, char **out, int *line) {
	if (!expect_char(cur, &cur, '{')) {																			// Expect an '{'
		printf("%s: %d: expected '{' after the SECTIONS directive\n", script, *line);							// ...
		return 0;
	}
	
	while (*cur != '\0' && *cur != '}') {																		// Let's use the same way that we were using in the parse_script
		if (*cur == '/' && *(cur + 1) == '*') {																	// Comment?
			cur += 2;																							// Yes, let's consume it!
			
			while (*cur != '\0' && !(*cur == '*' && *(cur + 1) == '/')) {
				if (*cur == '\n') {																				// New line?
					(*line)++;																					// Yes
				}
				
				cur++;
			}
			
			if (*cur != '\0') {																					// Consume the comment end tag?
				cur += 2;																						// Yes
			}
		} else if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
			*cur == '/' || *cur == '.' || *cur == '_') {														// Valid identifier?
			char *lexme = get_ident(cur, &cur);																	// Yes, lex it
			int res = 2;
			
			if (lexme == NULL) {
				printf("Error: failed to link the files\n");													// Failed...
				return 0;
			}
			
			consume_spaces(&cur, line);																			// Consume whitespaces
			
			if (expect_char(cur, &cur, ':')) {																	// Section?
				if (!(res = parse_section(src, context, script, cur, &cur, line, lexme))) {						// Yes, go!
					free(lexme);																				// Failed :(
					return 0;
				}
			} else {
				printf("%s: %d: unimplemented directive '%s'\n", script, *line, lexme);							// Unimplemented
				free(lexme);
				return 0;
			}
			
			if (res == 2) {																						// Free the lexme?
				free(lexme);																					// Yes
			}
		} else if (*cur == '\n') {																				// New line?
			cur++;																								// Yes
			(*line)++;
		} else if (*cur == ' ' || *cur == '\r' || *cur == '\v' || *cur == '\t') {								// White space?
			cur++;																								// Yes, consume it
		} else {
			printf("%s: %d: unexpected '%c'\n", script, *line, *cur);
			return 0;
		}
	}
	
	consume_spaces(&cur, line);																					// Consume whitespaces
				
	if (!expect_char(cur, &cur, '}')) {																			// Expect an '}'
		printf("%s: %d: expected '}' after the SECTIONS directive\n", script, *line);							// ...
		return 0;
	}
	
	*out = cur;
	
	return 1;
}

context_t *parse_script(context_t *src, char *script, char *code) {
	if (src == NULL || script == NULL || code == NULL) {														// Sanity check
		return NULL;
	}
	
	context_t *context = context_new();																			// Create the dest context
	
	if (context == NULL) {
		return NULL;																							// ...
	}
	
	context->arch = src->arch;
	
	char *cur = code;																							// Let's go!
	int line = 0;
	
	while (*cur != '\0') {
		if (*cur == '/' && *(cur + 1) == '*') {																	// Comment?
			cur += 2;																							// Yes, let's consume it!
			
			while (*cur != '\0' && !(*cur == '*' && *(cur + 1) == '/')) {
				if (*cur == '\n') {																				// New line?
					line++;																						// Yes
				}
				
				cur++;
			}
			
			if (*cur != '\0') {																					// Consume the comment end tag?
				cur += 2;																						// Yes
			}
		} else if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
					*cur == '.' || *cur == '_') {																// Valid identifier?
			char *lexme = get_ident(cur, &cur);																	// Yes, lex it
			
			if (lexme == NULL) {
				printf("Error: failed to link the files\n");													// Failed...
				context_free(context);
				return 0;
			}
			
			consume_spaces(&cur, &line);																	// Consume whitespaces
			
			if ((strlen(lexme) == 5) && !strcmp(lexme, "ENTRY")) {												// ENTRY directive?
				if (!expect_char(cur, &cur, '(')) {																// Yes, expect an '('
					printf("%s: %d: expected '(' after the ENTRY directive\n", script, line);					// ...
					free(lexme);
					context_free(context);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
					*cur == '_') {																				// Symbol name after the ENTRY directive?
					char *entry = get_ident(cur, &cur);															// Yes, lex it
					
					if (entry == NULL) {
						printf("Error: failed to link the files\n");											// Failed...
						free(lexme);
						context_free(context);
						return 0;
					}
					
					context_symbol_t *sym = get_sym(src, entry);												// Get the symbol
					
					if (sym == NULL) {
						printf("Cannot find entry symbol '%s', defaulting to 0\n", entry);						// Not found :(
						context->entry = 0;
					} else {
						context->entry = get_sect(src, sym->sect) + sym->loc;									// Found :)
					}
					
					free(entry);																				// Free the 'entry' str
				} else if (*cur >= '0' && *cur <= '9') {														// Address after the ENTRY directive?
					context->entry = get_num(cur, &cur);														// Yes :)
				} else {
					printf("%s: %d: expected variable name or number in the ENTRY directive\n", script, line);
					free(lexme);
					context_free(context);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, ')')) {																// Expect an ')'
					printf("%s: %d: expected ')' after the ENTRY directive\n", script, line);					// ...
					free(lexme);
					context_free(context);
					return 0;
				}
			} else if ((strlen(lexme) == 8) && !strcmp(lexme, "SECTIONS")) {									// SECTIONS directive?
				if (!parse_sections(src, context, script, cur, &cur, &line)) {									// Yes, parse it!
					free(lexme);
					context_free(context);
					return 0;
				}
			} else {
				printf("%s: %d: unimplemented directive '%s'\n", script, line, lexme);							// Unimplemented
				free(lexme);
				context_free(context);
				return 0;
			}
			
			free(lexme);
		} else if (*cur == '\n') {																				// New line?
			cur++;																								// Yes
			line++;
		} else if (*cur == ' ' || *cur == '\r' || *cur == '\v' || *cur == '\t') {								// White space?
			cur++;																								// Yes, consume it
		} else {
			printf("%s: %d: unexpected '%c'\n", script, line, *cur);
			context_free(context);
			return NULL;
		}
	}
	
	for (context_dep_t *dep = src->deps; dep != NULL; dep = dep->next) {										// Now, let's add all the dependencies
		context_add_dep(context, dep->name);
		
		for (context_dep_sym_t *sym = dep->syms; sym != NULL; sym = sym->next) {
			context_add_dep_sym(context, dep->name, sym->name);
		}
	}
	
	return context;
}
