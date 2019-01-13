// File author is Ítalo Lima Marconato Matias
//
// Created on December 02 of 2018, at 17:37 BRT
// Last edited on January 12 of 2019, at 19:11 BRT

#include <arch.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <x86.h>

static char *registers[70] = {
	"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
	"ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
	"ah", "bh", "ch", "dh",
	"al", "bl", "cl", "dl",
	"cs", "ds", "es", "fs", "gs", "ss",
	"cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7",
	"dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7",
	"tr0", "tr1", "tr2", "tr3", "tr4", "tr5", "tr6", "tr7",
	"st0", "st1", "st2", "st3", "st4", "st5", "st6", "st7",
	"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
};

static char *sregs[6] = { "es", "cs", "ss", "ds", "fs", "gs" };
static char *cregs[8] = { "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7" };
static char *dregs[8] = { "dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7" };
static char *tregs[8] = { "dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7" };
static char *gregsb[8] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
static char *gregsw[8] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
static char *gregsd[8] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
static char *stregs[8] = { "st0", "st1", "st2", "st3", "st4", "st5", "st6", "st7" };
static char *xregs[8] = { "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7" };

static char *mnemonics[765] = {
	"aaa", "aad", "aam", "aas", "adc", "adcb", "adcw", "adcd", "adcx", "add", "addb", "addw", "addd",
	"addpd", "addps", "addsd", "addss", "addsubpd", "addsubps", "adox", "aesdec", "aesdeclast", "aesenc",
	"aesenclast", "aesimc", "aeskeygenassist", "and", "andb", "andw", "andd", "andn", "andnpd", "andnps",
	"andpd", "andps", "arpl", "bextr", "blendpd", "blendps", "blendvpd", "blendvps", "blsi", "blsmask", "blsr",
	"bound", "bsf", "bsr", "bswap", "bt", "btw", "btd", "btc", "btcw", "btcd", "btr", "btrw", "btrd", "bts", "btsw",
	"btsd", "bzhi", "call", "cbw", "cwde", "clac", "clc", "cld", "clflush", "clflushopt", "cli", "clts", "clwb",
	"cmc", "cmova", "cmovae", "cmovb", "cmovbe", "cmovc", "cmove", "cmovg", "cmovge", "cmovl", "cmovle", "cmovna",
	"cmovnae", "cmovnb", "cmovnbe", "cmovnc", "cmovne", "cmovng", "cmovnge", "cmovnl", "cmovnle", "cmovno", "cmovnp",
	"cmovns", "cmovnz", "cmovo", "cmovp", "cmovpe", "cmovpo", "cmovs", "cmovz", "cmp", "cmpb", "cmpw", "cmpd", "cmppd",
	"cmpps", "cmpsb", "cmpsw", "cmpsd", "cmpss", "cmpxchg", "cmpxchg8b", "comisd", "comiss", "cpuid", "crc32b", "crc32w",
	"crc32d", "cvtdq2pd", "cvtdq2ps", "cvtpd2dq", "cvtpd2ps", "cvtps2dq", "cvtps2pd", "cvtsd2si", "cvtsd2ss", "cvtsi2sd",
	"cvtsi2ss", "cvtss2sd", "cvtss2si", "cvttpd2dq", "cvttps2dq", "cvttsd2si", "cvttss2si", "cwd", "cdq", "daa", "das",
	"dec", "div", "divpd", "divps", "divsd", "divss", "dppd", "dpps", "enter", "extractps", "f2xm1", "fabs", "fadd",
	"faddd", "faddq", "faddp", "fiaddd", "fiaddw", "fbld", "fbstp", "fchs", "fclex", "fnclex", "fcmovb", "fcmove", "fcmovbe",
	"fcmovu", "fcmovnb", "fcmovne", "fcmovnbe", "fcmovnu", "fcomd", "fcomq", "fcom", "fcompd", "fcompq", "fcomp", "fcompp",
	"fcomi", "fcomip", "fucomi", "fucomip", "fcos", "fdecstp", "fdivd", "fdivq", "fdiv", "fdivp", "fidivd", "fidivw",
	"fdivrd", "fdivrq", "fdivr", "fdivrp", "fidivrd", "fidivrw", "ffree", "ficomw", "ficomd", "ficompw", "ficompd", "fildw",
	"fildd", "fildq", "fincstp", "finit", "fninit", "fistw", "fistd", "fistpw", "fistpd", "fistpq", "fisttpw", "fisttpd",
	"fisttpq", "fldd", "fldq", "fldt", "fld", "fld1", "fldl2t", "fldl2e", "fldpi", "fldlg2", "fldln2", "fldz", "fldcw",
	"fldenv", "fmuld", "fmulq", "fmul", "fmulp", "fimuld", "fimulw", "fnop", "fpatan", "fprem", "fprem1", "fptan", "frndint",
	"frstor", "fsave", "fnsave", "fscale", "fsin", "fsincos", "fsqrt", "fstd", "fstq", "fst", "fstpd", "fstpq", "fstpt",
	"fstp", "fstcw", "fnstcw", "fstenv", "fnstenv", "fstsw", "fnstsw", "fsubd", "fsubq", "fsub", "fsubp", "fisubd", "fisubw",
	"fsubrd", "fsubrq", "fsubr", "fsubrp", "fisubrd", "fisubrw", "ftst", "fucom", "fucomp", "fucompp", "fxam", "fxch",
	"fxrstor", "fxsave", "fxtract", "fyl2x", "fyl2xp1", "haddpd", "haddps", "hlt", "hsubpd", "hsubps", "idivb", "idivw",
	"idivd", "imul", "imulb", "imulw", "imuld", "in", "inc", "incb", "incw", "incd", "insb", "insw", "insd", "insertps",
	"int3", "int", "into", "invd", "invlpg", "invpcid", "iret", "iretw", "iretd", "nja", "njae", "njb", "njbe", "njc", "njcxz",
	"njecxz", "nje", "njz", "njg", "njge", "njl", "njle", "njna", "njnae", "njnb", "njnbe", "njnc", "njne", "njng", "njnge",
	"njnl", "njnle", "njno", "njnp", "njns", "njnz", "njo", "njp", "njpe", "njpo", "njs", "njz", "njmp", "ja", "jae", "jb",
	"jbe", "jc", "jcxz", "jecxz", "je", "jz", "jg", "jge", "jl", "jle", "jna", "jnae", "jnb", "jnbe", "jnc", "jne", "jng",
	"jnge", "jnl", "jnle", "jno", "jnp", "jns", "jnz", "jo", "jp", "jpe", "jpo", "js", "jz", "jmp", "lahf", "lar", "lddqu",
	"ldmxcsr", "lea", "leave", "lfence", "lgdt", "lidt", "lds", "lss", "les", "lfs", "lgs", "lldt", "lmsw", "lodsb", "lodsw",
	"lodsd", "loop", "loope", "loopz", "loopne", "loopnz", "lsl", "ltr", "lzcnt", "maskmovdqu", "maxpd", "maxps", "maxsd",
	"maxss", "mfence", "minpd", "minps", "minsd", "minss", "monitor", "mov", "movb", "movw", "movd", "movapd", "movaps",
	"movbe", "movd", "movddup", "movdqa", "movdqu", "movhlps", "movlhps", "movhpd", "movhps", "movlpd", "movlps", "movmskpd",
	"movmskps", "movntdqa", "movntdq", "movnti", "movntpd", "movntps", "movq", "movsb", "movsw", "movsd", "movsd", "movss",
	"movshdup", "movsldup", "movsxb", "movsxw", "movupd", "movups", "movzxb", "movzxw", "mpsadbw", "mul", "mulpd", "mulps",
	"mulsd", "mulss", "mulx", "mwait", "neg", "nop", "not", "or", "orb", "orw", "ord", "orpd", "orps", "out", "outsb",
	"outsw", "outsd", "pabsb", "pabsw", "pabsd", "packsswb", "packssdw", "packuswb", "packusdw", "paddb", "paddw", "paddd",
	"paddsb", "paddsw", "paddusb", "paddusw", "palignr", "pand", "pandn", "pause", "pavgb", "pavgw", "pblendvb", "pblendw",
	"pclmulqdq", "pcmpeqb", "pcmpeqw", "pcmpeqd", "pcmpdqq", "pcmpestri", "pcmpestrm", "pcmpgtb", "pcmpgtw", "pcmpgtd",
	"pcmpgtq", "pcmpistri", "pcmpistrm", "pextrb", "pextrw", "pextrd", "phaddw", "phaddd", "phaddsw", "phminposuw", "phsubw",
	"phsubd", "phsubsw", "pinsrb", "pinsrw", "pinsrd", "pmaddubsw", "pmaddwd", "pmaxsb", "pmaxsw", "pmaxsd", "pmaxub", "pmaxuw",
	"pmaxud", "pminsb", "pminsw", "pminsd", "pminub", "pminuw", "pminud", "pmovmskb", "pmovsxbw", "pmovsxbd", "pmovsxbq",
	"pmovsxwd", "pmovsxwq", "pmovsxdq", "pmovzxbw", "pmovzxbd", "pmovzxbq", "pmovszwd", "pmovszwq", "pmovzxdq", "pmuldq",
	"pmulhrsw", "pmulhuw", "pmulhw", "pmullw", "pmulld", "pmuludq", "pop", "popa", "popaw", "popad", "popcnt", "popf", "popfw",
	"popfd", "por", "prefetcht0", "prefetcht1", "prefetcht2", "prefetchnta", "prefetchw", "psadbw", "psignb", "psignw", "psignd",
	"pslldq", "psllw", "pslld", "psllq", "psraw", "psrad", "psrldq", "psrlw", "psrld", "psrlq", "psubb", "psubw", "psubd",
	"psubq", "psubsb", "psubsw", "psubusb", "psubusw", "ptest", "punpckhbw", "punpckhwd", "punpckhqdq", "punpcklbw", "punpcklwd",
	"punpckldq", "punpcklqdq", "push", "pusha", "pushaw", "pushad", "pushf", "pushfw", "pushfd", "pxor", "rclb", "rclw", "rcld",
	"rcrb", "rcrw", "rcrd", "rolb", "rolw", "rold", "rorb", "rorw", "rord", "rcpps", "rcpss", "rdmsr", "rdpid", "rdpkru", "rdpmc",
	"rdrand", "rdseed", "rdtsc", "rdtscp", "ret", "retf", "retfw", "retfd", "roundpd", "roundps", "roundsd", "roundss", "rsm",
	"rsqrtps", "rsqrtss", "sahf", "salb", "salw", "sald", "sarb", "sarw", "sard", "shlb", "shlw", "shld", "shrb", "shrw", "shrd",
	"sbb", "sbbb", "sbbw", "sbbd", "scasb", "scasw", "scasd", "seta", "setae", "setb", "setbe", "setc", "sete", "setg", "setge",
	"setl", "setle", "setna",  "setnae", "setnb", "setnbe", "setnc", "setne", "setng", "setnge", "setnl", "setnle", "setno",
	"setnp", "setns", "setnz", "seto", "setp", "setpe", "setpo", "sets", "setz", "sfence", "sgdt", "sidt", "sldt", "sha1rnds4",
	"sha1nexte", "sha1msg1", "sha1msg2", "sha256rnds2", "sha256msg1", "sha256msg2", "shld", "shrd", "shufpd", "shufps", "smsw",
	"sqrtpd", "sqrtps", "sqrtsd", "sqrtss", "stac", "stc", "std", "sti", "stmxcsr", "stosb", "stosw", "stosd", "str", "sub",
	"subb", "subw", "subd", "subpd", "subps", "subsd", "subss", "sysenter", "sysexit", "test", "testb", "testw", "testd", "tzcnt",
	"ucomisd", "ucomiss", "ud0", "ud1", "ud2", "unpckhpd", "unpckhps", "unpcklpd", "unpcklps", "verr", "verw", "wait", "wbinvd",
	"wrmsr", "wrpkru", "xadd", "xchg", "xlatb", "xlat", "xor", "xorb", "xorw", "xord", "xorpd", "xorps"
};

