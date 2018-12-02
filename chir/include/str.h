// File author is Ítalo Lima Marconato Matias
//
// Created on November 19 of 2018, at 20:04 BRT
// Last edited on November 23 of 2018, at 13:31 BRT

#ifndef __STR_H__
#define __STR_H__

#include <stdint.h>

#define STR_SCALE_FACTOR 2

typedef struct {
	uint32_t len;
	uint32_t max;
	char *string;
} string_t;

string_t *str_create(char *str);
void str_free(string_t *str);
int str_append_character(string_t *str, char c);
int str_append_string(string_t *str, char *data);
int str_append_sint(string_t *str, int32_t data, uint8_t base);
int str_append_uint(string_t *str, uint32_t data, uint8_t base);
int str_append_formated(string_t *str, char *data, ...);

#endif		// __STR_H__
