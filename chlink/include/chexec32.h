// File author is Ítalo Lima Marconato Matias
//
// Created on January 27 of 2019, at 13:52 BRT
// Last edited on February 22 of 2019, at 21:24 BRT

#ifndef __CHEXEC32_H__
#define __CHEXEC32_H__

#include <stdint.h>
#include <wchar.h>

#define CHEXEC32_HEADER_FLAGS_ARCH_X86 0x01
#define CHEXEC32_HEADER_FLAGS_LIBRARY 0x02
#define CHEXEC32_HEADER_FLAGS_EXECUTABLE 0x04
#define CHEXEC32_HEADER_FLAGS_DRIVER 0x08
#define CHEXEC32_HEADER_FLAGS_KERNEL 0x10

#define CHEXEC32_SECTION_FLAGS_NONE 0x01
#define CHEXEC32_SECTION_FLAGS_ZEROINIT 0x02

#define CHEXEC32_SYM_FLAGS_LOC 0x00
#define CHEXEC32_SYM_FLAGS_NONE 0x01
#define CHEXEC32_SYM_FLAGS_UNDEF 0x02

#define CHEXEC32_REL_OP_ABS 0x00
#define CHEXEC32_REL_OP_REL 0x01
#define CHEXEC32_REL_OP_SYM 0x02
#define CHEXEC32_REL_OP_REL_SYM 0x04
#define CHEXEC32_REL_OP_BYTE 0x08
#define CHEXEC32_REL_OP_WORD 0x10
#define CHEXEC32_REL_OP_DWORD 0x20

typedef struct {
	uint32_t magic;
	uint16_t flags;
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
	uint16_t flags;
	uint32_t offset;
	uint32_t virt;
	uint32_t size;
	uint32_t name_len;
	wchar_t name[0];
} chexec32_section_t;

typedef struct {
	uint16_t flags;
	uint32_t virt;
	uint32_t name_len;
	wchar_t name[0];
} chexec32_sym_t;

typedef struct {
	uint8_t op;
	uint32_t incr;
	uint32_t virt;
	uint32_t name_len;
	wchar_t name[0];
} chexec32_rel_t;

typedef struct {
	uint32_t name_len;
	wchar_t name[0];
} chexec32_dep_t;

#endif		// __CHEXEC32_H__