struct {
	char *name;
	int opcode;
	int postop;
	int optype;
	int alt_addr;
	int extension;
	uint32_t args;
	uint32_t arg1;
	uint32_t arg2;
	uint32_t arg3;
} instructions[1079] = {
	{ "aaa", 0x37, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aad", 0xD5, 0x0A, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "aad", 0xD5, -1, INSTR_TYPE_BYTE, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aam", 0xD4, 0x0A, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "aam", 0xD5, -1, INSTR_TYPE_BYTE, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "aas", 0x3F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "adc", 0x14, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "adc", 0x15, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "adc", 0x15, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "adcb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "adcw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "adcd", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "adcw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "adcd", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "adc", 0x10, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "adc", 0x11, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "adc", 0x11, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "adc", 0x12, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "adc", 0x13, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "adc", 0x13, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "adcx", 0x0F, 0xF638, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "add", 0x04, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "add", 0x05, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "add", 0x05, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "addb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "addw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "addd", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "addw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "addd", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "add", 0x00, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "add", 0x01, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "add", 0x01, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "add", 0x02, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "add", 0x03, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "add", 0x03, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "addpd", 0x0F, 0x58, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "addps", 0x0F, 0x58, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "addsd", 0xF2, 0x580F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "addss", 0xF3, 0x580F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "addsubpd", 0x0F, 0xD0, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "addsubps", 0xF2, 0xD00F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "adox", 0xF3, 0xF6380F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "aesdec", 0x0F, 0xDE38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "aesdeclast", 0x0F, 0xDF38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "aesenc", 0x0F, 0xDC38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "aesenclast", 0x0F, 0xDD38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "aesimc", 0x0F, 0xDB38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "aeskeygenassist", 0x0F, 0xDF3A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "and", 0x24, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "and", 0x25, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "and", 0x25, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "andb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "andw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "andd", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "andw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "andd", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "and", 0x20, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "and", 0x21, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "and", 0x21, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "and", 0x22, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "and", 0x23, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "and", 0x23, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "andn", 0xE2C4, 0xF278, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD, 0, -1, 3, INSTR_ARG_GREGD, INSTR_ARG_GREGD, INSTR_ARG_MODRM },
	
	{ "andnpd", 0x0F, 0x55, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "andnps", 0x0F, 0x55, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "andpd", 0x0F, 0x54, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "andps", 0x0F, 0x54, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "arpl", 0x63, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	
	{ "bextr", 0xE2C4, 0xF778, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 3, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_GREGD },
	
	{ "blendpd", 0x0F, 0x0D3A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "blendps", 0x0F, 0x0C3A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "blendvpd", 0x0F, 0x1538, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "blendvps", 0x0F, 0x1438, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "blsi", 0xE2C4, 0xF378, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD, 0, 3, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "blmsk", 0xE2C4, 0xF378, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD, 0, 2, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "blsr", 0xE2C4, 0xF378, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD, 0, 1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "bound", 0x62, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "bound", 0x62, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "bsf", 0x0F, 0xBC, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "bsr", 0x0F, 0xBC, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "bsr", 0x0F, 0xBD, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "bsr", 0x0F, 0xBD, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "bswap", 0x0F, 0xC8, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "bt", 0x0F, 0xA3, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "bt", 0x0F, 0xA3, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "btw", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "btd", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "btc", 0x0F, 0xBB, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "btc", 0x0F, 0xBB, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "btcw", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "btcd", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "btr", 0x0F, 0xB3, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "btr", 0x0F, 0xB3, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "btrw", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "btrd", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "bts", 0x0F, 0xAB, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "bts", 0x0F, 0xAB, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "btsw", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "btsd", 0x0F, 0xBA, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "bzhi", 0xE2C4, 0xF578, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 3, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_GREGD },
	
	{ "call", 0xE8, -1, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "call", 0xFF, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "call", 0x9A, -1, INSTR_TYPE_POINTER, 0, -1, 1, INSTR_ARG_POINTER, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cbw", 0x98, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cwde", 0x98, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clac", 0x0F, 0xCA01, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clc", 0xF8, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cld", 0xFC, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clflush", 0x0F, 0xAE, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "clflushopt", 0x0F, 0xAE, INSTR_TYPE_MODRM, 1, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cli", 0xFA, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clts", 0x0F, 0x06, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "clwb", 0x0F, 0xAE, INSTR_TYPE_MODRM, 1, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cmc", 0xF5, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cmova", 0x0F, 0x47, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovae", 0x0F, 0x43, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovb", 0x0F, 0x42, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovbe", 0x0F, 0x46, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovc", 0x0F, 0x42, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmove", 0x0F, 0x44, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovz", 0x0F, 0x44, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovg", 0x0F, 0x4F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovge", 0x0F, 0x4D, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovl", 0x0F, 0x4C, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovle", 0x0F, 0x4E, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovna", 0x0F, 0x46, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnae", 0x0F, 0x42, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnb", 0x0F, 0x43, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnbe", 0x0F, 0x47, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnc", 0x0F, 0x43, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovne", 0x0F, 0x45, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovng", 0x0F, 0x4E, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnge", 0x0F, 0x4C, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnl", 0x0F, 0x4D, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnle", 0x0F, 0x4F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovno", 0x0F, 0x41, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnp", 0x0F, 0x4B, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovns", 0x0F, 0x49, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnz", 0x0F, 0x45, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovo", 0x0F, 0x40, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovp", 0x0F, 0x4A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovpe", 0x0F, 0x4A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovpo", 0x0F, 0x4B, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovs", 0x0F, 0x48, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovz", 0x0F, 0x44, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmova", 0x0F, 0x47, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovae", 0x0F, 0x43, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovb", 0x0F, 0x42, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovbe", 0x0F, 0x46, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovc", 0x0F, 0x42, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmove", 0x0F, 0x44, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovz", 0x0F, 0x44, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovg", 0x0F, 0x4F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovge", 0x0F, 0x4D, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovl", 0x0F, 0x4C, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovle", 0x0F, 0x4E, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovna", 0x0F, 0x46, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnae", 0x0F, 0x42, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnb", 0x0F, 0x43, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnbe", 0x0F, 0x47, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnc", 0x0F, 0x43, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovne", 0x0F, 0x45, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovng", 0x0F, 0x4E, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnge", 0x0F, 0x4C, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnl", 0x0F, 0x4D, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnle", 0x0F, 0x4F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovno", 0x0F, 0x41, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnp", 0x0F, 0x4B, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovns", 0x0F, 0x49, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovnz", 0x0F, 0x45, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovo", 0x0F, 0x40, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovp", 0x0F, 0x4A, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovpe", 0x0F, 0x4A, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovpo", 0x0F, 0x4B, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovs", 0x0F, 0x48, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmovz", 0x0F, 0x44, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "cmp", 0x3C, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "cmp", 0x3D, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "cmp", 0x3D, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "cmpb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "cmpw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "cmpd", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "cmpw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "cmpd", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "cmp", 0x38, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "cmp", 0x39, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "cmp", 0x39, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "cmp", 0x3A, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmp", 0x3B, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cmp", 0x3B, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "cmppd", 0x0F, 0xC2, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "cmpps", 0x0F, 0xC2, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "cmpsb", 0xA6, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cmpsw", 0xA7, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cmpsd", 0xA7, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "cmpsd", 0xF2, 0xC20F, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "cmpss", 0xF3, 0xC20F, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "cmpxchg", 0x0F, 0xB0, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "cmpxchg", 0x0F, 0xB1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "cmpxchg", 0x0F, 0xB1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "cmpxchg8b", 0x0F, 0xC7, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "comisd", 0x0F, 0x2F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "comiss", 0x0F, 0x2F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "cpuid", 0x0F, 0xA2, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "crc32b", 0x0FF2, 0xF038, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "crc32w", 0x0FF2, 0xF138, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "crc32d", 0x0FF2, 0xF138, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "cvtdq2pd", 0xF3, 0xE60F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtdq2ps", 0x0F, 0x5B, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtpd2dq", 0xF2, 0xE60F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtpd2ps", 0x0F, 0x5A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtps2dq", 0x0F, 0x5B, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtps2pd", 0x0F, 0x5A, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtsd2si", 0xF2, 0x2D0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtsd2ss", 0xF2, 0x5A0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtsi2sd", 0xF2, 0x2A0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cvtsi2ss", 0xF3, 0x2A0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "cvtss2sd", 0xF3, 0x5A0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvtss2si", 0xF3, 0x2D0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvttpd2dq", 0x0F, 0xE6, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvttps2dq", 0xF3, 0x5B0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvttsd2si", 0xF2, 0x2C0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "cvttss2si", 0xF3, 0x2C0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "cwd", 0x99, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "cdq", 0x99, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "daa", 0x27, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "das", 0x2F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "dec", 0xFF, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "dec", 0x48, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "dec", 0x48, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "div", 0xF6, -1, INSTR_TYPE_MODRM, 0, 6, 2, INSTR_ARG_ACCUMB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "div", 0xF7, -1, INSTR_TYPE_MODRM, 1, 6, 2, INSTR_ARG_ACCUMW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "div", 0xF7, -1, INSTR_TYPE_MODRM, 0, 6, 2, INSTR_ARG_ACCUMD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "divpd", 0x0F, 0x5E, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "divps", 0x0F, 0x5E, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "divsd", 0xF2, 0x5E0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "divss", 0xFE, 0x5E0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "dppd", 0x0F, 0x413A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "dpps", 0x0F, 0x403A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "enter", 0xC8, -1, INSTR_TYPE_WORD | INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_IMMW, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "extractps", 0x0F, 0x173A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_XREG, INSTR_ARG_IMMB },
	
	{ "f2xm1", 0xD9, 0xF0, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fabs", 0xD9, 0xE1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "faddd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "faddq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fadd", 0xD8, 0xC0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fadd", 0xDC, 0xC0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "faddp", 0xDE, 0xC0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "faddp", 0xDE, 0xC1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fiaddd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fiaddw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fbld", 0xDF, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fbstp", 0xDF, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fchs", 0xD9, 0xE0, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fclex", 0x9B, 0xDBE2, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fnclex", 0xDB, 0xE2, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fcmovb", 0xDA, 0xC0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmove", 0xDA, 0xC8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmovbe", 0xDA, 0xD0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmovu", 0xDA, 0xD8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmovnb", 0xDB, 0xC0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmovne", 0xDB, 0xC8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmovnbe", 0xDB, 0xD0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcmovnu", 0xDB, 0xD8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	
	{ "fcomd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcomq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcom", 0xD8, 0xD0, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcom", 0xD8, 0xD1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcompd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcompq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcomp", 0xD8, 0xD8, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcomp", 0xD8, 0xD9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fcompp", 0xDE, 0xD9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fcomi", 0xDB, 0xF0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fcomip", 0xDF, 0xF0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fucomi", 0xDB, 0xE8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fucomip", 0xDF, 0xE8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	
	{ "fcos", 0xD9, 0xFF, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fdecstp", 0xD9, 0xF6, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fdivd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fdivq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fdiv", 0xD8, 0xF0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fdiv", 0xDC, 0xF8, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fdivp", 0xDE, 0xF8, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fdivp", 0xDE, 0xF8, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fidivd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fidivw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fdivrd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fdivrq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fdivr", 0xD8, 0xF8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fdivr", 0xDC, 0xF0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fdivrp", 0xDE, 0xF0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fdivrp", 0xDE, 0xF1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fidivrd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fidivrw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ffree", 0xDD, 0xC0, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ficomw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "ficomd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "ficompw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "ficompd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fildw", 0xDF, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fildd", 0xDB, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fildq", 0xDF, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fincstp", 0xD9, 0xF7, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "finit", 0x9B, 0xE3DB, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fninit", 0xDB, 0xE3, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fistw", 0xDF, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fistd", 0xDB, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fistpw", 0xDF, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fistpd", 0xDB, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fistpq", 0xDF, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fisttpw", 0xDF, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fisttpd", 0xDB, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fisttpq", 0xDD, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fldd", 0xD9, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldq", 0xDD, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldt", 0xDB, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fld", 0xD9, 0xC0, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fld1", 0xD9, 0xE8, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldl2t", 0xD9, 0xE9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldl2e", 0xD9, 0xEA, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldpi", 0xD9, 0xEB, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldlg2", 0xD9, 0xEC, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldln2", 0xD9, 0xED, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fldz", 0xD9, 0xEE, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fldcw", 0xD9, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fldenv", 0xD9, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fmuld", 0xD8, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fmulq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fmul", 0xD8, 0xC8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fmul", 0xDC, 0xC8, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fmulp", 0xDE, 0xC8, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fmulp", 0xDE, 0xC9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fimuld", 0xDA, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fimulw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fnop", 0xD9, 0xD0, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fpatan", 0xD9, 0xF3, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fprem", 0xD9, 0xF8, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fprem1", 0xD9, 0xF5, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fptan", 0xD9, 0xF2, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "frndint", 0xD9, 0xFC, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "frstor", 0xDD, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fsave", 0x9B, 0xDD, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fnsave", 0xDD, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fscale", 0xD9, 0xFD, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fsin", 0xD9, 0xFE, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fsincos", 0xD9, 0xFB, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fsqrt", 0xD9, 0xFA, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fstd", 0xD9, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fstq", 0xDD, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fst", 0xDD, 0xD0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fstpd", 0xD9, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fstpq", 0xDD, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fstpt", 0xDB, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fstp", 0xDD, 0xD8, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fstcw", 0x9B, 0xD9, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fnstcw", 0xD9, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fstenv", 0x9B, 0xD9, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fnstenv", 0xD9, -1, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fstsw", 0x9B, 0xDD, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fstsw", 0X9B, 0xE0DF, INSTR_TYPE_NONE, 0, -1, 1, INSTR_ARG_ACCUMW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fnstsw", 0xDD, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fnstsw", 0xDF, 0xE0, INSTR_TYPE_NONE, 0, -1, 1, INSTR_ARG_ACCUMW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fsubd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fsubq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fsub", 0xD8, 0xE0, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fsub", 0xDC, 0xE8, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fsubp", 0xDE, 0xE8, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fsubp", 0xDE, 0xE9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fisubd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fisubw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fsubrd", 0xD8, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fsubrq", 0xDC, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fsubr", 0xD8, 0xE8, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_ST0, INSTR_ARG_STREG, INSTR_ARG_NONE },
	{ "fsubr", 0xDC, 0xE0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fsubrp", 0xDE, 0xE0, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_STREG, INSTR_ARG_ST0, INSTR_ARG_NONE },
	{ "fsubrp", 0xDE, 0xE1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fisubrd", 0xDA, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fisubrw", 0xDE, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ftst", 0xD9, 0xE4, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fucom", 0xDD, 0xE0, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fucom", 0xDD, 0xE1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fucomp", 0xDD, 0xE8, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fucomp", 0xDD, 0xE9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fucompp", 0xDA, 0xE9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fxam", 0xD9, 0xE5, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fxch", 0xD9, 0xC8, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_STREG, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "fxch", 0xD9, 0xC9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fxrstor", 0x0F, 0xAE, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fxsave", 0x0F, 0xAE, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fxtract", 0xD9, 0xF4, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fyl2x", 0xD9, 0xF1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "fyl2xp1", 0xD9, 0xF9, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "haddpd", 0x0F, 0x7C, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "haddps", 0xF2, 0x7C0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "hlt", 0xF4, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "hsubpd", 0x0F, 0x7D, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "hsubps", 0xF2, 0x7D0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "idivb", 0xF6, -1, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "idivw", 0xF7, -1, INSTR_TYPE_MODRM, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "idivd", 0xF7, -1, INSTR_TYPE_MODRM, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "imulb", 0xF6, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "imulw", 0xF7, -1, INSTR_TYPE_MODRM, 1, 5, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "imuld", 0xF7, -1, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "imul", 0x0F, 0xAF, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "imul", 0x0F, 0xAF, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "imul", 0x6B, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_IMMB },
	{ "imul", 0x6B, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_IMMB },
	{ "imul", 0x69, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, -1, 3, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_IMMW },
	{ "imul", 0x69, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, -1, 3, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_IMMD },
	
	{ "in", 0xE4, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "in", 0xE5, -1, INSTR_TYPE_BYTE, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "in", 0xE5, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "in", 0xEC, -1, INSTR_TYPE_NONE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_DESTW, INSTR_ARG_NONE },
	{ "in", 0xED, -1, INSTR_TYPE_NONE, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_DESTW, INSTR_ARG_NONE },
	{ "in", 0xED, -1, INSTR_TYPE_NONE, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_DESTW, INSTR_ARG_NONE },
	
	{ "incb", 0xFE, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "incw", 0xFF, -1, INSTR_TYPE_MODRM, 1, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "incd", 0xFF, -1, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "inc", 0x40, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "inc", 0x40, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "insb", 0x6C, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "insw", 0x6D, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "insd", 0x6D, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "insertps", 0x0F, 0x213A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "int3", 0xCC, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "int", 0xCD, -1, INSTR_TYPE_BYTE, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "into", 0xCE, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "invd", 0x0F, 0x08, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "invlpg", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "invpcid", 0x0F, 0x8238, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	
	{ "iret", 0xCF, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "iretw", 0xCF, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "iretd", 0xCF, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "nja", 0x77, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njae", 0x73, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njb", 0x72, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njbe", 0x76, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njc", 0x72, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njcxz", 0xE3, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njecxz", 0xE3, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "nje", 0x74, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njz", 0x74, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njg", 0x7F, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njge", 0x7D, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njl", 0x7C, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njle", 0x7E, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njna", 0x76, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnae", 0x72, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnb", 0x73, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnbe", 0x77, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnc", 0x73, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njne", 0x75, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njng", 0x7E, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnge", 0x7C, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnl", 0x7D, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnle", 0x7F, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njno", 0x71, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnp", 0x7B, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njns", 0x79, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njnz", 0x75, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njo", 0x70, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njp", 0x7A, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njpe", 0x7A, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njpo", 0x7B, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njs", 0x78, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njz", 0x74, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "ja", 0x87, -1, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jae", 0x0F, 0x83, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jb", 0x0F, 0x82, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jbe", 0x0F, 0x86, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jc", 0x0F, 0x82, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "je", 0x0F, 0x84, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jz", 0x0F, 0x84, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jg", 0x0F, 0x8F, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jge", 0x0F, 0x8D, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jl", 0x0F, 0x8C, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jle", 0x0F, 0x8E, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jna", 0x0F, 0x86, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnae", 0x0F, 0x82, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnb", 0x0F, 0x83, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnbe", 0x0F, 0x87, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnc", 0x0F, 0x83, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jne", 0x0F, 0x85, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jng", 0x0F, 0x8E, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnge", 0x0F, 0x8C, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnl", 0x0F, 0x8D, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnle", 0x0F, 0x8F, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jno", 0x0F, 0x81, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnp", 0x0F, 0x8B, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jns", 0x0F, 0x89, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jnz", 0x0F, 0x85, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jo", 0x0F, 0x80, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jp", 0x0F, 0x8A, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jpe", 0x0F, 0x8A, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jpo", 0x0F, 0x8B, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "js", 0x0F, 0x88, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jz", 0x0F, 0x84, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "njmp", 0xEB, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMB, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jmp", 0xE9, -1, INSTR_TYPE_RELD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jmp", 0xFF, -1, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "jmp", 0xEA, -1, INSTR_TYPE_POINTER, 0, -1, 1, INSTR_ARG_POINTER, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lahf", 0x9F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lar", 0x0F, 0x02, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "lar", 0x0F, 0x02, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "lddqu", 0xF2, 0xF00F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "ldmxcsr", 0x0F, 0xAE, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lea", 0x8D, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lea", 0x8D, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	
	{ "leave", 0xC9, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lfence", 0x0F, 0xE8AE, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lgdt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "lidt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lds", 0xC5, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lds", 0xC5, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lss", 0x0F, 0xB2, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lss", 0x0F, 0xB2, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lss", 0xC4, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lss", 0xC4, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lfs", 0x0F, 0xB4, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lfs", 0x0F, 0xB4, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lgs", 0x0F, 0xB5, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "lgs", 0x0F, 0xB5, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	
	{ "lldt", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lmsw", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lodsb", 0xAC, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "lodsw", 0xAD, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "lodsd", 0xAD, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "loop", 0xE2, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "loope", 0xE1, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "loopz", 0xE1, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "loopne", 0xE0, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "loopnz", 0xE0, -1, INSTR_TYPE_RELB, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lsl", 0x0F, 0x03, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "lsl", 0x0F, 0x03, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "ltr", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 6, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "lzcnt", 0xF3, 0xBD0F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "lzcnt", 0xF3, 0xBD0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "maskmovdqu", 0x0F, 0x07, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "maxpd", 0x0F, 0x5F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "maxps", 0x0F, 0x5F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "maxsd", 0xF2, 0x5F0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "maxss", 0xF3, 0x5F0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "mfence", 0x0F, 0xF0AE, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "minpd", 0x0F, 0x5D, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "minps", 0x0F, 0x5D, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "minsd", 0xF2, 0x5D0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "minss", 0xF3, 0x5D0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "monitor", 0x0F, 0xC801, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "mov", 0x88, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "mov", 0x89, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "mov", 0x89, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "mov", 0x8A, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "mov", 0x8B, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "mov", 0x8B, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "mov", 0x8C, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_SREG, INSTR_ARG_NONE },
	{ "mov", 0x8E, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_SREG, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "mov", 0xA0, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_MOFFS, INSTR_ARG_NONE },
	{ "mov", 0xA1, -1, INSTR_TYPE_DWORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_MOFFS, INSTR_ARG_NONE },
	{ "mov", 0xA1, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_MOFFS, INSTR_ARG_NONE },
	{ "mov", 0xA2, -1, INSTR_TYPE_DWORD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_MOFFS, INSTR_ARG_ACCUMB, INSTR_ARG_NONE },
	{ "mov", 0xA3, -1, INSTR_TYPE_DWORD | INSTR_TYPE_LEFT, 1, -1, 2, INSTR_ARG_MOFFS, INSTR_ARG_ACCUMW, INSTR_ARG_NONE },
	{ "mov", 0xA3, -1, INSTR_TYPE_DWORD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_MOFFS, INSTR_ARG_ACCUMD, INSTR_ARG_NONE },
	{ "mov", 0xB0, -1, INSTR_TYPE_OPREGB | INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "mov", 0xB8, -1, INSTR_TYPE_OPREGW | INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "mov", 0xB8, -1, INSTR_TYPE_OPREGD | INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "movb", 0xC6, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "movw", 0xC7, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "movd", 0xC7, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x20, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_CREG, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x22, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_DREG, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x20, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_CREG, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x22, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_DREG, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x21, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_DREG, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x23, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_DREG, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x24, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_TREG, INSTR_ARG_NONE },
	{ "mov", 0x0F, 0x26, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_TREG, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "movapd", 0x0F, 0x28, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movapd", 0x0F, 0x29, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movaps", 0x0F, 0x28, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movaps", 0x0F, 0x29, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "movbe", 0x0F, 0xF038, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movbe", 0x0F, 0xF038, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movbe", 0x0F, 0xF138, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "movbe", 0x0F, 0xF138, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	
	{ "movd", 0x0F, 0x6E, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movd", 0x0F, 0x7E, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movddup", 0xF2, 0x120F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "movdqa", 0x0F, 0x6F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movdqa", 0x0F, 0x7F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movdqu", 0xF3, 0x6F0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movdqu", 0xF3, 0x7F0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movhlps", 0x0F, 0x12, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movlhps", 0x0F, 0x16, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movhpd", 0x0F, 0x16, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movhpd", 0x0F, 0x17, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movhps", 0x0F, 0x16, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movhps", 0x0F, 0x17, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movlpd", 0x0F, 0x12, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movlpd", 0x0F, 0x13, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movlps", 0x0F, 0x12, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movlps", 0x0F, 0x13, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movmskpd", 0x0F, 0x50, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movmskps", 0x0F, 0x50, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movntdqa", 0x0F, 0x2A38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_MMODRM, INSTR_ARG_NONE },
	{ "movntdq", 0x0F, 0xE7, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movnti", 0x0F, 0xC3, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "movntpd", 0x0F, 0x2B, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movntps", 0x0F, 0x2B, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movq", 0xF3, 0x7E0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movq", 0x0F, 0xD6, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movsb", 0xA4, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "movsw", 0xA5, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "movsd", 0xA5, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "movsd", 0xF2, 0x100F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movsd", 0xF2, 0x110F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movss", 0xF3, 0x100F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movss", 0xF3, 0x110F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movshdup", 0xF3, 0x160F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movsldup", 0xF3, 0x120F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "movsxb", 0x0F, 0xBE, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "movsxb", 0x0F, 0xBE, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "movsxw", 0x0F, 0xBF, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "movupd", 0x0F, 0x10, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movupd", 0x0F, 0x11, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	{ "movups", 0x0F, 0x10, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "movups", 0x0F, 0x11, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XMODRM, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "movzxb", 0x0F, 0xB6, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "movzxb", 0x0F, 0xB6, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "movzxw", 0x0F, 0xB7, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "mpsadbw", 0x0F, 0x423A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "mul", 0xF6, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_ACCUMB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "mul", 0xF7, -1, INSTR_TYPE_MODRM, 1, 4, 2, INSTR_ARG_ACCUMW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "mul", 0xF7, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_ACCUMD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "mulpd", 0x0F, 0x59, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "mulps", 0x0F, 0x59, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "mulsd", 0xF2, 0x590F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "mulss", 0xF3, 0x590F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "mulx", 0xE2C4, 0xF678, INSTR_TYPE_MODRM | INSTR_TYPE_OPREGD, 0, -1, 3, INSTR_ARG_GREGD, INSTR_ARG_GREGD, INSTR_ARG_MODRM },
	
	{ "mwait", 0x0F, 0xC901, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "neg", 0xF7, -1, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "nop", 0x90, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "nop", 0x0F, 0x1F, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "not", 0xF6, -1, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "or", 0x0C, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "or", 0x0D, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "or", 0x0D, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "orb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "orw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "ord", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "orw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "ord", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "or", 0x08, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "or", 0x09, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "or", 0x09, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "or", 0x0A, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "or", 0x0B, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "or", 0x0B, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "orpd", 0x0F, 0x56, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "orps", 0x0F, 0x56, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "out", 0xE6, -1, INSTR_TYPE_BYTE | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_IMMB, INSTR_ARG_ACCUMB, INSTR_ARG_NONE },
	{ "out", 0xE7, -1, INSTR_TYPE_BYTE | INSTR_TYPE_LEFT, 1, -1, 2, INSTR_ARG_IMMB, INSTR_ARG_ACCUMW, INSTR_ARG_NONE },
	{ "out", 0xE7, -1, INSTR_TYPE_BYTE | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_IMMB, INSTR_ARG_ACCUMD, INSTR_ARG_NONE },
	{ "out", 0xEE, -1, INSTR_TYPE_NONE, 0, -1, 2, INSTR_ARG_DESTW, INSTR_ARG_ACCUMB, INSTR_ARG_NONE },
	{ "out", 0xEF, -1, INSTR_TYPE_NONE, 1, -1, 2, INSTR_ARG_DESTW, INSTR_ARG_ACCUMW, INSTR_ARG_NONE },
	{ "out", 0xEF, -1, INSTR_TYPE_NONE, 0, -1, 2, INSTR_ARG_DESTW, INSTR_ARG_ACCUMD, INSTR_ARG_NONE },
	
	{ "outsb", 0x6E, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "outsw", 0x6F, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "outsd", 0x6F, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pabsb", 0x0F, 0x1C38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pabsw", 0x0F, 0x1D38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pabsd", 0x0F, 0x1E38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "packsswb", 0x0F, 0x63, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "packssdw", 0x0F, 0x6B, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "packuswb", 0x0F, 0x67, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "packusdw", 0x0F, 0x2B38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "paddb", 0x0F, 0xFC, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddw", 0x0F, 0xFD, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddd", 0x0F, 0xFE, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddq", 0x0F, 0xD4, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddsb", 0x0F, 0xEC, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddsw", 0x0F, 0xED, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddusb", 0x0F, 0xDC, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "paddusw", 0x0F, 0xDD, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "palignr", 0x0F, 0x0F3A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "pand", 0x0F, 0xDB, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pandn", 0x0F, 0xDF, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pause", 0xF3, 0x90, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pavgb", 0x0F, 0xE0, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pavgw", 0x0F, 0xE3, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pblendvb", 0x0F, 0x1038, INSTR_TYPE_MODRM, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_XMM0 },
	{ "pblendw", 0x0F, 0x0E3A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "pclmulqdq", 0x0F, 0x443A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "pcmpeqb", 0x0F, 0x74, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpeqw", 0x0F, 0x75, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpeqd", 0x0F, 0x76, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpeqq", 0x0F, 0x2938, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpestri", 0x0F, 0x613A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pcmpestrm", 0x0F, 0x603A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pcmpgtb", 0x0F, 0x64, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpgtw", 0x0F, 0x65, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpgtd", 0x0F, 0x66, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpgtq", 0x0F, 0x3738, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pcmpistri", 0x0F, 0x633A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pcmpistrm", 0x0F, 0x623A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "pextrb", 0x0F, 0x143A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pextrw", 0x0F, 0xC5, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pextrd", 0x0F, 0x163A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "phaddw", 0x0F, 0x0138, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "phaddd", 0x0F, 0x0238, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "phaddsw", 0x0F, 0x0338, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "phminposuw", 0x0F, 0x4138, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "phsubw", 0x0F, 0x0538, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "phsubd", 0x0F, 0x0638, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "phsubsw", 0x0F, 0x0738, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pinsrb", 0x0F, 0x203A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pinsrw", 0x0F, 0xC4, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "pinsrd", 0x0F, 0x223A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "pmaddubsw", 0x0F, 0x0438, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmaddwd", 0x0F, 0xF5, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pmaxsb", 0x0F, 0x3C38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmaxsw", 0x0F, 0xEE, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmaxsd", 0x0F, 0x3D38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmaxub", 0x0F, 0xDE, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmaxuw", 0x0F, 0x3E38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmaxud", 0x0F, 0x3F38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pminsb", 0x0F, 0x3838, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pminsw", 0x0F, 0xEA, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pminsd", 0x0F, 0x3938, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pminub", 0x0F, 0xDA, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pminuw", 0x0F, 0x3A38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pminud", 0x0F, 0x3838, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pmovmskb", 0x0F, 0xD7, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_XREG, INSTR_ARG_NONE },
	
	{ "pmovsxbw", 0x0F, 0x2038, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovsxbd", 0x0F, 0x2138, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovsxbq", 0x0F, 0x2238, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovsxwd", 0x0F, 0x2338, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovsxwq", 0x0F, 0x2438, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovsxdq", 0x0F, 0x2538, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pmovzxbw", 0x0F, 0x3038, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovzxbd", 0x0F, 0x3138, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovzxbq", 0x0F, 0x3238, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovzxwd", 0x0F, 0x3338, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovzxwq", 0x0F, 0x3438, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmovzxdq", 0x0F, 0x3538, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pmuldq", 0x0F, 0x2838, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmulhrsw", 0x0F, 0x0B38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmulhuw", 0x0F, 0xE4, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmulhw", 0x0F, 0xE5, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmulld", 0x0F, 0x4038, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmullw", 0x0F, 0xD5, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pmuludq", 0x0F, 0xF4, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pop", 0x58, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pop", 0x58, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "popa", 0x61, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popaw", 0x61, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popad", 0x61, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "popcnt", 0xF3, 0xB80F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "popcnt", 0xF3, 0xB80F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "popf", 0x9D, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popfw", 0x9D, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "popfd", 0x9D, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "por", 0x0F, 0xEB, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "prefetcht0", 0x0F, 0x18, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "prefetcht1", 0x0F, 0x18, INSTR_TYPE_MODRM, 0, 2, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "prefetcht2", 0x0F, 0x18, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "prefetchnta", 0x0F, 0x18, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "prefetchtw", 0x0F, 0x0D, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "psadbw", 0x0F, 0xF6, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "psignb", 0x0F, 0x0838, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psignw", 0x0F, 0x0938, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psignd", 0x0F, 0x0A38, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "pslldq", 0x0F, 0x73, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "psllw", 0x0F, 0xF1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psllw", 0x0F, 0x71, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "pslld", 0x0F, 0xF2, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "pslld", 0x0F, 0x72, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "psllq", 0x0F, 0xF3, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psllq", 0x0F, 0x73, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "psraw", 0x0F, 0xE1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psraw", 0x0F, 0x71, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "psrad", 0x0F, 0xE2, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psrad", 0x0F, 0x72, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "psraq", 0x0F, 0xE3, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psraq", 0x0F, 0x73, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "psrldq", 0x0F, 0x73, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 3, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "psrlw", 0x0F, 0xD1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psrlw", 0x0F, 0x71, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 2, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "psrld", 0x0F, 0xD2, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psrld", 0x0F, 0x72, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 2, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "psrlq", 0x0F, 0xD3, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psrlq", 0x0F, 0x73, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 2, 2, INSTR_ARG_XREG, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "psubb", 0x0F, 0xF8, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubw", 0x0F, 0xF9, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubd", 0x0F, 0xFA, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubq", 0x0F, 0xFB, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubsb", 0x0F, 0xE8, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubsw", 0x0F, 0xE9, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubusb", 0x0F, 0xD8, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "psubusw", 0x0F, 0xD9, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "ptest", 0x0F, 0x1738, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "punpckhbw", 0x0F, 0x68, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "punpckhwd", 0x0F, 0x69, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "punpckhdq", 0x0F, 0x6A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "punpckhqdq", 0x0F, 0x6D, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "punpcklbw", 0x0F, 0x60, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "punpcklwd", 0x0F, 0x61, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "punpckldq", 0x0F, 0x62, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "punpcklqdq", 0x0F, 0x6C, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "push", 0x50, -1, INSTR_TYPE_OPREGW, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "push", 0x50, -1, INSTR_TYPE_OPREGD, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "push", 0x68, -1, INSTR_TYPE_DWORD, 0, -1, 1, INSTR_ARG_IMMD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pusha", 0x60, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushaw", 0x60, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushad", 0x60, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pushf", 0x9C, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushfw", 0x9C, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "pushfd", 0x9C, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "pxor", 0x0F, 0xEF, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "rclb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rclb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rclb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rclw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rclw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rclw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rcld", 0xD1, -1, INSTR_TYPE_MODRM, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rcld", 0xD3, -1, INSTR_TYPE_MODRM, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rcld", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 2, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rcrb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rcrb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rcrb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rcrw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rcrw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rcrw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rcrd", 0xD1, -1, INSTR_TYPE_MODRM, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rcrd", 0xD3, -1, INSTR_TYPE_MODRM, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rcrd", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rolb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rolb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rolb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rolw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rolw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rolw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rold", 0xD1, -1, INSTR_TYPE_MODRM, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rold", 0xD3, -1, INSTR_TYPE_MODRM, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rold", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rorb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rorb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rorb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rorw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rorw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rorw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "rord", 0xD1, -1, INSTR_TYPE_MODRM, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "rord", 0xD3, -1, INSTR_TYPE_MODRM, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "rord", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 1, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "rcpps", 0x0F, 0x53, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "rcpss", 0xF3, 0x530F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "rdmsr", 0x0F, 0x32, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdpid", 0xF3, 0xC70F, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdpkru", 0x0F, 0xEE01, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdpmc", 0x0F, 0x33, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdrand", 0x0F, 0xC7, INSTR_TYPE_MODRM, 1, -1, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdrand", 0x0F, 0xC7, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdseed", 0x0F, 0xC7, INSTR_TYPE_MODRM, 1, 7, 1, INSTR_ARG_GREGW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdseed", 0x0F, 0xC7, INSTR_TYPE_MODRM, 0, 7, 1, INSTR_ARG_GREGD, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdtsc", 0x0F, 0x31, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "rdtscp", 0x0F, 0xF901, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "ret", 0xC3, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "ret", 0xC2, -1, INSTR_TYPE_WORD, 0, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retf", 0xCB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retf", 0xCA, -1, INSTR_TYPE_WORD, 0, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfw", 0xCB, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfw", 0xCA, -1, INSTR_TYPE_WORD, 1, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfd", 0xCB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "retfd", 0xCA, -1, INSTR_TYPE_WORD, 0, -1, 1, INSTR_ARG_IMMW, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "roundpd", 0x0F, 0x093A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "roundps", 0x0F, 0x083A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "roundsd", 0x0F, 0x0B3A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "roundss", 0x0F, 0x0A3A, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "rsm", 0x0F, 0xAA, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "rsqrtps", 0x0F, 0x52, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "rsqrtss", 0xF3, 0x520F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "sahf", 0x9E, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "salb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "salb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "salb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "salw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "salw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "salw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sald", 0xD1, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "sald", 0xD3, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "sald", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sarb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "sarb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "sarb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sarw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "sarw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "sarw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sard", 0xD1, -1, INSTR_TYPE_MODRM, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "sard", 0xD3, -1, INSTR_TYPE_MODRM, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "sard", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 7, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "shlb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "shlb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "shlb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "shlw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "shlw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "shlw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "shld", 0xD1, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "shld", 0xD3, -1, INSTR_TYPE_MODRM, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "shld", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 4, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "shrb", 0xD0, -1, INSTR_TYPE_MODRM, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "shrb", 0xD2, -1, INSTR_TYPE_MODRM, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "shrb", 0xC0, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "shrw", 0xD1, -1, INSTR_TYPE_MODRM, 1, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "shrw", 0xD3, -1, INSTR_TYPE_MODRM, 1, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "shrw", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "shrd", 0xD1, -1, INSTR_TYPE_MODRM, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_VONE, INSTR_ARG_NONE },
	{ "shrd", 0xD3, -1, INSTR_TYPE_MODRM, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_COUNTB, INSTR_ARG_NONE },
	{ "shrd", 0xC1, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	
	{ "sbb", 0x1C, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sbb", 0x1D, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "sbb", 0x1D, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "sbbb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sbbw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sbbd", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sbbw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "sbbd", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 3, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "sbb", 0x18, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "sbb", 0x19, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "sbb", 0x19, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "sbb", 0x1A, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sbb", 0x1B, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sbb", 0x1B, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "scasb", 0xAE, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "scasw", 0xAF, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "scasd", 0xAF, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "seta", 0x0F, 0x97, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setae", 0x0F, 0x93, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setb", 0x0F, 0x92, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setbe", 0x0F, 0x96, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setc", 0x0F, 0x92, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "sete", 0x0F, 0x94, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setz", 0x0F, 0x94, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setg", 0x0F, 0x9F, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setge", 0x0F, 0x9D, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setl", 0x0F, 0x9C, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setle", 0x0F, 0x9E, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setna", 0x0F, 0x96, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnae", 0x0F, 0x92, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnb", 0x0F, 0x93, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnbe", 0x0F, 0x97, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnc", 0x0F, 0x93, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setne", 0x0F, 0x95, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setng", 0x0F, 0x9E, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnge", 0x0F, 0x9C, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnl", 0x0F, 0x9D, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnle", 0x0F, 0x9F, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setno", 0x0F, 0x91, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnp", 0x0F, 0x9B, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setns", 0x0F, 0x99, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setnz", 0x0F, 0x95, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "seto", 0x0F, 0x90, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setp", 0x0F, 0x9A, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setpe", 0x0F, 0x9A, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setpo", 0x0F, 0x9B, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "sets", 0x0F, 0x98, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "setz", 0x0F, 0x94, INSTR_TYPE_MODRM, 0, -1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sfence", 0x0F, 0xF8AE, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sgdt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "sidt", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "sldt", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 0, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "smsw", 0x0F, 0x01, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sha1rnds4", 0x0F, 0xCC3A, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "sha1nexte", 0x0F, 0xC838, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sha1msg1", 0x0F, 0xC938, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sha1msg1", 0x0F, 0xCA38, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sha256rnds2", 0x0F, 0xCB38, INSTR_TYPE_MODRM, 0, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_XMM0 },
	{ "sha256msg1", 0x0F, 0xCC38, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sha256msg1", 0x0F, 0xCD38, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "shld", 0x0F, 0xA4, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_IMMB },
	{ "shld", 0x0F, 0xA4, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_IMMB },
	{ "shld", 0x0F, 0xA5, INSTR_TYPE_MODRM, 1, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_ACCUMB },
	{ "shld", 0x0F, 0xA5, INSTR_TYPE_MODRM, 0, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_ACCUMB },
	
	{ "shrd", 0x0F, 0xAC, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_IMMB },
	{ "shrd", 0x0F, 0xAC, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_IMMB },
	{ "shrd", 0x0F, 0xAD, INSTR_TYPE_MODRM, 1, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_ACCUMB },
	{ "shrd", 0x0F, 0xAD, INSTR_TYPE_MODRM, 0, -1, 3, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_ACCUMB },
	
	{ "shufpd", 0x0F, 0xC6, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	{ "shufps", 0x0F, 0xC6, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, -1, 3, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_IMMB },
	
	{ "sqrtpd", 0x0F, 0x51, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sqrtps", 0x0F, 0x51, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sqrtsd", 0xF2, 0x510F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "sqrtss", 0xF3, 0x510F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "stac", 0x0F, 0xCB01, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "stc", 0xF9, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "std", 0xFD, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "sti", 0xFB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "stmxcsr", 0x0F, 0xAE, INSTR_TYPE_MODRM, 0, 3, 1, INSTR_ARG_MMODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "stosb", 0xAA, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "stosw", 0xAB, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "stosd", 0xAB, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "str", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 1, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sub", 0x2C, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "sub", 0x2D, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "sub", 0x2D, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "subb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "subw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "subd", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "subw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "subd", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 5, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "sub", 0x28, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "sub", 0x29, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "sub", 0x29, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "sub", 0x2A, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sub", 0x2B, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "sub", 0x2B, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "subpd", 0x0F, 0x5C, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "subps", 0x0F, 0x5C, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "subsd", 0xF2, 0x5C0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "subss", 0xF3, 0x5C0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "sysenter", 0x0F, 0x34, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "sysexit", 0x0F, 0x35, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "test", 0xA8, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "test", 0xA9, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "test", 0xA9, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "testb", 0xF6, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "testw", 0xF7, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "testd", 0xF7, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 0, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "testb", 0x84, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "testw", 0x85, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "testd", 0x85, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "tzcnt", 0xF3, 0xBC0F, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "tzcnt", 0xF3, 0xBC0F, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "ucomisd", 0x0F, 0x2E, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "ucomiss", 0x0F, 0x2E, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "ud0", 0x0F, 0xFF, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "ud1", 0x0F, 0xB9, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "ud2", 0x0F, 0x0B, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "unpckhpd", 0x0F, 0x15, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "unpckhps", 0x0F, 0x15, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "unpcklpd", 0x0F, 0x14, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "unpcklps", 0x0F, 0x14, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	
	{ "verr", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 4, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "verw", 0x0F, 0x00, INSTR_TYPE_MODRM, 0, 5, 1, INSTR_ARG_MODRM, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "wait", 0x9B, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "wbinvd", 0x0F, 0x09, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "wrmsr", 0x0F, 0x30, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "wrpkru", 0x0F, 0xEF01, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "xadd", 0x0F, 0xC0, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "xadd", 0x0F, 0xC1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "xadd", 0x0F, 0xC1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	
	{ "xchg", 0x90, -1, INSTR_TYPE_OPREGW, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "xchg", 0x90, -1, INSTR_TYPE_OPREGW | INSTR_TYPE_LEFT, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_ACCUMW, INSTR_ARG_NONE },
	{ "xchg", 0x90, -1, INSTR_TYPE_OPREGD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "xchg", 0x90, -1, INSTR_TYPE_OPREGD | INSTR_TYPE_LEFT, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_ACCUMD, INSTR_ARG_NONE },
	{ "xchg", 0x86, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "xchg", 0x86, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "xchg", 0x87, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "xchg", 0x87, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "xchg", 0x87, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "xchg", 0x87, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "xlatb", 0xD7, -1, INSTR_TYPE_NONE, 1, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	{ "xlat", 0xD7, -1, INSTR_TYPE_NONE, 0, -1, 0, INSTR_ARG_NONE, INSTR_ARG_NONE, INSTR_ARG_NONE },
	
	{ "xor", 0x34, -1, INSTR_TYPE_BYTE, 0, -1, 2, INSTR_ARG_ACCUMB, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "xor", 0x35, -1, INSTR_TYPE_WORD, 1, -1, 2, INSTR_ARG_ACCUMW, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "xor", 0x35, -1, INSTR_TYPE_DWORD, 0, -1, 2, INSTR_ARG_ACCUMD, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "xorb", 0x80, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "xorw", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 1, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "xord", 0x83, -1, INSTR_TYPE_MODRM | INSTR_TYPE_BYTE, 0, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMB, INSTR_ARG_NONE },
	{ "xorw", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_WORD, 1, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMW, INSTR_ARG_NONE },
	{ "xord", 0x81, -1, INSTR_TYPE_MODRM | INSTR_TYPE_DWORD, 0, 6, 2, INSTR_ARG_MODRM, INSTR_ARG_IMMD, INSTR_ARG_NONE },
	{ "xor", 0x30, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGB, INSTR_ARG_NONE },
	{ "xor", 0x31, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGW, INSTR_ARG_NONE },
	{ "xor", 0x31, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_MODRM, INSTR_ARG_GREGD, INSTR_ARG_NONE },
	{ "xor", 0x32, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGB, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "xor", 0x33, -1, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_GREGW, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	{ "xor", 0x33, -1, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_GREGD, INSTR_ARG_MODRM, INSTR_ARG_NONE },
	
	{ "xorpd", 0x0F, 0x57, INSTR_TYPE_MODRM, 1, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE },
	{ "xorps", 0x0F, 0x57, INSTR_TYPE_MODRM, 0, -1, 2, INSTR_ARG_XREG, INSTR_ARG_XMODRM, INSTR_ARG_NONE }
};

static void x86_help(void) { }
static int x86_option(int argc, char **argv, int i) { (void)argc; (void)argv; (void)i; return 0; }

static void lexer_consume(lexer_t *lexer) {
	if (lexer->pos < lexer->length) {																							// We can increase the position?
		if (lexer->text[lexer->pos] == '\n') {																					// Yes, new line?
			lexer->line++;																										// Yes! Increase the line count
			lexer->col = 0;																										// And set the current column to zero
		} else {
			lexer->col++;																										// No, just increase the current column
		}

		lexer->pos++;																											// Increase the position
	}
}

static token_t *lexer_new_token(token_t *list, token_t *cur) {
	if (cur == NULL) {																											// We're at the start of the list?
		return list;																											// Yes :)
	}
	
	cur->next = calloc(1, sizeof(token_t));																						// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																											// Failed...
	}
	
	cur->next->prev = cur;																										// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static int x86_find_register(char *name) {
	for (int i = 0; i < 70; i++) {
		if ((strlen(registers[i]) == strlen(name)) && !strcasecmp(registers[i], name)) {										// Found?
			return 1;																											// Yes :)
		}
	}
	
	return 0;
}

static token_t *x86_lex(lexer_t *lexer, token_t *list, token_t *cur) {
	if (lexer == NULL || list == NULL) {																						// Null pointer checks
		return NULL;
	} else if (lexer->text[lexer->pos] == '*' || lexer->text[lexer->pos] == '[' || lexer->text[lexer->pos] == ']') {			// Single character token?
		cur = lexer_new_token(list, cur);																						// Yes, create a new token at the end of the list
		
		if (cur == NULL) {
			return NULL;																										// Failed...
		}
		
		cur->type = lexer->text[lexer->pos] == '*' ? TOK_TYPE_MUL :
					(lexer->text[lexer->pos] == '[' ? TOK_TYPE_OBRAC : TOK_TYPE_CBRAC);											// Set the type
		cur->filename = lexer->filename;																						// Set the filename
		cur->line = lexer->line;																								// Set the line
		cur->col = lexer->col;																									// And the column
		
		lexer_consume(lexer);																									// Consume the character
		
		return cur;
	} else {
		return NULL;																											// ...
	}
}

static node_t *parser_new_node(node_t *cur, size_t size) {
	if (cur == NULL) {																											// We're at the start of the list?
		return (node_t*)calloc(1, size);																						// Yes :)
	}
	
	cur->next = calloc(1, size);																								// Alloc the new token
	
	if (cur->next == NULL) {
		return NULL;																											// Failed...
	}
	
	cur->next->prev = cur;																										// Set the previous entry of it to the last entry (the one before the new one)
	
	return cur->next;
}

static node_t *parser_parse_register(parser_t *parser, node_t *cur) {
	token_t *tok = parser_expect_noval(parser, TOK_TYPE_REGISTER);																// Get our token
	
	if (tok == NULL) {
		return NULL;																											// Failed...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(register_node_t));																// Create the node
	
	if (ret != NULL) {																											// Failed?
		ret->type = NODE_TYPE_REGISTER;																							// No, so let's set the type
		((register_node_t*)ret)->name = tok->value;																				// And the value!
	}
	
	return ret;
}

static node_t *parser_parse_pointer(parser_t *parser, node_t *cur, node_t *left) {
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																		// Identifier?
		left->next = parser_parse_identifier(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																	// Number?
		left->next = parser_parse_number(parser, NULL);
	} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {																	// Register?
		left->next = parser_parse_register(parser, NULL);
	}
	
	if (left->next == NULL) {
		return NULL;																											// ...
	}
	
	node_t *ret = parser_new_node(cur, sizeof(node_t));																			// Create the node
	
	if (ret != NULL) {																											// Failed?
		ret->type = NODE_TYPE_POINTER;																							// No, so let's set the type
		ret->childs = left;																										// The val
	}
	
	return ret;
}

static node_t *parser_parse_address(parser_t *parser, node_t *cur) {
	node_t *val = NULL;																											// Let's get the val
	int have_disp = 0;
	int have_mul = 0;
	int32_t disp = 0;
	uint32_t mul = 0;
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																		// Identifier?
		val = parser_parse_identifier(parser, NULL);
		
		if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																		// Pointer?
			val = parser_parse_pointer(parser, NULL, val);																		// Yes
		}
	} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																	// Number?
		val = parser_parse_number(parser, NULL);
		
		if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																		// Pointer?
			val = parser_parse_pointer(parser, NULL, val);																		// Yes
		}
	} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {																	// Register?
		val = parser_parse_register(parser, NULL);
		
		if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																		// Pointer?
			if (val != NULL) {																									// Failed?
				int found = 0;																									// Nope, let's see if this is a valid segment register!

				for (int i = 0; !found && i < 6; i++) {
					if (!strcasecmp(((register_node_t*)val)->name, sregs[i])) {													// Found?
						found = 1;																								// Yes!
					}
				}

				if (!found) {
					free(val);																									// Not found...
					return NULL;
				}
			}
			
			val = parser_parse_pointer(parser, NULL, val);																		// Parse the pointer
		}
	}
	
	if (val == NULL) {
		return NULL;																											// ...
	}
	
	if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {																			// Displacement?
		token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);															// Yes, now we should have a number
		char *endptr = NULL;
		
		if (tok == NULL) {
			return NULL;																										// ...
		} else if (tok->value[0] != '+' && tok->value[0] != '-') {																// It's really the displacement?
			return NULL;																										// ...
		} else if (tok->value[0] == '+') {																						// Displacement = Add?
			disp = strtoimax(tok->value + 1, &endptr, 0);																		// Yes
		} else {
			disp = strtoimax(tok->value, &endptr, 0);																			// Nope, we need to parse the '-'
		}
		
		have_disp = 1;
	}
	
	if (parser_accept_noval(parser, TOK_TYPE_MUL)) {																			// Multiply?
		token_t *tok = parser_expect_noval(parser, TOK_TYPE_NUMBER);															// Yes, now we should have a number
		char *endptr = NULL;
		
		if (tok == NULL) {
			return NULL;																										// ...
		} else if (tok->value[0] == '+' || tok->value[0] == '-') {																// It's really what we want?
			return NULL;
		} else if (tok->value[0] == '0' && tok->value[1] == 'b') {																// Binary?
			mul = strtoumax(tok->value + 2, &endptr, 2);																		// Yes, convert using base = 2
		} else {
			mul = strtoumax(tok->value, &endptr, 0);																			// Use auto-detection (base = 0)
		}
		
		have_mul = 1;
	}
	
	parser_expect_noval(parser, TOK_TYPE_CBRAC);																				// Expect the closing bracket
	
	node_t *ret = parser_new_node(cur, sizeof(address_node_t));																	// Create the node
	
	if (ret != NULL) {																											// Failed?
		ret->type = NODE_TYPE_ADDRESS;																							// No, so let's set the type
		ret->childs = val;																										// The val
		((address_node_t*)ret)->have_disp = have_disp;																			// And some informations about the displacement etc
		((address_node_t*)ret)->have_mul = have_mul;
		((address_node_t*)ret)->disp = disp;
		((address_node_t*)ret)->mul = mul;
	}
	
	return ret;
}

static int x86_find_mnemonic(char *name) {
	for (int i = 0; i < 765; i++) {
		if ((strlen(mnemonics[i]) == strlen(name)) && !strcasecmp(mnemonics[i], name)) {										// Found?
			return 1;																											// Yes :)
		}
	}
	
	return 0;
}

static node_t *x86_parse(parser_t *parser, node_t *cur) {
	if (parser == NULL) {																										// Null pointer check
		return NULL;	
	}
	
	token_t *tok = parser->position;																							// Save the initial token (for the errors/warnings)
	
	if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && !strcasecmp(parser->position->value, "rep")) {						// REP prefix?
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);
		cur = parser_new_node(cur, sizeof(node_t));																				// Create the node
		
		if (cur == NULL) {
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_REP;																								// Set the type
		
		return cur;
	} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && !strcasecmp(parser->position->value, "repe")) {				// REPE prefix?
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);
		cur = parser_new_node(cur, sizeof(node_t));																				// Create the node
		
		if (cur == NULL) {
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_REPE;																								// Set the type
		
		return cur;
	} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && !strcasecmp(parser->position->value, "repz")) {				// REPZ prefix?
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);
		cur = parser_new_node(cur, sizeof(node_t));																				// Create the node
		
		if (cur == NULL) {
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_REPE;																								// Set the type
		
		return cur;
	} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && !strcasecmp(parser->position->value, "lock")) {				// LOCK prefix?
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);
		cur = parser_new_node(cur, sizeof(node_t));																				// Create the node
		
		if (cur == NULL) {
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_LOCK;																								// Set the type
		
		return cur;
	} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && !strcasecmp(parser->position->value, "repne")) {				// REPNE prefix?
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);
		cur = parser_new_node(cur, sizeof(node_t));																				// Create the node
		
		if (cur == NULL) {
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_REPNE;																							// Set the type
		
		return cur;
	} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && !strcasecmp(parser->position->value, "repnz")) {				// REPNZ prefix?
		parser_expect_noval(parser, TOK_TYPE_IDENTIFIER);
		cur = parser_new_node(cur, sizeof(node_t));																				// Create the node
		
		if (cur == NULL) {
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_REPNE;																							// Set the type
		
		return cur;
	} else if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER) && x86_find_mnemonic(parser->position->value)) {					// Instruction?
		char *name = parser_expect_noval(parser, TOK_TYPE_IDENTIFIER)->value;													// Yes, save the name
		node_t *args = NULL;
		
		if (!parser_accept_noval(parser, TOK_TYPE_EOS)) {																		// We have any argument?
start:		if (parser_check_noval(parser, TOK_TYPE_IDENTIFIER)) {																// Yes, identifier?
				node_t *arg = parser_parse_identifier(parser, NULL);
				
				if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																// Pointer?
					args = parser_parse_pointer(parser, args, arg);																// Yes
				} else if (args != NULL) {																						// Nope, first op?
					args->next = arg;																							// Nope
				} else {
					args = arg;																									// Yes
				}
			} else if (parser_check_noval(parser, TOK_TYPE_NUMBER)) {															// Number?
				node_t *arg = parser_parse_number(parser, NULL);
				
				if (parser_accept_noval(parser, TOK_TYPE_COLON)) {																// Pointer?
					args = parser_parse_pointer(parser, args, arg);																// Yes
				} else if (args != NULL) {																						// Nope, first op?
					args->next = arg;																							// Nope
				} else {
					args = arg;																									// Yes
				}
			} else if (parser_check_noval(parser, TOK_TYPE_REGISTER)) {															// Registers?
				args = parser_parse_register(parser, args);
			} else if (parser_accept_noval(parser, TOK_TYPE_OBRAC)) {															// Address
				args = parser_parse_address(parser, args);
			}
			
			if (args == NULL) {
				printf("%s: %d: %d: invalid argument\n", tok->filename, tok->line, tok->col);									// Invalid, return -1 (error)
				
				if (args != NULL) {
					args = node_rewind_list(args);
					node_free_list(args);
				}
				
				return (node_t*)-1;
			}
			
			if (parser_accept_noval(parser, TOK_TYPE_COMMA)) {																	// More arguments?
				goto start;																										// Yes, go back to the start!
			}
			
			args = node_rewind_list(args);																						// Rewind the arg list
			parser_expect_noval(parser, TOK_TYPE_EOS);																			// Now we MUST have a EOS
		}
		
		cur = parser_new_node(cur, sizeof(instruction_node_t));																	// Create the node
		
		if (cur == NULL) {
			if (args != NULL) {																									// Failed, return -1 (error)
				args = node_rewind_list(args);
				node_free_list(args);
			}
			
			return (node_t*)-1;
		}
		
		cur->type = NODE_TYPE_INSTRUCTION;																						// Set the type
		cur->childs = args;																										// Set the arguments
		((instruction_node_t*)cur)->name = name;																				// And the instruction name
		
		return cur;
	} else {
		return NULL;																											// Nope, this is a label (probably)
	}
}

static uint32_t count_childs(node_t *node) {
	uint32_t ret = 0;
	
	for (; node != NULL; ret++, node = node->next) ;																			// Just keep on going forward and counting!
	
	return ret;
}

static int get_optype(node_t *node) {
	int ret = 0;
	
	if (node->type == NODE_TYPE_IDENTIFIER) {																					// Identifier (symbol name without the brackets)
		ret |= INSTR_ARG_IMMB | INSTR_ARG_IMMW | INSTR_ARG_IMMD;
	} else if (node->type == NODE_TYPE_NUMBER) {																				// Number
		uintmax_t val = ((number_node_t*)node)->value;																			// Get the value
		
		if (val == 1) {																											// Vone
			ret |= INSTR_ARG_VONE | INSTR_ARG_IMMB | INSTR_ARG_IMMW | INSTR_ARG_IMMD;
		} else if (val <= UINT8_MAX) {																							// Byte
			ret |= INSTR_ARG_IMMB | INSTR_ARG_IMMW | INSTR_ARG_IMMD;
		} else if (val <= UINT16_MAX) {																							// Word
			ret |= INSTR_ARG_IMMW | INSTR_ARG_IMMD;
		} else {																												// DWord
			ret |= INSTR_ARG_IMMD;
		}
	} else if (node->type == NODE_TYPE_REGISTER) {																				// Register
		char *name = ((register_node_t*)node)->name;
		int found = 0;
		
		if (!strcasecmp(name, "xmm0")) {																						// XMM0?
			ret |= (INSTR_ARG_XREG | INSTR_ARG_XMM0);																			// Yes!
			found = 1;
		} else if (!strcasecmp(name, "st0")) {																					// ST0?
			ret |= (INSTR_ARG_STREG | INSTR_ARG_ST0);																			// Yes!
			printf("0x%x\n", ret);
			found = 1;
		} else if (!strcasecmp(name, "eax")) {																					// Accum for dwords?
			ret |= (INSTR_ARG_ACCUMD | INSTR_ARG_GREGD);																		// Yes!
			found = 1;
		} else if (!strcasecmp(name, "ax")) {																					// Accum for words?
			ret |= (INSTR_ARG_ACCUMW | INSTR_ARG_GREGW);																		// Yes!
			found = 1;
		} else if (!strcasecmp(name, "al")) {																					// Accum for bytes?
			ret |= (INSTR_ARG_ACCUMB | INSTR_ARG_GREGB);																		// Yes!
			found = 1;
		} else if (!strcasecmp(name, "ecx")) {																					// Counter for dwords?
			ret |= (INSTR_ARG_COUNTD | INSTR_ARG_GREGD);																		// Yes!
			found = 1;
		} else if (!strcasecmp(name, "cx")) {																					// Counter for words?
			ret |= (INSTR_ARG_COUNTW | INSTR_ARG_GREGW);																		// Yes!
			found = 1;
		} else if (!strcasecmp(name, "cl")) {																					// Counter for bytes?
			ret |= (INSTR_ARG_COUNTB | INSTR_ARG_GREGB);																		// Yes!
			found = 1;
		} else if (!strcasecmp(name, "edx")) {																					// Dest for dwords?
			ret |= (INSTR_ARG_DESTD | INSTR_ARG_GREGD);																			// Yes!
			found = 1;
		} else if (!strcasecmp(name, "dx")) {																					// Dest for words?
			ret |= (INSTR_ARG_DESTW | INSTR_ARG_GREGW);																			// Yes!
			found = 1;
		} else if (!strcasecmp(name, "dl")) {																					// Dest for bytes?
			ret |= (INSTR_ARG_COUNTB | INSTR_ARG_GREGB);																		// Yes!
			found = 1;
		}
		
		for (int i = 0; !found && i < 8; i++) {																					// First, check if it is a 32-bits register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_GREGD;
				found = 1;
			}
		}
		
		for (int i = 8; !found && i < 16; i++) {																				// Or a 16-bits register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_GREGW;
				found = 1;
			}
		}
		
		for (int i = 16; !found && i < 24; i++) {																				// Or a 8-bits register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_GREGB;
				found = 1;
			}
		}
		
		for (int i = 24; !found && i < 30; i++) {																				// Or a segment register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_SREG;
				found = 1;
			}
		}
		
		for (int i = 30; !found && i < 38; i++) {																				// Or a control register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_CREG;
				found = 1;
			}
		}
		
		for (int i = 38; !found && i < 46; i++) {																				// Or a debug register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_DREG;
				found = 1;
			}
		}
		
		for (int i = 46; !found && i < 54; i++) {																				// Or a test register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_TREG;
				found = 1;
			}
		}
		
		for (int i = 54; !found && i < 62; i++) {																				// Or a FPU (ST) register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_STREG;
				found = 1;
			}
		}
		
		for (int i = 62; !found && i < 70; i++) {																				// Or a SSE (XMM) register
			if (!strcasecmp(name, registers[i])) {
				ret |= INSTR_ARG_XREG;
				found = 1;
			}
		}
	} else if (node->type == NODE_TYPE_ADDRESS) {																				// Identifier, number or register with the brackets
		if (node->childs->type == NODE_TYPE_REGISTER || (node->childs->type == NODE_TYPE_POINTER &&
			node->childs->childs->next->type == NODE_TYPE_REGISTER)) {															// Register?
			ret |= INSTR_ARG_MODRM_REG;
		} else {
			ret |= INSTR_ARG_MODRM_ADDR | INSTR_ARG_MOFFS;																		// Identifier/number?
		}
		
		if (((address_node_t*)node)->have_disp) {																				// We have the displacement?
			ret |= INSTR_ARG_MODRM_DISP;
		}
		
		if (((address_node_t*)node)->have_mul) {																				// We have the scale?
			ret |= INSTR_ARG_MODRM_MULT;
		}
	} else if (node->type == NODE_TYPE_POINTER) {																				// Pointer
		ret |= INSTR_ARG_POINTER;
	}
	
	return ret;
}

