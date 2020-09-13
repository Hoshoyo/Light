#pragma once
#include <stdint.h>
typedef unsigned char u8;
typedef char s8;
typedef int bool;

#define true 1
#define false 0

typedef struct {
	u8 B : 1;		// Extension of the ModR/M r/m field (src register), SIB base field, or Opcode reg field
	u8 X : 1;		// Extension of the SIB index field
	u8 R : 1;		// Extension of the ModR/M reg (dest register) field
	u8 W : 1;		// 0 = Operand size determined by CS.D. 1 = 64 bit operand size
	u8 high : 4;	// Must be 4 (0b0100)
} X64_REX;

typedef union {
    uint8_t  v8;
    uint16_t v16;
    uint32_t v32;
    uint64_t v64;
} Int_Value;
typedef union {
	int8_t  v8;
	int16_t v16;
	int32_t v32;
	int64_t v64;
} SInt_Value;
	
typedef enum {
	REG_NONE = -1,
	// 64 bit
	RAX = 0, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
	R8, R9,	R10, R11, R12, R13, R14, R15,

	// 32 bit
	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
    R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D,

	// 16 bit
	AX, CX, DX, BX, SP, BP, SI, DI,
    R8W, R9W, R10W, R11W, R12W, R13W, R14W, R15W,

	// 8 bit
	AL, CL, DL, BL, AH, CH, DH, BH,
    R8B, R9B, R10B, R11B, R12B, R13B, R14B, R15B,

	SPL, BPL, SIL, DIL
} X64_Register;

typedef enum  {
	INDIRECT = 0,
	INDIRECT_BYTE_DISPLACED = 1,
	INDIRECT_DWORD_DISPLACED = 2,
	DIRECT = 3,
} X64_Addressing_Mode;

// mov
u8* emit_mov_mr_indirect(u8* stream, X64_Register dest, X64_Register src);
u8* emit_mov_mr_direct(u8* stream, X64_Register dest, X64_Register src);

u8* emit_mov_test(u8* stream);
u8* emit_arith_test(u8* stream);