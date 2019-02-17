// File author is Ítalo Lima Marconato Matias
//
// Created on February 18 of 2019, at 13:01 BRT
// Last edited on February 18 of 2019, at 13:36 BRT

#include <script.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

linker_options_t *linker_options_new() {
	return calloc(1, sizeof(linker_options_t));																		// Alloc and return
}

linker_section_t *linker_add_section(linker_options_t *options, char *name) {
	if (options == NULL || name == NULL) {																			// Sanity check
		return NULL;
	}
	
	linker_section_t *cur = options->sections;																		// Let's search for the last entry (and if this section doens't exists)!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {										// Found?
			return cur;																								// Yes, so it already exists
		} else if (cur->next == NULL) {																				// We need to create this section?
			break;																									// Yes
		}
	}
	
	if (cur != NULL) {																								// First section?
		cur->next = calloc(1, sizeof(linker_section_t));															// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(linker_section_t));																	// Yes, alloc it
	}
	
	if (cur == NULL) {
		return NULL;																								// Failed to alloc
	}
	
	cur->name = name;																								// Set the name
	
	return cur;
}

int linker_add_section_wildcard(linker_section_t *section, char *name) {
	if (section == NULL || name == NULL) {																			// Sanity checks
		return 0;
	}
	
	char *prefix = name;
	char *postfix = NULL;
	
	for (size_t i = 0; i < strlen(name); i++) {																		// Let's search for the *
		if (prefix[i] == '*') {																						// Found?
			prefix[i] = '\0';																						// Yes :)
			postfix = prefix + i + 1;
			break;
		}
	}
	
	linker_section_wildcard_t *cur = section->sections;																// Let's search for the last entry (and if this section doens't exists)!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(postfix) == strlen(cur->postfix)) && (strlen(prefix) == strlen(cur->prefix)) &&
			!strcmp(postfix, cur->postfix) && !strcmp(prefix, cur->prefix)) {										// Found?
			return 0;																								// Yes, so it already exists
		} else if (cur->next == NULL) {																				// We need to create this section?
			break;																									// Yes
		}
	}
	
	if (cur != NULL) {																								// First section?
		cur->next = calloc(1, sizeof(linker_section_wildcard_t));													// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(linker_section_wildcard_t));															// Yes, alloc it
	}
	
	if (cur == NULL) {
		return 0;																									// Failed to alloc
	}
	
	cur->postfix = postfix;																							// Set the postfix and the prefix
	cur->prefix = prefix;
	
	return 1;
}

int linker_assign_variable(linker_options_t *options, char *name, uintmax_t num, char *sym) {
	if (options == NULL || name == NULL) {																			// Sanity check
		return 0;
	} else if ((strlen(name) == 1) && name[0] == '.') {																// Variable = cur?
		options->cur.value_num = num;																				// Yes :)
		options->cur.value_sym = sym;
		options->cur.value_type = sym != NULL;
		return 0;
	}
	
	linker_variable_t *cur = options->vars;																			// Let's search for the last entry (and if this variable doens't exists)!
	
	for (; cur != NULL; cur = cur->next) {
		if ((strlen(name) == strlen(cur->name)) && !strcmp(name, cur->name)) {										// Found?
			cur->value_num = num;																					// Yes, so just assign everything
			cur->value_sym = sym;
			cur->value_type = sym != NULL;
			return 0;
		} else if (cur->next == NULL) {																				// We need to create this section?
			break;																									// Yes
		}
	}
	
	if (cur != NULL) {																								// First variable?
		cur->next = calloc(1, sizeof(linker_variable_t));															// Nope, alloc it and save as the last entry
		cur = cur->next;
	} else {
		cur = calloc(1, sizeof(linker_variable_t));																	// Yes, alloc it
	}
	
	if (cur == NULL) {
		return 0;																									// Failed to alloc
	}
	
	cur->name = name;																								// Assign everything
	cur->value_num = num;
	cur->value_sym = sym;
	cur->value_type = sym != NULL;
	
	return 1;
}

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