static uint32_t get_opval(codegen_t *codegen, node_t *node, int size, int rel) {
	codegen_section_t *sect = codegen->current_section;
	uint32_t ret = rel ? -size : 0;
	
	if (node->type == NODE_TYPE_IDENTIFIER) {																					// Identifier (symbol)?
		codegen_add_relocation(codegen, ((identifier_node_t*)node)->value, sect->name, size, sect->size, 0, rel);				// Yes, add relocation
	} else if (node->type == NODE_TYPE_NUMBER) {																				// Number?
		if (rel) {																												// Relative?
			codegen_add_relocation(codegen, NULL, sect->name, size, sect->size, (uint32_t)(((number_node_t*)node)->value), 1);	// Yes
		} else {
			ret = (uint32_t)(((number_node_t*)node)->value);																	// Nope
		}
	} else if (node->type == NODE_TYPE_POINTER) {																				// Pointer?
		return get_opval(codegen, node->childs->next, size, rel);																// Yes
	}
	
	return ret;
}

static int get_sreg(char *name) {
	for (int i = 0; i < 6; i++) {																								// Search!
		if (!strcasecmp(name, sregs[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_creg(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, cregs[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_dreg(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, dregs[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_treg(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, tregs[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_gregb(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, gregsb[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_gregw(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, gregsw[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return get_gregb(name);																										// Maybe it's a 8-bits one
}

static int get_gregd(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, gregsd[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return get_gregw(name);																										// Maybe it's a 16-bits one
}

static int get_streg(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, stregs[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static int get_xreg(char *name) {
	for (int i = 0; i < 8; i++) {																								// Search!
		if (!strcasecmp(name, xregs[i])) {																						// Found?
			return i;																											// Yes!
		}
	}
	
	return 0;
}

static void x86_write_segover(codegen_t *codegen, node_t *n) {
	if (n->type != NODE_TYPE_POINTER) {																							// Segment override?
		return;																													// Nope
	} else if (!strcasecmp(((register_node_t*)n->childs)->name, "cs")) {														// CS override
		codegen_write_byte(codegen, 0x2E);
	} else if (!strcasecmp(((register_node_t*)n->childs)->name, "ds")) {														// DS override
		codegen_write_byte(codegen, 0x3E);
	} else if (!strcasecmp(((register_node_t*)n->childs)->name, "es")) {														// ES override
		codegen_write_byte(codegen, 0x26);
	} else if (!strcasecmp(((register_node_t*)n->childs)->name, "fs")) {														// FS override
		codegen_write_byte(codegen, 0x64);
	} else if (!strcasecmp(((register_node_t*)n->childs)->name, "gs")) {														// GS override
		codegen_write_byte(codegen, 0x65);
	} else if (!strcasecmp(((register_node_t*)n->childs)->name, "ss")) {														// SS override
		codegen_write_byte(codegen, 0x36);
	}
}

static int x86_write_modrm(codegen_t *codegen, int ex, int reg, int op, node_t *n) {
	int ext = (ex != -1 ? ex << 3 : 0) | (reg != -1 ? ((op & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG))
													? reg : (reg << 3)) : 0);													// First, get the "extension" that some instructions use
	int ptr = 0;
	
	if (n->childs != NULL && n->childs->type == NODE_TYPE_POINTER) {															// Pointer?
		ptr = 1;																												// Yes, get the offset!
		if (op == (INSTR_ARG_MODRM_REG | INSTR_ARG_MODRM_DISP)) {
			n->childs = n->childs->childs->next;
		} else {
			n = n->childs->childs->next;
		}
	}
	
	if (op != (INSTR_ARG_MODRM_REG | INSTR_ARG_MODRM_DISP)) {
		n = !ptr && !(op & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG |
							INSTR_ARG_XREG | INSTR_ARG_GREGB | INSTR_ARG_GREGW | INSTR_ARG_GREGD)) ? n->childs : n;
	}
	
	if (op == (INSTR_ARG_MODRM_ADDR | INSTR_ARG_MOFFS)) {																		// Just a number/identifier in the brackets?
		codegen_write_byte(codegen, ext | 0x05);																				// Yes, write the modr/m byte
		codegen_write_dword(codegen, get_opval(codegen, n, 4, 0));																// And the value
	} else if (op & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG | INSTR_ARG_XREG | INSTR_ARG_GREGB |
					 INSTR_ARG_GREGW | INSTR_ARG_GREGD)) {																		// Just a register (without the brackets)?
		int rreg = (op & INSTR_ARG_SREG) ? get_sreg(((register_node_t*)n)->name) :
				   ((op & INSTR_ARG_CREG) ? get_creg(((register_node_t*)n)->name) :
				   ((op & INSTR_ARG_DREG) ? get_dreg(((register_node_t*)n)->name) :
				   ((op & INSTR_ARG_TREG) ? get_treg(((register_node_t*)n)->name) :
				   ((op & INSTR_ARG_XREG) ? get_xreg(((register_node_t*)n)->name) :
				   get_gregd(((register_node_t*)n)->name)))));																	// Yes
		
		if (op & (INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG)) {															// Control register as second op?
			rreg <<= 3;																											// Yes
		}
		
		codegen_write_byte(codegen, ext | rreg | 0xC0);																			// Yes, write the modr/m byte
	} else if (op & INSTR_ARG_SREG) {																							// Segment register?
		codegen_write_byte(codegen, ext | (get_sreg(((register_node_t*)n)->name) << 3) | 0xC0);									// Yes, write the modr/m byte
	} else if (op == INSTR_ARG_MODRM_REG) {																						// 32-bits register?
		if (!strcasecmp(((register_node_t*)n)->name, "ebp")) {																	// Yes, write the modr/m byte, it would be ext | 0x05
			codegen_write_word(codegen, 0x4500);																				// Yes, fix that
		} else {
			codegen_write_byte(codegen, ext | get_gregd(((register_node_t*)n)->name));											// Nope!
		}
		
		if (!strcasecmp(((register_node_t*)n)->name, "esp")) {																	// modr/m = ext | 0x04?
			codegen_write_byte(codegen, 0x24);																					// Yes, fix!
		}
	} else if (op == (INSTR_ARG_MODRM_REG | INSTR_ARG_MODRM_DISP)) {															// 32-bits register + displacement?
		uint32_t disp = ((address_node_t*)n)->disp;																				// Get the displacement
		uint8_t dshft = 0x80;
		
		if (((int32_t)disp <= INT8_MAX) || ((int32_t)disp >= INT8_MIN)) {														// 8-bits disp?
			dshft = 0x40;																										// Yeah
		}
		
		if (!strcasecmp(((register_node_t*)n->childs)->name, "ebp")) {															// Yes, write the modr/m byte, it would be ext | 0x05
			codegen_write_word(codegen, 0x4500 | dshft);																		// Yes, fix that
		} else {
			codegen_write_byte(codegen, ext | get_gregd(((register_node_t*)n->childs)->name) | dshft);							// Nope!
		}
		
		if (!strcasecmp(((register_node_t*)n->childs)->name, "esp")) {															// modr/m = ext | 0x04?
			codegen_write_byte(codegen, 0x24);																					// Yes, fix!
		}
		
		if (dshft == 0x40) {																									// 8-bits disp?
			codegen_write_byte(codegen, (uint8_t)disp);																			// Yes
		} else {
			codegen_write_dword(codegen, disp);																					// 32-bits disp
		}
	} else {
		return 0;																												// Unsupported
	}
	
	return 1;
}

static node_t *get_last(node_t* node, int ops) {
	for (int i = 1; i < ops; i++) {																								// Go to the last node entry
		node = node->next;
	}
	
	return node;
}

static int get_lastn(int op1, int op2, int op3, int ops) {
	return ops == 3 ? op3 : (ops == 2 ? op2 : op1);																				// Just return the last node type
}

static int x86_gen(codegen_t *codegen, node_t *node) {
	if (codegen == NULL || node == NULL) {																						// Null pointer check
		return 0;
	} else if (node->type == NODE_TYPE_REP || node->type == NODE_TYPE_REPE) {													// REP/REPE/REPZ?
		codegen_write_byte(codegen, 0xF3);																						// Yes :)
		return 1;
	} else if (node->type == NODE_TYPE_LOCK) {																					// LOCK?
		codegen_write_byte(codegen, 0xF0);																						// Yes :)
		return 1;
	} else if (node->type == NODE_TYPE_REPNE) {																					// REPNE/REPNZ?
		codegen_write_byte(codegen, 0xF2);																						// Yes :)
		return 1;
	} else if (node->type != NODE_TYPE_INSTRUCTION) {																			// Our node?
		return 0;																												// Nope
	}
	
	instruction_node_t *inod = (instruction_node_t*)node;
	
	uint32_t ops = count_childs(node->childs);
	uint32_t bestop1 = 0;
	uint32_t bestop2 = 0;
	uint32_t bestop3 = 0;
	int opcode2 = 0;
	int opcode = 0;
	int exists = 0;
	int found = 0;
	int instc = 0;
	int inst = 0;
	int op1 = 0;
	int op2 = 0;
	int op3 = 0;
	
	if (ops > 3) {																												// Too many args (> 3)?
		printf("too many operands to the instruction '%s'\n", inod->name);														// ...
		return -1;
	} else if (ops == 1) {
		op1 = get_optype(node->childs);																							// Get the optype from the first operand
	} else if (ops == 2) {
		op1 = get_optype(node->childs);																							// Get the optype from the first operand
		op2 = get_optype(node->childs->next);																					// Get the optype from the second operand
	} else if (ops == 3) {
		op1 = get_optype(node->childs);																							// Get the optype from the first operand
		op2 = get_optype(node->childs->next);																					// Get the optype from the second operand
		op3 = get_optype(node->childs->next->next);																				// Get the optype from the third operand
	}
	
	for (; instc < 1079; instc++) {																								// Let's try to find this instruction!
		if ((strlen(instructions[instc].name) != strlen(inod->name)) || strcasecmp(instructions[instc].name, inod->name)) {		// Same name?
			continue;																											// Nope
		} else {
			exists = 1;																											// Ok, at least this instr exists!
		}
		
		if (instructions[instc].args != ops) {																					// Same operand count?
			continue;																											// Nope
		} else if (ops == 1 && (op1 & instructions[instc].arg1) == 0) {															// Valid operands (we only need to check one)
			continue;
		} else if (ops == 2 && ((op1 & instructions[instc].arg1) == 0 || (op2 & instructions[instc].arg2) == 0)) {				// We need to check two operands
			continue;
		} else if (ops == 3 && ((op1 & instructions[instc].arg1) == 0 || (op2 & instructions[instc].arg2) == 0 ||
				   				(op3 & instructions[instc].arg3) == 0)) {														// We need to check three operands
			continue;
		}
		
		if (ops == 0) {
			inst = instc;
		} else if ((ops >= 1 && instructions[instc].arg1 - (op1 & instructions[instc].arg1) < bestop1) || !found) {				// Best one?
			inst = instc;																										// Yes
			bestop1 = instructions[instc].arg1 - (op1 & instructions[instc].arg1);
			bestop2 = instructions[instc].arg2 - (op2 & instructions[instc].arg2);
			bestop3 = instructions[instc].arg3 - (op3 & instructions[instc].arg3);
		} else if (ops >= 2 && instructions[instc].arg2 - (op2 & instructions[instc].arg2) < bestop2) {							// Best one?
			inst = instc;																										// Yes
			bestop1 = instructions[instc].arg1 - (op1 & instructions[instc].arg1);
			bestop2 = instructions[instc].arg2 - (op2 & instructions[instc].arg2);
			bestop3 = instructions[instc].arg3 - (op3 & instructions[instc].arg3);
		} else if (ops >= 3 && instructions[instc].arg3 - (op3 & instructions[instc].arg3) < bestop3) {							// Best one?
			inst = instc;																										// Yes
			bestop1 = instructions[instc].arg1 - (op1 & instructions[instc].arg1);
			bestop2 = instructions[instc].arg2 - (op2 & instructions[instc].arg2);
			bestop3 = instructions[instc].arg3 - (op3 & instructions[instc].arg3);
		}
		
		found = 1;
	}
	
	if (!found && exists) {																										// Invalid operands?
		printf("invalid operands to the instruction '%s'\n", inod->name);														// Yeah...
		return -1;
	} else if (!exists) {
		printf("invalid instruction '%s'\n", inod->name);																		// ... This wasn't supposed to happen
		return -1;
	}
	
	if (instructions[inst].alt_addr) {																							// Write the address size override?
		codegen_write_byte(codegen, 0x66);																						// Yes
	}
	
	if (node->childs != NULL && node->childs->type == NODE_TYPE_ADDRESS) {														// Let's see if we have a segment override in the first op
		x86_write_segover(codegen, node->childs->childs);																		// Ok!
	}
	
	if (node->childs != NULL && node->childs->next != NULL && node->childs->next->type == NODE_TYPE_ADDRESS) {					// Let's see if the second one have!
		x86_write_segover(codegen, node->childs->next->childs);																	// Ok!
	}
	
	if ((instructions[inst].optype & INSTR_TYPE_OPREGB) && (instructions[inst].postop == -1)) {									// Add the register to the first part of the opcode? (+rb)
		node_t *n = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) :
					(ops >= 3 ? node->childs->next : node->childs);
		opcode = instructions[inst].opcode + get_gregb(((register_node_t*)n)->name);
	} else if ((instructions[inst].optype & INSTR_TYPE_OPREGW) && (instructions[inst].postop == -1)) {							// Add the register to the first part of the opcode? (+rw)
		node_t *n = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) :
					(ops >= 3 ? node->childs->next : node->childs);
		opcode = instructions[inst].opcode + get_gregw(((register_node_t*)n)->name);
	} else if ((instructions[inst].optype & INSTR_TYPE_OPREGD) && (instructions[inst].postop == -1)) {							// Add the register to the first part of the opcode? (+rd)
		node_t *n = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) :
					(ops >= 3 ? node->childs->next : node->childs);
		int opt = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_lastn(op1, op2, op3, ops) :
					(ops >= 3 ? op2 : op1);
		opcode = instructions[inst].opcode + ((opt & INSTR_ARG_STREG) ? get_streg(((register_node_t*)n)->name) :
											  							get_gregd(((register_node_t*)n)->name));
	} else {																													// Just the first part of the opcode
		opcode = instructions[inst].opcode;
	}
	
	if ((instructions[inst].optype & INSTR_TYPE_OPREGB) && (instructions[inst].postop != -1)) {									// Add the register to the second part of the opcode? (+rb)
		node_t *n = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) :
					(ops >= 3 ? node->childs->next : node->childs);
		opcode2 = instructions[inst].postop + get_gregb(((register_node_t*)n)->name);
	} else if ((instructions[inst].optype & INSTR_TYPE_OPREGW) && (instructions[inst].postop != -1)) {							// Add the register to the opcode? (+rw)
		node_t *n = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) :
					(ops >= 3 ? node->childs->next : node->childs);
		opcode2 = instructions[inst].postop + get_gregw(((register_node_t*)n)->name);
	} else if ((instructions[inst].optype & INSTR_TYPE_OPREGD) && (instructions[inst].postop != -1)) {							// Add the register to the opcode? (+rd)
		node_t *n = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) :
					(ops >= 3 ? node->childs->next : node->childs);
		int opt = ops >= 2 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? get_lastn(op1, op2, op3, ops) :
					(ops >= 3 ? op2 : op1);
		opcode2 = instructions[inst].postop + ((opt & INSTR_ARG_STREG) ? get_streg(((register_node_t*)n)->name) :
											  (((uint8_t)instructions[inst].postop) == 0x78 ?
											   -(get_gregd(((register_node_t*)n)->name) * 8) :
											   get_gregd(((register_node_t*)n)->name)));
	} else {																													// Just the second part of the opcode?
		opcode2 = instructions[inst].postop;
	}
	
	if (opcode <= 0xFF) {																										// Write the first part of the opcode
		codegen_write_byte(codegen, opcode);
	} else {
		codegen_write_word(codegen, opcode);
	}
	
	if (opcode2 != -1 && opcode2 <= 0xFF) {																						// Two bytes opcode?
		codegen_write_byte(codegen, opcode2);																					// Yes
	} else if (opcode2 != -1 && opcode2 <= 0xFFFF) {																			// Three bytes opcode?
		codegen_write_word(codegen, opcode2);																					// Yes
	} else if (opcode2 != -1 && opcode2 <= 0xFFFFFF) {																			// Four bytes opcode?
		codegen_write_byte(codegen, (uint8_t)opcode2);																			// Yes
		codegen_write_word(codegen, (uint16_t)(opcode2 >> 8));
	} else if (opcode2 != -1) {
		codegen_write_dword(codegen, opcode2);																					// Five bytes opcode
	}
	
	if (instructions[inst].optype & INSTR_TYPE_RELB) {																			// 1 bytes after the opcode, but the value it's relative to the end of the instr, not absolute
		codegen_write_byte(codegen, get_opval(codegen, ops >= 2 ? get_last(node->childs, ops) : node->childs, 1, 1));
	} else if (instructions[inst].optype & INSTR_TYPE_RELW) {																	// 2 bytes after the opcode, but the value it's relative to the end of the instr, not absolute
		codegen_write_dword(codegen, get_opval(codegen, ops >= 2 ? get_last(node->childs, ops) : node->childs, 2, 1));
	} else if (instructions[inst].optype & INSTR_TYPE_RELD) {																	// 4 bytes after the opcode, but the value it's relative to the end of the instr, not absolute
		codegen_write_dword(codegen, get_opval(codegen, ops >= 2 ? get_last(node->childs, ops) : node->childs, 4, 1));
	} else if (instructions[inst].optype == (INSTR_TYPE_WORD | INSTR_TYPE_BYTE)) {												// 3 bytes after the opcode
		codegen_write_word(codegen, (uint16_t)get_opval(codegen, node->childs, 2, 0));											// Write!
		codegen_write_byte(codegen, (uint8_t)get_opval(codegen, node->childs->next, 1, 0));
	} else if (instructions[inst].optype & INSTR_TYPE_BYTE && !(instructions[inst].optype & INSTR_TYPE_MODRM)) {				// 1 byte after the opcode
		node_t *args = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) : node->childs;
		
		if (args->type == NODE_TYPE_ADDRESS) {																					// Moffs?
			codegen_write_byte(codegen, (uint8_t)get_opval(codegen, args->childs, 1, 0));										// Yeah!
		} else {
			codegen_write_byte(codegen, (uint8_t)get_opval(codegen, args, 1, 0));												// Nope
		}
	} else if (instructions[inst].optype & INSTR_TYPE_WORD && !(instructions[inst].optype & INSTR_TYPE_MODRM)) {				// 2 byte after the opcode
		node_t *args = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) : node->childs;
		
		if (args->type == NODE_TYPE_ADDRESS) {																					// Moffs?
			codegen_write_word(codegen, (uint16_t)get_opval(codegen, args->childs, 2, 0));										// Yeah!
		} else {
			codegen_write_word(codegen, (uint16_t)get_opval(codegen, args, 2, 0));												// Nope
		}
	} else if (instructions[inst].optype & INSTR_TYPE_DWORD && !(instructions[inst].optype & INSTR_TYPE_MODRM)) {				// 4 byte after the opcode
		node_t *args = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) : node->childs;
		
		if (args->type == NODE_TYPE_ADDRESS) {																					// Moffs?
			codegen_write_dword(codegen, get_opval(codegen, args->childs, 4, 0));												// Yeah!
		} else {
			codegen_write_dword(codegen, get_opval(codegen, args, 4, 0));														// Nope
		}
	} else if ((instructions[inst].optype & (INSTR_TYPE_MODRM | INSTR_TYPE_BYTE)) == (INSTR_TYPE_MODRM | INSTR_TYPE_BYTE)) {	// ModR/M + Byte
		int op = instructions[inst].optype & INSTR_TYPE_LEFT ? get_lastn(op1, op2, op3, ops) : op1;
		int reg = -1;
		node_t *n1 = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? node->childs : get_last(node->childs, ops);
		node_t *n2 = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) : node->childs;
		
		if (ops >= 3 && (op1 & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG |
								INSTR_ARG_GREGB | INSTR_ARG_GREGW | INSTR_ARG_GREGD))) {										// Reg first
			reg = (op1 & INSTR_ARG_SREG) ? get_sreg(((register_node_t*)n1)->name) : 
				  ((op1 & INSTR_ARG_CREG) ? get_creg(((register_node_t*)n1)->name) :
				  ((op1 & INSTR_ARG_DREG) ? get_dreg(((register_node_t*)n1)->name) :
				  ((op1 & INSTR_ARG_TREG) ? get_treg(((register_node_t*)n1)->name) :
				  get_gregd(((register_node_t*)n1)->name))));																	// Yes
			n1 = n1->next;																										// The MODR/M is in the next op
			op = op2;
		}
		
		if (!x86_write_modrm(codegen, instructions[inst].extension, reg, op, n1)) {												// Write the ModR/M
			printf("invalid operands to the instruction '%s'\n", inod->name);													// Failed :(
			return 1;
		}
		
		codegen_write_byte(codegen, (uint8_t)get_opval(codegen, n2, 1, 0));														// Write the byte
	} else if ((instructions[inst].optype & (INSTR_TYPE_MODRM | INSTR_TYPE_WORD)) == (INSTR_TYPE_MODRM | INSTR_TYPE_WORD)) {	// ModR/M + Word
		int op = instructions[inst].optype & INSTR_TYPE_LEFT ? get_lastn(op1, op2, op3, ops) : op1;
		int reg = -1;
		node_t *n1 = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? node->childs : get_last(node->childs, ops);
		node_t *n2 = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) : node->childs;
		
		if (ops >= 3 && (op1 & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG | INSTR_ARG_GREGB |
								INSTR_ARG_GREGW | INSTR_ARG_GREGD))) {															// Reg first
			reg = (op1 & INSTR_ARG_SREG) ? get_sreg(((register_node_t*)n1)->name) : 
				  ((op1 & INSTR_ARG_CREG) ? get_creg(((register_node_t*)n1)->name) :
				  ((op1 & INSTR_ARG_DREG) ? get_dreg(((register_node_t*)n1)->name) :
				  ((op1 & INSTR_ARG_TREG) ? get_treg(((register_node_t*)n1)->name) :
				  get_gregd(((register_node_t*)n1)->name))));																	// Yes
			n1 = n1->next;																										// The MODR/M is in the next op
			op = op2;
		}
		
		if (!x86_write_modrm(codegen, instructions[inst].extension, reg, op, n1)) {												// Write the ModR/M
			printf("invalid operands to the instruction '%s'\n", inod->name);													// Failed :(
			return 1;
		}
		
		codegen_write_word(codegen, (uint16_t)get_opval(codegen, n2, 2, 0));													// Write the word
	} else if ((instructions[inst].optype & (INSTR_TYPE_MODRM | INSTR_TYPE_DWORD)) == (INSTR_TYPE_MODRM | INSTR_TYPE_DWORD)) {	// ModR/M + DWord
		int op = instructions[inst].optype & INSTR_TYPE_LEFT ? get_lastn(op1, op2, op3, ops) : op1;
		int reg = -1;
		node_t *n1 = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? node->childs : get_last(node->childs, ops);
		node_t *n2 = ops >= 2 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? get_last(node->childs, ops) : node->childs;
		
		if (ops >= 3 && (op1 & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG | INSTR_ARG_GREGB |
								INSTR_ARG_GREGW | INSTR_ARG_GREGD))) {															// Reg first
			reg = (op1 & INSTR_ARG_SREG) ? get_sreg(((register_node_t*)n1)->name) : 
				  ((op1 & INSTR_ARG_CREG) ? get_creg(((register_node_t*)n1)->name) :
				  ((op1 & INSTR_ARG_DREG) ? get_dreg(((register_node_t*)n1)->name) :
				  ((op1 & INSTR_ARG_TREG) ? get_treg(((register_node_t*)n1)->name) :
				  get_gregd(((register_node_t*)n1)->name))));																	// Yes
			n1 = n1->next;																										// The MODR/M is in the next op
			op = op2;
		}
		
		if (!x86_write_modrm(codegen, instructions[inst].extension, reg, op, n1)) {												// Write the ModR/M
			printf("invalid operands to the instruction '%s'\n", inod->name);													// Failed :(
			return 1;
		}
		
		codegen_write_dword(codegen, get_opval(codegen, n2, 4, 0));																// Write the dword
	} else if (instructions[inst].optype & INSTR_TYPE_MODRM) {																	// ModR/M
		node_t *n = (ops == 3 && (instructions[inst].optype & INSTR_TYPE_LEFT)) ||
					(instructions[inst].optype & INSTR_TYPE_OPREGD) ? node->childs->next : node->childs;
		int opa = (ops == 3 && (instructions[inst].optype & INSTR_TYPE_LEFT)) ||
				  (instructions[inst].optype & INSTR_TYPE_OPREGD) ? op2 : op1;
		int opb = ops == 3 && !(instructions[inst].optype & INSTR_TYPE_LEFT) ? op3 : op2;
		int reg = -1;
		
		if (!(ops == 2 && (instructions[inst].optype & INSTR_TYPE_OPREGD))) {
			if (opa & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG | INSTR_ARG_XREG | INSTR_ARG_GREGB |
					   INSTR_ARG_GREGW | INSTR_ARG_GREGD)) {																	// Reg first?
				reg = (opa & INSTR_ARG_SREG) ? get_sreg(((register_node_t*)n)->name) : 
					  ((opa & INSTR_ARG_CREG) ? get_creg(((register_node_t*)n)->name) :
					  ((opa & INSTR_ARG_DREG) ? get_dreg(((register_node_t*)n)->name) :
					  ((opa & INSTR_ARG_TREG) ? get_treg(((register_node_t*)n)->name) :
					  ((opa & INSTR_ARG_XREG) ? get_xreg(((register_node_t*)n)->name) :
					  get_gregd(((register_node_t*)n)->name)))));																// Yes
				n = ops == 3 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? n->next->next : n->next;						// The MODR/M is in the next op
				opa = ops == 3 && (instructions[inst].optype & INSTR_TYPE_LEFT) ? op3 : (ops == 3 ? op3 : op2);
			} else if (opb & (INSTR_ARG_SREG | INSTR_ARG_CREG | INSTR_ARG_DREG | INSTR_ARG_TREG | INSTR_ARG_XREG |
							  INSTR_ARG_GREGB | INSTR_ARG_GREGW | INSTR_ARG_GREGD)) {											// Reg in the end?
				reg = (opb & INSTR_ARG_SREG) ? get_sreg(((register_node_t*)n->next)->name) : 
					  ((opb & INSTR_ARG_CREG) ? get_creg(((register_node_t*)n->next)->name) :
					  ((opb & INSTR_ARG_DREG) ? get_dreg(((register_node_t*)n->next)->name) :
					  ((opb & INSTR_ARG_TREG) ? get_treg(((register_node_t*)n->next)->name) :
					  ((opb & INSTR_ARG_XREG) ? get_xreg(((register_node_t*)n->next)->name) :
					  get_gregd(((register_node_t*)n->next)->name)))));															// Yes
			}

			if (ops == 3 && (instructions[inst].optype & INSTR_TYPE_LEFT)) {													// *HACK* (i think)
				reg <<= 1;
			}
		}
		
		if (!x86_write_modrm(codegen, instructions[inst].extension, reg, opa, n)) {												// Write it!
			printf("invalid operands to the instruction '%s'\n", inod->name);													// Failed :(
			return 1;
		}
	} else if (instructions[inst].optype == INSTR_TYPE_POINTER) {																// Pointer (16:32)
		codegen_write_dword(codegen, get_opval(codegen, node->childs->childs->next, 4, 0));										// Write the right part
		codegen_write_word(codegen, (uint16_t)get_opval(codegen, node->childs->childs, 2, 0));									// Write the left part
	} else if (instructions[inst].optype != INSTR_TYPE_NONE && !(instructions[inst].optype & INSTR_TYPE_OPREGW) &&
			   !(instructions[inst].optype & INSTR_TYPE_OPREGD)) {
		printf("invalid operands to the instruction '%s'\n", inod->name);														// ...
		return 1;
	}
	
	return 1;
}

