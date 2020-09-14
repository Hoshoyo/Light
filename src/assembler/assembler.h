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

	SPL, BPL, SIL, DIL,
} X64_Register;

typedef enum {
	XMM0 = 0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8
} X64_XMM_Register;

typedef enum  {
	INDIRECT = 0,
	INDIRECT_BYTE_DISPLACED = 1,
	INDIRECT_DWORD_DISPLACED = 2,
	DIRECT = 3,
} X64_Addressing_Mode;

typedef enum {
	ARITH_ADD = 0,
	ARITH_OR = 1,
	ARITH_ADC = 2,	// add with carry
	ARITH_SBB = 3, 	// subtract with borrow
	ARITH_AND = 4,
	ARITH_SUB = 5,
	ARITH_XOR = 6,
	ARITH_CMP = 7,
} X64_Arithmetic_Instr;

typedef enum {
	NOT = 2,
	NEG = 3,
	MUL = 4,
	IMUL = 5,
	DIV = 6,
	IDIV = 7,
} X64_Multiplicative_Instr;

// prefix 0x0f for near + nearopcode
typedef enum {
	// BOTH
	JE   = 0x74,		// op1 == op2
	JZ   = 0x74,		// op1 == op2
	JNE  = 0x75,		// op1 != op2
	JNZ  = 0x75,		// op1 != op2
	
	// UNSIGNED
	JA   = 0x77,		// op1 > op2
	JNA  = 0x76,		// !(op1 > op2)
	JAE  = 0x73,		// op1 >= op2
	JNAE = 0x72,		// !(op1 >= op2)
	JB   = 0x72,		// op1 < op2
	JNC  = 0x73,		// !(op1 < op2)
	JBE  = 0x76,		// op1 <= op2
	JNBE = 0x77,		// !(op1 <= op2)

	//SIGNED
	JG   = 0x7f,		// op1 > op2
	JNG  = 0x7e,		// !(op1 > op2)
	JGE  = 0x7d,		// op1 >= op2
	JNGE = 0x7c,		// !(op1 >= op2)
	JL   = 0x7c,		// op1 < op2
	JNL  = 0x7d,		// !(op1 < op2)
	JLE  = 0x7e,		// op1 <= op2
	JNLE = 0x7f,		// !(op1 <= op2)

	// SPECIAL
	JO   = 0x70,		// jump if overflow
	JNO  = 0x71,		// jump if not overflow
	JS   = 0x78,		// jump if sign
	JNS  = 0x79,		// jump if not sign
	JP   = 0x7A,		// jump if parity
	JPE  = 0x7A,		// jump if parity even
	JNP  = 0x7B,		// jump if not parity
	JPO  = 0x7B,		// jump if parity odd
	JCXZ  = 0xe3,		// jump if register CX is zero
	JECXZ = 0xe3,		// jump if register ECX is zero
	JRCXZ = 0xe3,		// jump if register RCX is zero
} X64_Jump_Conditional_Short;

typedef enum {
	XMM_ADDS = 0x58,
	XMM_SUBS = 0x5C,
	XMM_MULS = 0x59,
	XMM_DIVS = 0x5E,
} X64_XMM_Arithmetic_Instr;

typedef enum {
	SSE_CMP_EQ = 0,
	SSE_CMP_LT = 1,
	SSE_CMP_LE = 2,
} X64_SSE_Compare_Flag;

typedef enum {
	SSE_CVT_F32_INT32 = 0x2D,
	SSE_CVT_INT32_F32 = 0x2A,
	SSE_CVT_F64_F32 = 0x5A,
	SSE_CVT_F64_INT32 = 0x2C,
} X64_SSE_Convert_Instr;

typedef enum {
	ROL = 0,
	ROR = 1,
	RCL = 2,
	RCR = 3,
	SHL = 4,
	SHR = 5,
	SAR = 7
} X64_Shift_Instruction;

// mov
u8* emit_mov_mr_indirect(u8* stream, X64_Register dest, X64_Register src);
u8* emit_mov_mr_direct(u8* stream, X64_Register dest, X64_Register src);

u8* emit_mov_test(u8* stream);
u8* emit_add_test(u8* stream);
u8* emit_and_test(u8* stream);
u8* emit_float_test(u8* stream);
u8* emit_test_mul(u8* stream);
u8* emit_jmp_cond_test(u8* stream);
u8* emit_shl_test(u8* stream);