static int parse_section(char *script, char *cur, char **out, int *line, char *name, uintmax_t vaddr, uintmax_t align, linker_options_t *options) {
	linker_section_t *section = linker_add_section(options, name);												// Create the section
	
	if (section == NULL) {
		printf("Error: failed to link the files\n");															// Failed :(
		return 0;
	}
	
	section->vaddr = vaddr;
	section->align = align;
	
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
				char *sect = get_ident(cur, &cur);																// Get the section name
				
				if (sect == NULL) {
					printf("%s: %d: expected a section name\n", script, *line);									// ...
					return 0;
				}
				
				linker_add_section_wildcard(section, sect);														// Add it!
				consume_spaces(&cur, line);																		// Consume whitespaces
			}
			
			if (!expect_char(cur, &cur, ')')) {																	// Yes, expect a '('
				printf("%s: %d: expected a ')'\n", script, *line);												// ...
				return 0;
			}
		} else if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
			*cur == '/' || *cur == '.' || *cur == '_') {														// Valid identifier?
			char *lexme = get_ident(cur, &cur);																	// Yes, lex it
			
			if (lexme == NULL) {
				printf("Error: failed to link the files\n");													// Failed...
				return 0;
			}
			
			consume_spaces(&cur, line);																			// Consume whitespaces
				
			if (!expect_char(cur, &cur, '=')) {																	// Expect an '='
				printf("%s: %d: expected '=' after the variable name\n", script, *line);						// ...
				free(lexme);
				return 0;
			}
			
			consume_spaces(&cur, line);																			// Consume whitespaces
			
			if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
				*cur == '.' || *cur == '_') {																	// Symbol name?
				char *val = get_ident(cur, &cur);																// Yes, lex it
				
				if (val == NULL) {
					printf("Error: failed to link the files\n");												// Failed...
					free(lexme);
					return 0;
				}
				
				if (!linker_assign_variable(options, lexme, 0, val)) {											// Assign!
					free(lexme);
				}
			} else if (*cur >= '0' && *cur <= '9') {															// Number?
				linker_assign_variable(options, lexme, get_num(cur, &cur), NULL);								// Yes, assign!
			} else {
				printf("%s: %d: expected variable name or number in the variable assignment\n", script, *line);
				free(lexme);
				return 0;
			}
			
			consume_spaces(&cur, line);																			// Consume whitespaces
			
			if (!expect_char(cur, &cur, ';')) {																	// Expect an ';'
				printf("%s: %d: expected ';' after the variable assignemnt\n", script, *line);					// ...
				free(lexme);
				return 0;
			}
			
			free(lexme);																						// And free it
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
	
	*out = cur;
	
	return section->name == name ? 1 : 2;
}

