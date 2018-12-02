// File author is Ítalo Lima Marconato Matias
//
// Created on November 19 of 2018, at 20:06 BRT
// Last edited on November 23 of 2018, at 14:17 BRT

#include <str.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

string_t *str_create(char *str) {
	string_t *ret = malloc(sizeof(string_t));														// Alloc space for the string struct
	
	if (ret == NULL) {
		return NULL;																				// Failed
	}
	
	ret->len = (str == NULL) ? 0 : strlen(str);														// Set the default length
	ret->max = (str == NULL) ? STR_SCALE_FACTOR  : (ret->len * STR_SCALE_FACTOR);					// Set the default max length
	ret->string = malloc(ret->max + 1);																// Alloc the string buffer
	
	if (ret->string == NULL) {
		free(ret);																					// Failed...
		return NULL;
	} else if (str != NULL) {
		strcpy(ret->string, str);																	// Init by copying the string 'str'
	} else {
		ret->string[0] = 0;																			// Init with a NUL character (C string terminator)
	}
	
	return ret;
}

void str_free(string_t *str) {
	if (str == NULL) {																				// Valid string?
		return;																						// Nope
	} else if (str->string != NULL) {																// Free the string buffer?
		free(str->string);																			// Yes
	}
	
	free(str);																						// And free the string struct
}

int str_append_string(string_t *str, char *data) {
	if (str == NULL || data == NULL) {																// Valid string and data?
		return 0;																					// Nope
	}
	
	uint32_t length = strlen(data);																	// Let's save the length of the data string
	
	if (str->len + length >= str->max) {															// Need to alloc more space?
		uint32_t fact = STR_SCALE_FACTOR;															// Yes, let's find the right factor!
		
		while (str->len + length >= str->max * fact) {
			fact *= STR_SCALE_FACTOR;
		}
		
		char *new = realloc(str->string, str->max * fact + 1);										// Now let's try to do it!
		
		if (new == NULL) {
			return 0;																				// Failed...
		}
		
		str->string = new;																			// Ok, set the new string buffer
		str->max *= fact;																			// And the new max length
	}
	
	strcpy(str->string + str->len, data);															// Ok, append the string!
	
	str->len += length;
	
	return 1;
}

int str_append_character(string_t *str, char c) {
	if (str == NULL) {																				// Valid string?
		return 0;																					// Nope
	}
	
	if (str->len + 1 >= str->max) {																	// Need to alloc more space?
		char *new = realloc(str->string, str->max * STR_SCALE_FACTOR + 1);							// Yes, let's try to alloc it
		
		if (new == NULL) {
			return 0;																				// Failed...
		}
		
		str->string = new;																			// Ok, set the new string buffer
		str->max *= STR_SCALE_FACTOR;																// And the new max length
	}
	
	str->string[str->len++] = c;																	// Append the character
	str->string[str->len] = '\0';
	
	return 1;
}

static char *str_append_sint_int(char *buf, int32_t data, uint8_t base) {
	if (data >= 10) {																				// Loop to put all the numbers
		buf = str_append_sint_int(buf, data / base, base);
	}
	
	*buf++ = "0123456789ABCDEF"[data % base];														// Put this one
	
	return buf;
}

static char *str_append_uint_int(char *buf, uint32_t data, uint8_t base) {
	if (data >= 10) {																				// Loop to put all the numbers
		buf = str_append_uint_int(buf, data / base, base);
	}
	
	*buf++ = "0123456789ABCDEF"[data % base];														// Put this one
	
	return buf;
}

int str_append_sint(string_t *str, int32_t data, uint8_t base) {
	if (str == NULL) {																				// Valid string?
		return 0;																					// Nope
	}
	
	if (str->len + 33 >= str->max) {																// Need to alloc more space?
		uint32_t fact = STR_SCALE_FACTOR;															// Yes, let's find the right factor!
		
		while (str->len + 33 >= str->max * fact) {
			fact *= STR_SCALE_FACTOR;
		}
		
		char *new = realloc(str->string, str->max * fact + 1);										// Now let's try to do it!
		
		if (new == NULL) {
			return 0;																				// Failed...
		}
		
		str->string = new;																			// Ok, set the new string buffer
		str->max *= fact;																			// And the new max length
	}
	
	if (data < 0) {																					// Negative number?
		*(str->string + str->len++) = '-';															// Yes, put the neg sign
		data = -data;																				// And convert it to a positive number
	}
	
	char *buf = str_append_sint_int(str->string + str->len, data, base);							// Convert to string!
	
	*buf = '\0';
	str->len += strlen(str->string + str->len);
	
	return 1;
}

int str_append_uint(string_t *str, uint32_t data, uint8_t base) {
	if (str == NULL) {																				// Valid string?
		return 0;																					// Nope
	}
	
	if (str->len + 33 >= str->max) {																// Need to alloc more space?
		uint32_t fact = STR_SCALE_FACTOR;															// Yes, let's find the right factor!
		
		while (str->len + 32 >= str->max * fact) {
			fact *= STR_SCALE_FACTOR;
		}
		
		char *new = realloc(str->string, str->max * fact + 1);										// Now let's try to do it!
		
		if (new == NULL) {
			return 0;																				// Failed...
		}
		
		str->string = new;																			// Ok, set the new string buffer
		str->max *= fact;																			// And the new max length
	}
	
	char *buf = str_append_uint_int(str->string + str->len, data, base);							// Convert to string!
	
	*buf = '\0';
	str->len += strlen(str->string + str->len);
	
	return 1;
}

int str_append_formated(string_t *str, char *data, ...) {
	if (str == NULL || data == NULL) {																// Valid string and data?
		return 0;																					// Nope
	}
	
	va_list va;
	va_start(va, data);																				// Let's start our va list with the arguments provided by the user (if any)
	
	for (uint32_t i = 0; i < strlen(data); i++) {
		if (data[i] != '%') {																		// It's an % (integer, string, character or other)?
			str_append_character(str, data[i]);														// No
		} else {
			switch (data[++i]) {																	// Yes, let's parse it!
			case 's': {																				// String
				if (!str_append_string(str, va_arg(va, char*))) {
					va_end(va);
					return 0;
				}
				
				break;
			}
			case 'c': {																				// Character
				if (!str_append_character(str, (char)va_arg(va, int))) {
					va_end(va);
					return 0;
				}
				
				break;
			}
			case 'd': {																				// Signed decimal number
				if (!str_append_sint(str, va_arg(va, int32_t), 10)) {
					va_end(va);
					return 0;
				}
				
				break;
			}
			case 'u': {																				// Unsigned decimal number
				if (!str_append_uint(str, va_arg(va, uint32_t), 10)) {
					va_end(va);
					return 0;
				}
				
				break;
			}
			case 'x': {																				// Unsigned hexadecimal number
				if (!str_append_uint(str, va_arg(va, uint32_t), 16)) {
					va_end(va);
					return 0;
				}
				
				break;
			}
			default: {																				// Probably it's another % (probably)
				if (!str_append_character(str, data[i])) {
					va_end(va);
					return 0;
				}
				
				break;
			}
			}
		}
	}
	
	va_end(va);
	
	return 1;
}
