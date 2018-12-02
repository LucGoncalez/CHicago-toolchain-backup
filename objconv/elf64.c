// File author is Ítalo Lima Marconato Matias
//
// Created on November 13 of 2018, at 19:22 BRT
// Last edited on November 15 of 2018, at 13:57 BRT

#include <stddef.h>
#include <stdio.h>
#include <elf.h>

uintptr_t conv_elf64(void *buf, int swap) {
	if (buf == NULL) {									// Sanity checks
		return 0;
	}
	
	printf("Error: unsupported ELF file\n");
	
	return 0;
}