static int parse_sections(char *script, char *cur, char **out, int *line, linker_options_t *options) {
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
			char *align = NULL;
			int res = 2;
			
			if (lexme == NULL) {
				printf("Error: failed to link the files\n");													// Failed...
				return 0;
			}
			
			consume_spaces(&cur, line);																			// Consume whitespaces
			
			if ((align = get_ident(cur, &cur)) != NULL) {														// Section? (With the ALIGN directive)
				uintmax_t alignn = 0;																			// Yes!
				
				if ((strlen(align) != 5) || strcmp(align, "ALIGN")) {											// Let's make sure that it's a ALIGN directive
					printf("%s: %d: expected ALIGN directive\n", script, *line);								
					free(align);
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces

				if (!expect_char(cur, &cur, '(')) {																// Yes, expect an '('
					printf("%s: %d: expected '(' after the ALIGN directive\n", script, line);					// ...
					free(align);
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (*cur >= '0' && *cur <= '9') {																// Number after the ALIGN directive?
					alignn = get_num(cur, &cur);																// Yes, lex it
				} else {
					printf("%s: %d: expected number in the ALIGN directive\n", script, line);
					free(align);
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, ')')) {																// Expect an ')'
					printf("%s: %d: expected ')' after the ALIGN directive\n", script, line);					// ...
					free(align);
					free(lexme);
					return 0;
				}
				
				free(align);
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, ':')) {																// Expect an ':'
					printf("%s: %d: expected ':' in the SECTION definition\n", script, *line);					// ...
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Yes, consume whitespaces
				
				char *vaddr = get_ident(cur, &cur);																// Let's see if we have a VADDR directive
				uintmax_t vaddrn = 0;
				
				if (vaddr != NULL) {
					if ((strlen(vaddr) != 5) || strcmp(vaddr, "VADDR")) {										// Check if it's really the VADDR directive
						printf("%s: %d: expected VADDR directive\n", script, *line);
						free(vaddr);
						free(lexme);
						return 0;
					}
					
					consume_spaces(&cur, line);																	// Consume whitespaces

					if (!expect_char(cur, &cur, '(')) {															// Yes, expect an '('
						printf("%s: %d: expected '(' after the VADDR directive\n", script, line);				// ...
						free(vaddr);
						free(lexme);
						return 0;
					}

					consume_spaces(&cur, line);																	// Consume whitespaces

					if (*cur >= '0' && *cur <= '9') {															// Address after the VADDR directive?
						vaddrn = get_num(cur, &cur);															// Yes, lex it
					} else {
						printf("%s: %d: expected number in the VADDR directive\n", script, line);
						free(vaddr);
						free(lexme);
						return 0;
					}
					
					consume_spaces(&cur, line);																	// Consume whitespaces

					if (!expect_char(cur, &cur, ')')) {															// Expect an ')'
						printf("%s: %d: expected ')' after the VADDR directive\n", script, line);				// ...
						free(vaddr);
						free(lexme);
						return 0;
					}
					
					free(vaddr);
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, '{')) {																// Expect an '{'
					printf("%s: %d: expected '{' in the SECTION definition\n", script, *line);					// ...
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!(res = parse_section(script, cur, &cur, line, lexme, vaddrn, alignn, options))) {			// Do it!
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, '}')) {																// Expect an '}'
					printf("%s: %d: expected '}' after the SECTION definition\n", script, *line);				// ...
					free(lexme);
					return 0;
				}
			} else if (expect_char(cur, &cur, ':')) {															// Section? (Without the ALIGN directive)
				consume_spaces(&cur, line);																		// Yes, consume whitespaces
				
				char *vaddr = get_ident(cur, &cur);																// Let's see if we have a VADDR directive
				uintmax_t vaddrn = 0;
				
				if (vaddr != NULL) {
					if ((strlen(vaddr) != 5) || strcmp(vaddr, "VADDR")) {										// Check if it's really the VADDR directive
						printf("%s: %d: expected VADDR directive\n", script, *line);
						free(vaddr);
						free(lexme);
						return 0;
					}
					
					consume_spaces(&cur, line);																	// Consume whitespaces

					if (!expect_char(cur, &cur, '(')) {															// Yes, expect an '('
						printf("%s: %d: expected '(' after the VADDR directive\n", script, line);				// ...
						free(vaddr);
						free(lexme);
						return 0;
					}

					consume_spaces(&cur, line);																	// Consume whitespaces

					if (*cur >= '0' && *cur <= '9') {															// Address after the VADDR directive?
						vaddrn = get_num(cur, &cur);															// Yes, lex it
					} else {
						printf("%s: %d: expected number in the VADDR directive\n", script, line);
						free(vaddr);
						free(lexme);
						return 0;
					}
					
					consume_spaces(&cur, line);																	// Consume whitespaces

					if (!expect_char(cur, &cur, ')')) {															// Expect an ')'
						printf("%s: %d: expected ')' after the VADDR directive\n", script, line);				// ...
						free(vaddr);
						free(lexme);
						return 0;
					}
					
					free(vaddr);
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, '{')) {																// Expect an '{'
					printf("%s: %d: expected '{' in the SECTION definition\n", script, *line);					// ...
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!(res = parse_section(script, cur, &cur, line, lexme, vaddrn, 1, options))) {				// Do it!
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, '}')) {																// Expect an '}'
					printf("%s: %d: expected '}' after the SECTION definition\n", script, *line);				// ...
					free(lexme);
					return 0;
				}
			} else if (expect_char(cur, &cur, '=')) {															// Variable assignment?
				consume_spaces(&cur, line);																		// Yes, consume whitespaces
				
				if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
					*cur == '.' || *cur == '_') {																// Symbol name?
					char *val = get_ident(cur, &cur);															// Yes, lex it
					
					if (val == NULL) {
						printf("Error: failed to link the files\n");											// Failed...
						free(lexme);
						return 0;
					}
					
					if (linker_assign_variable(options, lexme, 0, val)) {										// Assign!
						res = 0;
					}
				} else if (*cur >= '0' && *cur <= '9') {														// Number?
					linker_assign_variable(options, lexme, get_num(cur, &cur), NULL);							// Yes, assign!
				} else {
					printf("%s: %d: expected variable name or number in the variable assignment\n", script, *line);
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, line);																		// Consume whitespaces
				
				if (!expect_char(cur, &cur, ';')) {																// Expect an ';'
					printf("%s: %d: expected ';' after the variable assignemnt\n", script, *line);				// ...
					free(lexme);
					return 0;
				}
			} else {
				printf("%s: %d: expected '=' after the variable name\n", script, *line);						// ...
				free(lexme);
				return 0;
			}
			
			if (res == 2) {
				free(lexme);
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
	
	*out = cur;
	
	return 1;
}