static uint8_t x86_ttype(char *ident) {
	if (ident != NULL && x86_find_register(ident)) {																			// Register?
		return TOK_TYPE_REGISTER;																								// Yes!
	} else {
		return TOK_TYPE_IDENTIFIER;																								// No, so it's an normal identifier
	}
}

static void x86_tfree(token_t *token) {
	if (token != NULL && token->type == TOK_TYPE_REGISTER) {																	// Register?
		free(token->value);																										// Yes, free it!
	}
}

static void x86_tprint(token_t *token) {
	if (token == NULL) {																										// Null pointer check
		return;
	} else if (token->type == TOK_TYPE_MUL) {																					// Multiply?
		printf("Multiply\n");																									// Yes, print it
	} else if (token->type == TOK_TYPE_OBRAC) {																					// Opening Bracket?
		printf("Opening Bracket\n");																							// Yes, print it
	} else if (token->type == TOK_TYPE_CBRAC) {																					// Closing Bracket?
		printf("Closing Bracket\n");																							// Yes, print it
	} else if (token->type == TOK_TYPE_REGISTER) {																				// Register?
		printf("Register: %s\n", token->value);																					// Yes, print it
	}
}

REGISTER_ARCH(x86, "x86", "elf32", x86_help, x86_option, x86_lex, x86_parse, x86_gen, x86_ttype, x86_tfree, x86_tprint);		// Register this architecture
