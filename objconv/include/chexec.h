// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 09:52 BRT
// Last edited on November 16 of 2018, at 09:57 BRT

#ifndef __CHEXEC_H__
#define __CHEXEC_H__

#include <stdint.h>

#define CHEXEC_HEADER_MAGIC 0x58454843

#define CHEXEC_HEADER_FLAGS_ARCH_X86 0x01

#define CHEXEC_HEADER_FLAGS_LIBRARY 0x02
#define CHEXEC_HEADER_FLAGS_EXECUTABLE 0x04

#define CHEXEC_SECTION_FLAGS_NONE 0x01
#define CHEXEC_SECTION_FLAGS_ZEROINIT 0x02

#define CHEXEC_SYMBOL_FLAGS_NONE 0x01
#define CHEXEC_SYMBOL_FLAGS_UNDEF 0x02

typedef struct {
	uint32_t magic;
	uint32_t flags;
	uint32_t entry;
	uint32_t sh_count;
	uint32_t sh_start;
	uint32_t st_count;
	uint32_t st_start;
	uint32_t rel_count;
	uint32_t rel_start;
	uint32_t dep_count;
	uint32_t dep_start;
} chexec32_header_t;

typedef struct {
	uint32_t magic;
	uint32_t flags;
	uint64_t entry;
	uint64_t sh_count;
	uint64_t sh_start;
	uint64_t st_count;
	uint64_t st_start;
	uint64_t rel_count;
	uint64_t rel_start;
	uint64_t dep_count;
	uint64_t dep_start;
} chexec64_header_t;

typedef struct {
	uint32_t flags;
	uint32_t offset;
	uint32_t virt;
	uint32_t size;
	uint32_t name_len;
	char name[0];
} chexec32_section_t;

typedef struct {
	uint32_t flags;
	uint64_t offset;
	uint64_t virt;
	uint64_t size;
	uint64_t name_len;
	char name[0];
} chexec64_section_t;

typedef struct {
	uint32_t flags;
	uint32_t virt;
	uint32_t name_len;
	char name[0];
} chexec32_symbol_t;

typedef struct {
	uint32_t flags;
	uint64_t virt;
	uint64_t name_len;
	char name[0];
} chexec64_symbol_t;

typedef struct {
	uint8_t op;
	uint32_t virt;
	uint32_t name_len;
	char name[0];
} chexec32_relocation_t;

typedef struct {
	uint8_t op;
	uint64_t virt;
	uint64_t name_len;
	char name[0];
} chexec64_relocation_t;

typedef struct {
	uint32_t name_len;
	char name[0];
} chexec32_dependency_t;

typedef struct {
	uint64_t name_len;
	char name[0];
} chexec64_dependency_t;

#endif		// __CHEXEC_H__