int linker_parse_script(linker_options_t *options, char *script, char *code) {
	if (script == NULL || code == NULL || options == NULL) {													// Sanity check
		return 0;
	}
	
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
			int res = 2;
			char *lexme = get_ident(cur, &cur);																	// Yes, lex it
			
			if (lexme == NULL) {
				printf("Error: failed to link the files\n");													// Failed...
				return 0;
			}
			
			if ((strlen(lexme) == 5) && !strcmp(lexme, "ENTRY")) {												// ENTRY directive?
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, '(')) {																// Yes, expect an '('
					printf("%s: %d: expected '(' after the ENTRY directive\n", script, line);					// ...
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
					*cur == '_') {																				// Symbol name after the ENTRY directive?
					char *entry = get_ident(cur, &cur);															// Yes, lex it
					
					if (entry == NULL) {
						printf("Error: failed to link the files\n");											// Failed...
						free(lexme);
						return 0;
					}
					
					options->entry_sym = entry;
					options->entry_type = 1;
				} else if (*cur >= '0' && *cur <= '9') {														// Address after the ENTRY directive?
					options->entry_num = get_num(cur, &cur);													// Yes :)
					options->entry_type = 0;
				} else {
					printf("%s: %d: expected variable name or number in the ENTRY directive\n", script, line);
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, ')')) {																// Expect an ')'
					printf("%s: %d: expected ')' after the ENTRY directive\n", script, line);					// ...
					free(lexme);
					return 0;
				}
			} else if ((strlen(lexme) == 8) && !strcmp(lexme, "SECTIONS")) {									// SECTIONS directive?
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, '{')) {																// Yes, expect an '{'
					printf("%s: %d: expected '{' after the SECTIONS directive\n", script, line);				// ...
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!parse_sections(script, cur, &cur, &line, options)) {										// Parse the SECTIONS directive!
					free(lexme);																				// ...
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, '}')) {																// Expect an '}'
					printf("%s: %d: expected '}' after the SECTIONS directive\n", script, line);				// ...
					free(lexme);
					return 0;
				}
			} else {
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, '=')) {																// Expect an '='
					printf("%s: %d: expected '=' after the variable name\n", script, line);						// ...
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z') ||
					*cur == '.' || *cur == '_') {																// Symbol name?
					char *val = get_ident(cur, &cur);															// Yes, lex it
					
					if (val == NULL) {
						printf("Error: failed to link the files\n");											// Failed...
						free(lexme);
						return 0;
					}
					
					if (linker_assign_variable(options, lexme, 0, val)) {										// Assign!
						res = 0;
					}
				} else if (*cur >= '0' && *cur <= '9') {														// Number?
					linker_assign_variable(options, lexme, get_num(cur, &cur), NULL);							// Yes, assign!
				} else {
					printf("%s: %d: expected variable name or number in the variable assignment\n", script, line);
					free(lexme);
					return 0;
				}
				
				consume_spaces(&cur, &line);																	// Consume whitespaces
				
				if (!expect_char(cur, &cur, ';')) {																// Expect an ';'
					printf("%s: %d: expected ';' after the variable assignemnt\n", script, line);				// ...
					free(lexme);
					return 0;
				}
			}
			
			if (res == 2) {
				free(lexme);
			}
		} else if (*cur == '\n') {																				// New line?
			cur++;																								// Yes
			line++;
		} else if (*cur == ' ' || *cur == '\r' || *cur == '\v' || *cur == '\t') {								// White space?
			cur++;																								// Yes, consume it
		} else {
			printf("%s: %d: unexpected '%c'\n", script, line, *cur);
			return 0;
		}
	}
	
	return 1;
}
