#pragma once
#include <assert.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
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
	
typedef enum {
	REGISTER_NONE = -1,
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

	// Segment registers
	ES, CS, SS, DS, FS, GS,

	REG_COUNT,
} X64_Register;

typedef enum {
	DR0 = 0, DR1, DR2, DR3, DR4, DR5, DR6, DR7
} X64_DebugRegister;

typedef enum {
	CR0 = 0, CR1, CR2, CR3, CR4, CR5, CR6, CR7,
	CR8, CR9, CR10, CR11, CR12, CR13, CR14, CR15, // Extended
} X64_ControlRegister;

typedef enum {
	XMM0 = 0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8
} X64_XMM_Register;

typedef enum  {
	MODE_NONE                = -1,
	INDIRECT                 = 0,
	INDIRECT_BYTE_DISPLACED  = 1,
	INDIRECT_DWORD_DISPLACED = 2,
	DIRECT                   = 3,
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
	BITTEST = 4,
	BITTEST_COMPLEMENT = 7,
	BITTEST_RESET = 6,
	BITTEST_SET = 5
} X64_BitTest_Instr;

typedef enum {
	ROL = 0,	// Rotate left
	ROR = 1,	// Rotate right
	RCL = 2,	// Rotate left + 1
	RCR = 3,	// Rotate right + 1
	SHL = 4,	// Shift left
	SHR = 5,	// Shift right
	SAR = 7		// Shift arithmetic right
} X64_Shift_Instruction;

typedef enum {
	NOT = 2,	// logic not
	NEG = 3,	// negative (0-x)
	MUL = 4,	// multiply unsigned
	IMUL = 5,	// multiply signed
	DIV = 6,	// divide unsigned
	IDIV = 7,	// divide signed
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
	JBE  = 0x76,		// op1 <= op2
	JAE  = 0x73,		// op1 >= op2
	JNAE = 0x72,		// !(op1 >= op2)
	JB   = 0x72,		// op1 < op2
	JNC  = 0x73,		// !(op1 < op2)
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
	LOOP	= 0xe2,
	LOOPE	= 0xe1,
	LOOPNE	= 0xe0,
} X64_Loop_Short;

typedef enum {
	UD0 = 0xff,
	UD1 = 0xb9,
	UD2 = 0x0b,
} X64_UD_Instruction;

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
	SSE_CMP_UNORD = 3,
	SSE_CMP_NE = 4,
	SSE_CMP_NL = 5,
	SSE_CMP_NLE = 6,
	SSE_CMP_ORD = 7
} X64_SSE_Compare_Flag;

typedef enum {
	SSE_CVT_F32_INT32 = 0x2c, // 0x2D -> no truncation
	SSE_CVT_INT32_F32 = 0x2A,
	SSE_CVT_F64_F32 = 0x5A,
	SSE_CVT_F64_INT32 = 0x2c, // 0x2D -> no truncation
} X64_SSE_Convert_Instr;

typedef enum {
	CMOVE   = 0x44,	// equal
	CMOVZ   = 0x44, // zero
	CMOVNE  = 0x45,	// not equal
	CMOVNZ  = 0x45, // not zero
	CMOVA   = 0x47,	// above
	CMOVNBE = 0x47, // not below or equal
	CMOVAE  = 0x43,	// above or equal
	CMOVNB	= 0x43, // not below
	CMOVNC  = 0x43, // not carry
	CMOVB   = 0x42,	// below
	CMOVC   = 0x42,	// carry = 1
	CMOVNAE = 0x42, // not above or equal
	CMOVBE  = 0x46,	// below or equal
	CMOVNA  = 0x46, // not above
	CMOVG   = 0x4f,	// greater
	CMOVNLE = 0x4f, // not less or equal
	CMOVGE  = 0x4d,	// greater or equal
	CMOVNL  = 0x4d, // not less
	CMOVL   = 0x4c,	// less
	CMOVNGE = 0x4c, // not greater or equal
	CMOVLE  = 0x4e,	// less or equal
	CMVONG  = 0x4e, // not greater
	CMOVNO  = 0x41, // not overflow
	CMOVNP  = 0x4b, // not parity
	CMOVPO  = 0x4b, // parity odd
	CMOVNS  = 0x49, // not sign
	CMOVO   = 0x40, // overflow
	CMOVP   = 0x4a, // parity
	CMOVPE  = 0x4a, // parity even
	CMOVS   = 0x48, // sign
} X64_CMOVcc_Instruction;

typedef enum {
	SETE   = 0x94,	// equal
	SETZ   = 0x94, 	// zero
	SETNE  = 0x95,	// not equal
	SETNZ  = 0x95, 	// not zero
	SETA   = 0x97,	// above
	SETNBE = 0x97, 	// not below or equal
	SETAE  = 0x93,	// above or equal
	SETNB  = 0x93, 	// not below
	SETNC  = 0x93, 	// not carry
	SETB   = 0x92,	// below
	SETC   = 0x92,	// carry = 1
	SETNAE = 0x92, 	// not above or equal
	SETBE  = 0x96,	// below or equal
	SETNA  = 0x96, 	// not above
	SETG   = 0x9f,	// greater
	SETNLE = 0x9f, 	// not less or equal
	SETGE  = 0x9d,	// greater or equal
	SETNL  = 0x9d,	// not less
	SETL   = 0x9c,	// less
	SETNGE = 0x9c, 	// not greater or equal
	SETLE  = 0x9e,	// less or equal
	SETNG  = 0x9e, 	// not greater
	SETNO  = 0x91, 	// not overflow
	SETNP  = 0x9b, 	// not parity
	SETPO  = 0x9b, 	// parity odd
	SETNS  = 0x99, 	// not sign
	SETO   = 0x90, 	// overflow
	SETP   = 0x9a, 	// parity
	SETPE  = 0x9a, 	// parity even
	SETS   = 0x98, 	// sign
} X64_SETcc_Instruction;

typedef enum {
	RET_NEAR,
	RET_FAR,
} X64_Ret_Instruction;

typedef struct {
	s8 instr_byte_size;
	s8 diplacement_offset;
	s8 immediate_offset;
} Instr_Emit_Result;

#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define MIN(A, B) (((A) < (B)) ? (A) : (B))

static int
value_bitsize(u64 value)
{
    if(value > 0xffffffff)
        return 64;
	else if(value > 0xffff)
        return 32;
	else if(value > 0xff)
        return 16;
    else if (value > 0)
        return 8;
	else
		return 0;
}

// mod 2 bits 
// rm  3 bits	= right side = src
// reg 3 bits	= left  side = dest
static u8 
make_modrm(u8 mod, u8 reg, u8 rm) {
	assert(mod < 4);
	assert(rm  < 8);
	assert(reg < 8);
	return (mod << 6) | (reg << 3) | rm;
}

static u8
make_rex(int b, int x, int r, int w) {
	X64_REX res = {0};
	res.high = 4;
	res.B = b;
	res.X = x;
	res.R = r;
	res.W = w;
	return *(u8*)&res;
}

static u8
make_sib(u8 scale, u8 index, u8 base) {
	assert(scale < 4);
	assert(index < 8);
	assert(base  < 8);
	return (scale << 6) | (index << 3) | base;
}

static u8
register_representation(X64_Register r)
{
	if(r < R8) return r;
	if(r < EAX) return (r - R8);
	if(r < R8D) return (r - EAX);
	if(r < AX) return (r - R8D);
	if(r < R8W) return (r - AX);
	if(r < AL) return (r - R8W);
	if(r < R8B) return (r - AL);
	if(r < SPL) return (r - R8B);
	if(r < ES) return (r - SPL) + 4;
	if(r < REG_COUNT) return (r - ES);
	return (r - REG_COUNT);
}

static bool
register_equivalent(X64_Register r, X64_Register c)
{
	if(r <= R15W && c <= R15W) return register_representation(r) == register_representation(c);
	if (r == RSP && c == SPL) return true;
	if (r == RBP && c == BPL) return true;
	return register_representation(r) == register_representation(c);
}

static bool
register_is_extended(X64_Register r)
{
	return (r >= R8 && r <= R15) ||
		(r >= R8D && r <= R15D) ||
		(r >= R8W && r <= R15W) ||
		(r >= R8B && r <= R15B);
}

static bool
register_is_segment(X64_Register r)
{
	return(r >= ES && r <= GS);
}

static int
register_get_bitsize(X64_Register r)
{
    if(r >= RAX && r <= R15) return 64;
    if(r >= EAX && r <= R15D) return 32;
    if(r >= AX && r <= R15W) return 16;
	if(r >= ES && r <= GS) return 16;
    return 8;
}

static void
fill_outinfo(Instr_Emit_Result* out_info, s8 byte_size, s8 disp_offset, s8 imm_offset)
{
    if(out_info)
    {
        out_info->instr_byte_size = byte_size;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = imm_offset;
    }
}

static u8*
emit_value_raw(u8* stream, u64 value, s32 bitsize)
{
    switch(bitsize)
    {
        case 64: *(u64*)stream = value;      stream += sizeof(u64); break;
        case 32: *(u32*)stream = (u32)value; stream += sizeof(u32); break;
        case 16: *(u16*)stream = (u16)value; stream += sizeof(u16); break;
        case 8:  *(u8*)stream  = (u8) value; stream += sizeof(u8); break;
    }

    return stream;
}

static u8*
emit_single_byte_instruction(Instr_Emit_Result* out_info, u8* stream, u8 opcode)
{
    *stream++ = opcode;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

typedef struct {
    u8 bytes[3];
    s8 byte_count;
} X64_Opcode;

typedef enum {
	ADDR_BYTEPTR  = 8,
	ADDR_WORDPTR  = 16,
	ADDR_DWORDPTR = 32,
	ADDR_QWORDPTR = 64,
} X64_AddrSize;

typedef enum {
	SIB_X1 = 0,
	SIB_X2 = 1,
	SIB_X4 = 2,
	SIB_X8 = 3,
} X64_SibMode;

typedef enum {
	ADDR_MODE_NONE = 0,
	ADDR_MODE_M,
	ADDR_MODE_RM,
	ADDR_MODE_MR,
	ADDR_MODE_OI,
	ADDR_MODE_MI,
	ADDR_MODE_RMI,
	ADDR_MODE_MRI,
	ADDR_MODE_MRC,
	ADDR_MODE_TD,
	ADDR_MODE_FD,
	ADDR_MODE_ZO,
	ADDR_MODE_M1,
	ADDR_MODE_MC,
	ADDR_MODE_I,
	ADDR_MODE_O,
	ADDR_MODE_D,
	ADDR_MODE_A,
	ADDR_MODE_B,
} X64_AddrMode_Type;

#define ADDRMODE_FLAG_NO_SIZE_OVERRIDE (1 << 0)
#define ADDRMODE_FLAG_NO_REX  (1 << 1)
#define ADDRMODE_FLAG_NO_REXW (1 << 2)
#define ADDRMODE_FLAG_REXW    (1 << 3)

typedef struct {
	X64_AddrMode_Type   mode_type;
	X64_Register        reg;
	X64_Register        rm;		// sib base whenever sib is the mode
	X64_Register        sib_index;
	X64_Register        sib_base;
	X64_Register        moffs_base;
	X64_Addressing_Mode addr_mode;
	X64_SibMode         sib_mode;

	u64 displacement;
	u64 immediate;

	s32 displacement_bitsize;
	s32 immediate_bitsize;

	s32 ptr_bitsize;	// only valid when rm is not REGISTER_NONE

	u32 flags;
} X64_AddrMode;

static u8*
emit_size_override(u8* stream, X64_AddrSize ptr_size, X64_Register rm, X64_Addressing_Mode mode)
{
    if(mode != DIRECT && register_get_bitsize(rm) == 32)
    {
        // Address-size override prefix.
        *stream++ = 0x67;
    }

	// TODO(psv): commenting this might break something, test instructions
	// that reach this case. This was commented because MOVSX could not
	// utilize 0x67 and 0x66 prefix at the same time and it had to.
	// @Indirect16Override
    if(/*mode != DIRECT &&*/ ptr_size == 16)
    {
        // Operand-size override prefix is encoded using 66H
        *stream++ = 0x66;
    }
	else if(mode == DIRECT && register_get_bitsize(rm) == 16)
	{
		*stream++ = 0x66;
	}

    return stream;
}

/*
    8-bit general-purpose registers: AL, BL, CL, DL, SIL, DIL, SPL, BPL, and R8B-R15B are available using REX
    prefixes; AL, BL, CL, DL, AH, BH, CH, DH are available without using REX prefixes.
*/
static u8*
emit_rex(u8* stream, X64_Register reg, X64_Register rm, X64_Register index, X64_Register base, X64_AddrSize ptr_size, X64_Addressing_Mode mode, u32 flags)
{
    u8 b = 0, x = 0, r = 0, w = 0;
	w = (mode == DIRECT) ? register_get_bitsize(rm) == 64 : ptr_size == 64;
	if(flags & ADDRMODE_FLAG_NO_REXW)
		w = 0;
	if(flags & ADDRMODE_FLAG_REXW)
		w = 1;

    if(index == REGISTER_NONE && base == REGISTER_NONE)
    {
        // Memory Addressing Without an SIB Byte; REX.X Not Used
        // Register-Register Addressing (No Memory Operand); REX.X Not Used
        r = register_is_extended(reg);
        b = register_is_extended(rm);
    }
    else
    {
        // Memory Addressing With a SIB Byte
        r = register_is_extended(reg);
        b = register_is_extended(base);
        x = register_is_extended(index);
    }

    if(r != 0 || x != 0 || b != 0 || w != 0 || (reg >= SPL && reg <= DIL) || (rm >= SPL && rm <= DIL))
    {
        *stream++ = make_rex(b, x, r, w);
    }

    return stream;
}

// ----------------------------------

static X64_AddrMode
mk_base(X64_Addressing_Mode mode, X64_AddrMode_Type type)
{
	return (X64_AddrMode)
	{
		.mode_type   = type,
		.addr_mode   = mode,
		.sib_mode    = MODE_NONE,
		.sib_base    = REGISTER_NONE,
		.sib_index   = REGISTER_NONE,
		.rm          = REGISTER_NONE,
		.reg         = REGISTER_NONE,
		.moffs_base  = REGISTER_NONE,
		.ptr_bitsize = 0,
		.flags       = 0,
	};
}

static X64_AddrMode
mk_m_direct(X64_Register rm)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_M);
	result.rm = rm;
	return result;
}

static X64_AddrMode
mk_m_indirect(X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize)
{
	// TODO(psv): put this somewhere else
	//assert(rm >= RAX && rm <= R15D);

	X64_AddrMode result = mk_base(INDIRECT, ADDR_MODE_M);
	result.rm = rm;
	result.displacement = displacement;
	result.displacement_bitsize = value_bitsize(displacement);
	result.ptr_bitsize = ptr_bitsize;

	if(displacement > 0xff)
	{	
		result.addr_mode = INDIRECT_DWORD_DISPLACED;
	}
	else if(displacement > 0)
	{
		result.addr_mode = INDIRECT_BYTE_DISPLACED;	
	}

	if(register_equivalent(rm, RSP))
	{
		// need sib byte
		result.sib_mode = SIB_X1;
		result.sib_base = REGISTER_NONE;
		result.sib_base = rm;
		result.sib_index = RSP;
		result.rm = (register_get_bitsize(rm) == 32) ? ESP : RSP;	// needed, for representing no register
	}
	else if(result.addr_mode == INDIRECT && register_equivalent(rm, RBP))
	{
		result.addr_mode = INDIRECT_BYTE_DISPLACED;
		result.displacement_bitsize = 8;
	}
	return result;
}

static X64_AddrMode
mk_m_indirect_sib(X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	assert(rm >= RAX && rm <= R15D);

	if(sib_mode == SIB_X1 && register_equivalent(RBP, rm))
	{
		// TODO(psv): assert that the registers are the same size
		X64_Register temp = index;
		index = rm;
		rm = temp;
	}

	X64_AddrMode result = mk_base(INDIRECT, ADDR_MODE_M);
	result.reg = REGISTER_NONE;
	result.rm = (register_get_bitsize(rm) == 32) ? ESP : RSP;	// needed, for representing no register
	result.displacement = displacement;
	result.displacement_bitsize = value_bitsize(displacement);
	result.ptr_bitsize = ptr_bitsize;
	result.sib_base = rm;
	result.sib_index = index;
	result.sib_mode = sib_mode;

	if(displacement > 0xff)
	{	
		result.addr_mode = INDIRECT_DWORD_DISPLACED;
	}
	else if(displacement > 0)
	{
		result.addr_mode = INDIRECT_BYTE_DISPLACED;	
	}

	if(result.addr_mode == INDIRECT && register_equivalent(rm, RBP))
	{
		result.addr_mode = INDIRECT_BYTE_DISPLACED;
		result.displacement_bitsize = 8;
	}

	return result;
}

static X64_AddrMode
mk_rm_direct(X64_Register reg, X64_Register rm)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_RM);
	result.reg = reg;
	result.rm = rm;
	return result;
}

static X64_AddrMode
mk_mr_direct(X64_Register rm, X64_Register reg)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_MR);
	result.reg = reg;
	result.rm = rm;
	return result;
}

static X64_AddrMode
mk_rmi_direct(X64_Register reg, X64_Register rm, u32 immediate, s32 immediate_bitsize)
{
	assert(immediate_bitsize <= 32);
	assert(immediate_bitsize == 0 || value_bitsize(immediate) <= immediate_bitsize);
	assert(register_get_bitsize(reg) > 8);

	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_RMI);
	result.reg = reg;
	result.rm = rm;
	result.immediate = immediate;
	result.immediate_bitsize = immediate_bitsize;
	if(immediate_bitsize == 0)
		result.immediate_bitsize = value_bitsize(immediate);

	return result;
}

static X64_AddrMode
mk_rmi_indirect(X64_Register reg, X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize, u32 immediate, s32 immediate_bitsize)
{
	assert(immediate_bitsize <= 32);
	assert(immediate_bitsize == 0 || value_bitsize(immediate) <= immediate_bitsize);
	assert(register_get_bitsize(reg) > 8);

	X64_AddrMode result = mk_m_indirect(rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_RMI;
	result.reg = reg;
	result.immediate = immediate;
	result.immediate_bitsize = immediate_bitsize;
	if(immediate_bitsize == 0)
		result.immediate_bitsize = value_bitsize(immediate);

	return result;
}

// TODO(psv): RSP is not valid for sib don't allow it
static X64_AddrMode
mk_rmi_indirect_sib(X64_Register reg, X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize, u32 immediate, s32 immediate_bitsize)
{
	assert(immediate_bitsize <= 32);
	assert(immediate_bitsize == 0 || value_bitsize(immediate) <= immediate_bitsize);
	assert(register_get_bitsize(reg) > 8);

	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_RMI;
	result.immediate = immediate;
	result.immediate_bitsize = immediate_bitsize;
	result.reg = reg;

	return result;
}

static X64_AddrMode
mk_mri_direct(X64_Register rm, X64_Register reg, u8 imm8)
{
	X64_AddrMode result = mk_mr_direct(rm, reg);
	result.mode_type = ADDR_MODE_MRI;
	result.immediate_bitsize = 8;
	result.immediate = imm8;
	return result;
}

static X64_AddrMode
mk_mrc_direct(X64_Register rm, X64_Register reg)
{
	X64_AddrMode result = mk_mr_direct(rm, reg);
	result.mode_type = ADDR_MODE_MRC;
	return result;
}

static X64_AddrMode
mk_a_direct(X64_XMM_Register reg, X64_XMM_Register rm)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_A);
	result.reg = reg;
	result.rm = rm;
	return result;
}

static X64_AddrMode
mk_a_indirect(X64_XMM_Register reg, X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect(rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_A;
	result.reg = reg;
	return result;
}

static X64_AddrMode
mk_a_indirect_sib(X64_XMM_Register reg, X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_A;
	result.reg = reg;

	return result;
}

static X64_AddrMode
mk_b_direct(X64_XMM_Register reg, X64_XMM_Register rm)
{
	X64_AddrMode result = mk_a_direct(reg, rm);
	result.mode_type = ADDR_MODE_B;
	return result;
}
static X64_AddrMode
mk_b_indirect(X64_XMM_Register reg, X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_a_indirect(reg, rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_B;
	return result;
}

static X64_AddrMode
mk_b_indirect_sib(X64_XMM_Register reg, X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_a_indirect_sib(reg, rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_B;
	return result;
}

static X64_AddrMode
mk_zo()
{
	return mk_base(DIRECT, ADDR_MODE_ZO);
}

static X64_AddrMode
mk_zo_reg(X64_Register reg)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_ZO);
	result.reg = reg;
	return result;
}

static X64_AddrMode
mk_zo_reg_bitsize(X64_Register reg, s32 bitsize)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_ZO);
	result.reg = reg;
	result.ptr_bitsize = bitsize;
	return result;
}

static X64_AddrMode
mk_zo_bitsize(s32 bitsize)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_ZO);
	result.ptr_bitsize = bitsize;
	return result;
}

static X64_AddrMode
mk_o(X64_Register reg)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_O);
	result.reg = reg;
	return result;
}

static X64_AddrMode
mk_rm_indirect(X64_Register reg, X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect(rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_RM;
	result.reg = reg;

	return result;
}

static X64_AddrMode
mk_mr_indirect(X64_Register rm, X64_Register reg, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect(rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MR;
	result.reg = reg;

	return result;
}

static X64_AddrMode
mk_mri_indirect(X64_Register rm, X64_Register reg, u32 displacement, X64_AddrSize ptr_bitsize, u8 imm8)
{
	X64_AddrMode result = mk_mr_indirect(rm, reg, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MRI;
	result.immediate_bitsize = 8;
	result.immediate = imm8;
	return result;
}

static X64_AddrMode
mk_mrc_indirect(X64_Register rm, X64_Register reg, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_mr_indirect(rm, reg, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MRC;
	return result;
}

static X64_AddrMode
mk_rm_indirect_sib(X64_Register reg, X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_RM;
	result.reg = reg;

	return result;
}

static X64_AddrMode
mk_mr_indirect_sib(X64_Register rm, X64_Register reg, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MR;
	result.reg = reg;

	return result;
}

static X64_AddrMode
mk_mri_indirect_sib(X64_Register rm, X64_Register reg, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize, u8 imm8)
{
	X64_AddrMode result = mk_mr_indirect_sib(rm, reg, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MRI;
	result.immediate_bitsize = 8;
	result.immediate = imm8;

	return result;
}

static X64_AddrMode
mk_mrc_indirect_sib(X64_Register rm, X64_Register reg, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_mr_indirect_sib(rm, reg, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MRC;

	return result;
}

static X64_AddrMode
mk_mi_direct(X64_Register rm, u32 immediate, s32 immediate_bitsize)
{
	assert(immediate_bitsize <= 32);
	assert(immediate_bitsize == 0 || value_bitsize(immediate) <= immediate_bitsize);

	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_MI);
	result.reg = REGISTER_NONE;
	result.rm = rm;
	result.immediate = immediate;
	result.immediate_bitsize = (immediate_bitsize == 0) ? MAX(8, value_bitsize(immediate)) : immediate_bitsize;

	return result;
}

static X64_AddrMode
mk_mi_indirect(X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize, u32 immediate, s32 immediate_bitsize)
{
	assert((rm >= RAX && rm <= R15D) || rm == REGISTER_NONE);

	X64_AddrMode result = mk_base(INDIRECT, ADDR_MODE_MI);
	result.rm = rm;
	result.displacement = displacement;
	result.displacement_bitsize = value_bitsize(displacement);
	result.ptr_bitsize = ptr_bitsize;
	result.immediate = immediate;
	result.immediate_bitsize = (immediate_bitsize == 0) ? MAX(8, value_bitsize(immediate)) : immediate_bitsize;

	if(register_equivalent(rm, REGISTER_NONE))
	{
		result.addr_mode = INDIRECT;
		result.rm = RBP;
		result.ptr_bitsize = ptr_bitsize;
		result.displacement_bitsize = 32;
	}
	else if(displacement > 0xff)
	{	
		result.addr_mode = INDIRECT_DWORD_DISPLACED;
	}
	else if(displacement > 0)
	{
		result.addr_mode = INDIRECT_BYTE_DISPLACED;	
	}

	if(register_equivalent(rm, RSP))
	{
		// need sib byte
		result.sib_mode = SIB_X1;
		result.sib_base = REGISTER_NONE;
		result.sib_base = rm;
		result.sib_index = RSP;
		result.rm = (register_get_bitsize(rm) == 32) ? ESP : RSP;	// needed, for representing no register
	}
	else if(result.addr_mode == INDIRECT && register_equivalent(rm, RBP))
	{
		result.addr_mode = INDIRECT_BYTE_DISPLACED;
		result.displacement_bitsize = 8;
	}
	return result;
}

static X64_AddrMode
mk_mi_indirect_sib(X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize, u32 immediate, s32 immediate_bitsize)
{
	assert(immediate_bitsize <= 32);
	assert(immediate_bitsize == 0 || value_bitsize(immediate) <= immediate_bitsize);

	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MI;
	result.immediate = immediate;
	result.immediate_bitsize = (immediate_bitsize == 0) ? MAX(8, value_bitsize(immediate)) : immediate_bitsize;
	if(result.sib_index == REGISTER_NONE) result.sib_index = RSP;
	if(result.sib_base == REGISTER_NONE) result.sib_base = RSP;

	return result;
}

static X64_AddrMode
mk_oi(X64_Register rm, u64 immediate, s32 immediate_bitsize)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_OI);
	result.rm = rm;
	result.reg = REGISTER_NONE;
	result.immediate = immediate;
	result.immediate_bitsize = (immediate_bitsize == 0) ? MAX(8, value_bitsize(immediate)) : immediate_bitsize;
	return result;
}

static X64_AddrMode
mk_i(u32 immediate, s32 immediate_bitsize)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_I);
	result.immediate = immediate;
	result.immediate_bitsize = (immediate_bitsize == 0) ? MAX(8, value_bitsize(immediate)) : immediate_bitsize;
	return result;
}

static X64_AddrMode
mk_i_reg(X64_Register rm, u32 immediate, s32 immediate_bitsize)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_I);
	result.rm = rm;
	result.reg = REGISTER_NONE;
	result.immediate = immediate;
	result.immediate_bitsize = (immediate_bitsize == 0) ? MAX(8, value_bitsize(immediate)) : immediate_bitsize;
	return result;
}

static X64_AddrMode
mk_fd(X64_Register base, u64 offset, X64_AddrSize bitsize)
{
	assert(register_is_segment(base) || base == REGISTER_NONE);
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_FD);
	result.moffs_base = base;
	result.immediate_bitsize = 64;
	result.immediate = offset;

	switch(bitsize)
	{
		case 64: result.rm = RAX; break;
		case 32: result.rm = EAX; break;
		case 16: result.rm = AX; break;
		case 8 : result.rm = AL; break;
		default: assert(0); break;
	}

	return result;
}

static X64_AddrMode
mk_td(X64_Register base, u64 offset, X64_AddrSize bitsize)
{
	assert(register_is_segment(base) || base == REGISTER_NONE);
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_TD);
	result.moffs_base = base;
	result.immediate_bitsize = 64;
	result.immediate = offset;

	switch(bitsize)
	{
		case 64: result.rm = RAX; break;
		case 32: result.rm = EAX; break;
		case 16: result.rm = AX; break;
		case 8 : result.rm = AL; break;
		default: assert(0); break;
	}
	
	return result;
}

static X64_AddrMode
mk_m1_direct(X64_Register rm)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_M1);
	result.rm = rm;
	return result;
}

static X64_AddrMode
mk_m1_indirect(X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect(rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_M1;
	return result;
}

static X64_AddrMode
mk_m1_indirect_sib(X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_M1;
	return result;
}

static X64_AddrMode
mk_mc_direct(X64_Register rm)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_MC);
	result.rm = rm;
	return result;
}

static X64_AddrMode
mk_mc_indirect(X64_Register rm, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect(rm, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MC;
	return result;
}

static X64_AddrMode
mk_mc_indirect_sib(X64_Register rm, X64_Register index, X64_SibMode sib_mode, u32 displacement, X64_AddrSize ptr_bitsize)
{
	X64_AddrMode result = mk_m_indirect_sib(rm, index, sib_mode, displacement, ptr_bitsize);
	result.mode_type = ADDR_MODE_MC;
	return result;
}

static X64_AddrMode
mk_d(u32 relative)
{
	X64_AddrMode result = mk_base(DIRECT, ADDR_MODE_D);
	result.immediate = relative;
	result.immediate_bitsize = MAX(8, value_bitsize(relative));
	return result;
}

u8* emit_mul(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_div(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_idiv(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_imul(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_neg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_nop(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_not(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_dec(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_iec(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_mov(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movsx(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movsxd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movzx(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cmovcc(Instr_Emit_Result* out_info, u8* stream, X64_CMOVcc_Instruction instr, X64_AddrMode amode);
u8* emit_setcc(Instr_Emit_Result* out_info, u8* stream, X64_SETcc_Instruction instr, X64_AddrMode amode);
u8* emit_mov_debug_reg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_mov_control_reg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_jcc(Instr_Emit_Result* out_info, u8* stream, X64_Jump_Conditional_Short condition, s32 rel, s32 rel_bitsize);
u8* emit_jecxz(Instr_Emit_Result* out_info, u8* stream, s8 rel);
u8* emit_jrcxz(Instr_Emit_Result* out_info, u8* stream, s8 rel);
u8* emit_jmp(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_fjmp(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_call(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_fcall(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_loopcc(Instr_Emit_Result* out_info, u8* stream, X64_Loop_Short instr, s8 rel);
u8* emit_lgdt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_lidt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_ud(Instr_Emit_Result* out_info, u8* stream, X64_UD_Instruction instr, X64_AddrMode amode);

u8* emit_ret(Instr_Emit_Result* out_info, u8* stream, X64_Ret_Instruction ret, u16 imm);
u8* emit_push(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_pop(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_enter(Instr_Emit_Result* out_info, u8* stream, u16 storage_size, u8 lex_nest_level, bool b16);
u8* emit_in(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_out(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_int3(Instr_Emit_Result* out_info, u8* stream);
u8* emit_int0(Instr_Emit_Result* out_info, u8* stream);
u8* emit_int1(Instr_Emit_Result* out_info, u8* stream);
u8* emit_int(Instr_Emit_Result* out_info, u8* stream, u8 rel);

u8* emit_leave(Instr_Emit_Result* out_info, u8* stream);
u8* emit_leave16(Instr_Emit_Result* out_info, u8* stream);

u8* emit_instruction(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_Opcode opcode);
u8* emit_instruction_prefixed(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_Opcode opcode, u8 prefix);
u8* emit_arithmetic(Instr_Emit_Result* out_info, u8* stream, X64_Arithmetic_Instr instr, X64_AddrMode amode);
u8* emit_shift(Instr_Emit_Result* out_info, u8* stream, X64_Shift_Instruction instr_digit, X64_AddrMode amode);
u8* emit_lea(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_test(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_shld(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_shrd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_bsf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_bsr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_bswap(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_bt(Instr_Emit_Result* out_info, u8* stream, X64_BitTest_Instr instr, X64_AddrMode amode);
u8* emit_cbw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cwd(Instr_Emit_Result* out_info, u8* stream);
u8* emit_cdq(Instr_Emit_Result* out_info, u8* stream);
u8* emit_cqo(Instr_Emit_Result* out_info, u8* stream);
u8* emit_clc(Instr_Emit_Result* out_info, u8* stream);
u8* emit_cld(Instr_Emit_Result* out_info, u8* stream);
u8* emit_cli(Instr_Emit_Result* out_info, u8* stream);
u8* emit_clts(Instr_Emit_Result* out_info, u8* stream);
u8* emit_stc(Instr_Emit_Result* out_info, u8* stream);
u8* emit_std(Instr_Emit_Result* out_info, u8* stream);
u8* emit_sti(Instr_Emit_Result* out_info, u8* stream);
u8* emit_cmc(Instr_Emit_Result* out_info, u8* stream);
u8* emit_cpuid(Instr_Emit_Result* out_info, u8* stream);
u8* emit_hlt(Instr_Emit_Result* out_info, u8* stream);
u8* emit_invd(Instr_Emit_Result* out_info, u8* stream);
u8* emit_iret(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_lahf(Instr_Emit_Result* out_info, u8* stream);
u8* emit_sahf(Instr_Emit_Result* out_info, u8* stream);
u8* emit_popf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_pushf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_rdmsr(Instr_Emit_Result* out_info, u8* stream);
u8* emit_rdpmc(Instr_Emit_Result* out_info, u8* stream);
u8* emit_rdtsc(Instr_Emit_Result* out_info, u8* stream);
u8* emit_rsm(Instr_Emit_Result* out_info, u8* stream);
u8* emit_sysenter(Instr_Emit_Result* out_info, u8* stream);
u8* emit_sysexit(Instr_Emit_Result* out_info, u8* stream, bool b64);
u8* emit_wbinvd(Instr_Emit_Result* out_info, u8* stream);
u8* emit_wrmsr(Instr_Emit_Result* out_info, u8* stream);
u8* emit_xlat(Instr_Emit_Result* out_info, u8* stream, bool b64);
u8* emit_lldt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_lmsw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_ltr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_str(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_sgdt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_sidt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_sldt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_invplg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_smsw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_verr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_verw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_xadd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_xchg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_lar(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_lsl(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_addps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_addss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_addpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_addsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_subps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_subss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_subpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_subsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_andps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_andpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_andnotps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_andnotpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_orps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_orpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_xorps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_xorpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_cmpps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate);
u8* emit_cmppd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate);
u8* emit_cmpss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate);
u8* emit_cmpsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate);

u8* emit_comiss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_comisd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_ucomiss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_ucomisd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_cvtpi2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtpi2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtps2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtpd2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtsi2ss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtss2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtss2sd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtsd2ss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtdq2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtdq2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtpd2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtpd2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtps2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtps2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtsd2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvtsi2sd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_cvttpd2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvttpd2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvttps2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvttsd2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvttss2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_cvttps2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_divpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_divsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_divps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_divss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_mulpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_mulps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_mulss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_mulsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_movaps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movapd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movups(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movupd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movhps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movhpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movlpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_movlps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);

u8* emit_cmps(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);
u8* emit_cmpxchg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode);
u8* emit_ins(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);
u8* emit_outs(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);
u8* emit_scas(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);
u8* emit_stos(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);
u8* emit_lods(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);
u8* emit_movs(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize);

#ifdef HO_ASSEMBLER_IMPLEMENT 
u8*
emit_instruction(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_Opcode opcode)
{
    u8* start = stream;    

    // Prefix size override
    if(!(amode.flags & ADDRMODE_FLAG_NO_SIZE_OVERRIDE))
        stream = emit_size_override(stream, amode.ptr_bitsize, amode.rm, amode.addr_mode);

    // REX prefix
    if(!(amode.flags & ADDRMODE_FLAG_NO_REX))
        stream = emit_rex(stream, amode.reg, amode.rm, amode.sib_index, amode.sib_base, amode.ptr_bitsize, amode.addr_mode, amode.flags);

    // Opcode
    for(int i = 0; i < opcode.byte_count; ++i) *stream++ = opcode.bytes[i];

    // Mod/RM
    if(amode.rm != REGISTER_NONE && amode.reg != REGISTER_NONE)
        *stream++ = make_modrm(amode.addr_mode, register_representation(amode.reg), register_representation(amode.rm));

    // SIB
    if(amode.sib_mode != MODE_NONE)
        *stream++ = make_sib(amode.sib_mode, register_representation(amode.sib_index), register_representation(amode.sib_base));

    // Displacement
    s8 disp_offset = stream - start;
    stream = emit_value_raw(stream, amode.displacement, amode.displacement_bitsize);
    disp_offset = (disp_offset == (stream - start)) ? -1 : disp_offset;

    // Immediate
    s8 imm_offset = stream - start;
    stream = emit_value_raw(stream, amode.immediate, amode.immediate_bitsize);
    imm_offset = (imm_offset == (stream - start)) ? -1 : imm_offset;

    fill_outinfo(out_info, stream - start, disp_offset, imm_offset);
    
    return stream;
}

u8*
emit_instruction_prefixed(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_Opcode opcode, u8 prefix)
{
    u8* start = stream;    

    // Prefix size override
    *stream++ = prefix;

    // REX prefix
    if(!(amode.flags & ADDRMODE_FLAG_NO_REX))
        stream = emit_rex(stream, amode.reg, amode.rm, amode.sib_index, amode.sib_base, amode.ptr_bitsize, amode.addr_mode, amode.flags);

    // Opcode
    for(int i = 0; i < opcode.byte_count; ++i) *stream++ = opcode.bytes[i];

    // Mod/RM
    if(amode.rm != REGISTER_NONE && amode.reg != REGISTER_NONE)
        *stream++ = make_modrm(amode.addr_mode, register_representation(amode.reg), register_representation(amode.rm));

    // SIB
    if(amode.sib_mode != MODE_NONE)
        *stream++ = make_sib(amode.sib_mode, register_representation(amode.sib_index), register_representation(amode.sib_base));

    // Displacement
    s8 disp_offset = stream - start;
    stream = emit_value_raw(stream, amode.displacement, amode.displacement_bitsize);
    disp_offset = (disp_offset == (stream - start)) ? -1 : disp_offset;

    // Immediate
    s8 imm_offset = stream - start;
    stream = emit_value_raw(stream, amode.immediate, amode.immediate_bitsize);
    imm_offset = (imm_offset == (stream - start)) ? -1 : imm_offset;

    fill_outinfo(out_info, stream - start, disp_offset, imm_offset);
    
    return stream;
} 
#define AND_IMM8 0x80
#define AND_IMM 0x81
#define ADD_IMM8 0x80
#define ADD_IMM 0x81
#define ADDS_IMM8 0x83
#define ADD_MR 0x01
#define ADD_RM 0x03
#define ADD_AL 0x04
#define ADD_A 0x05
#define AND_AL 0x24
#define AND_A 0x25
#define OR_AL 0xc
#define OR_A 0xd
#define ADC_AL 0x14
#define ADC_A 0x15
#define SBB_AL 0x1c
#define SBB_A 0x1d
#define SUB_AL 0x2c
#define SUB_A 0x2d
#define XOR_AL 0x34
#define XOR_A 0x35
#define CMP_AL 0x3c
#define CMP_A 0x3d

#define ADD_MR8 0x00
#define CMP_MR8 0x38
#define AND_MR8 0x20
#define ADC_MR8 0x10
#define SBB_MR8 0x18
#define SUB_MR8 0x28
#define XOR_MR8 0x30
#define OR_MR8 0x08

#define ADD_RM8 0x02
#define CMP_RM8 0x3A
#define AND_RM8 0x22
#define ADC_RM8 0x12
#define SBB_RM8 0x1A
#define SUB_RM8 0x2A
#define XOR_RM8 0x32
#define OR_RM8 0x0A

static u8
mi_opcode_a(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_AL; break;
        case ARITH_OR:  opcode = OR_AL; break;
        case ARITH_XOR: opcode = XOR_AL; break;
        case ARITH_ADD: opcode = ADD_AL; break;
        case ARITH_ADC: opcode = ADC_AL; break;
        case ARITH_SUB: opcode = SUB_AL; break;
        case ARITH_SBB: opcode = SBB_AL; break;
        case ARITH_CMP: opcode = CMP_AL; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

static u8
mr_opcode(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_MR8; break;
        case ARITH_OR:  opcode = OR_MR8; break;
        case ARITH_XOR: opcode = XOR_MR8; break;
        case ARITH_ADD: opcode = ADD_MR8; break;
        case ARITH_ADC: opcode = ADC_MR8; break;
        case ARITH_SUB: opcode = SUB_MR8; break;
        case ARITH_SBB: opcode = SBB_MR8; break;
        case ARITH_CMP: opcode = CMP_MR8; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

static u8
rm_opcode(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_RM8; break;
        case ARITH_OR:  opcode = OR_RM8; break;
        case ARITH_XOR: opcode = XOR_RM8; break;
        case ARITH_ADD: opcode = ADD_RM8; break;
        case ARITH_ADC: opcode = ADC_RM8; break;
        case ARITH_SUB: opcode = SUB_RM8; break;
        case ARITH_SBB: opcode = SBB_RM8; break;
        case ARITH_CMP: opcode = CMP_RM8; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

u8*
emit_arithmetic(Instr_Emit_Result* out_info, u8* stream, X64_Arithmetic_Instr instr, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.immediate_bitsize > 0)
    {
        u8 op = 0x80; // imm8
        if(bitsize > 8) op += 1;    // 0x81
        if(amode.immediate_bitsize == 8 && bitsize > 8) op += 2;   // 0x83
        opcode.bytes[0] = op;

        if(amode.addr_mode == DIRECT && amode.immediate_bitsize == 16 && register_get_bitsize(amode.rm) > 16)
            amode.immediate_bitsize = 32;
        else if(amode.addr_mode != DIRECT && amode.immediate_bitsize == 16 && amode.ptr_bitsize > 16)
            amode.immediate_bitsize = 32;
        amode.reg = instr;
    }
    else if(amode.mode_type == ADDR_MODE_RM)
    {
        opcode.bytes[0] = rm_opcode(instr, bitsize);
    }
    else if(amode.mode_type == ADDR_MODE_MR)
    {
        opcode.bytes[0] = mr_opcode(instr, bitsize);
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lea(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = 0x8d;
    assert(register_get_bitsize(amode.reg) > 8);
    assert(amode.addr_mode != DIRECT);
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_xadd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_MR);
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (bitsize == 8) ? 0xc0 : 0xc1;

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_xchg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.mode_type == ADDR_MODE_O)
    {    
        assert(register_get_bitsize(amode.reg) > 8);
        u8* start = stream;
        if(amode.addr_mode == DIRECT) amode.rm = amode.reg;
        // size override
        stream = emit_size_override(stream, amode.ptr_bitsize, amode.rm, amode.addr_mode);

        // rex
        if(register_is_extended(amode.reg) || register_get_bitsize(amode.reg) == 64)
            *stream++ = make_rex(register_is_extended(amode.reg), 0, 0, register_get_bitsize(amode.reg) == 64);

        // opcode
        *stream++ = 0x90 | (0x7 & amode.reg);
        
        fill_outinfo(out_info, (s8)(stream - start), -1, -1);
        return stream;
    }
    else if(amode.mode_type == ADDR_MODE_RM || amode.mode_type == ADDR_MODE_MR)
    {
        opcode.bytes[0] = (register_get_bitsize(amode.reg) == 8) ? 0x86 : 0x87;
    }
    return emit_instruction(out_info, stream, amode, opcode);
} 
u8*
emit_bsf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(bitsize > 8);

    X64_Opcode opcode = {0};
    switch(amode.mode_type)
    {
        case ADDR_MODE_RM: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = 0xbc;
        } break;
        default: assert(0); break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_bsr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(bitsize > 8);

    X64_Opcode opcode = {0};
    switch(amode.mode_type)
    {
        case ADDR_MODE_RM: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = 0xbd;
        } break;
        default: assert(0); break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_bswap(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_O);
    
    u8* start = stream;
    if(register_is_extended(amode.reg) || register_get_bitsize(amode.reg) == 64)
        *stream++ = make_rex(register_is_extended(amode.reg), 0, 0, register_get_bitsize(amode.reg) == 64);

    // opcode
    *stream++ = 0x0f;
    *stream++ = 0xc8 | (0x7 & amode.reg);
    
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

static u8 bittest_opcode_mr(X64_BitTest_Instr instr)
{
    switch(instr)
    {
        case BITTEST:               return 0xa3;
        case BITTEST_COMPLEMENT:    return 0xbb;
        case BITTEST_RESET:         return 0xb3;
        case BITTEST_SET:           return 0xab;
        default: assert(0); return 0;
    }
}

u8*
emit_bt(Instr_Emit_Result* out_info, u8* stream, X64_BitTest_Instr instr, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(bitsize > 8);

    X64_Opcode opcode = {0};
    switch(amode.mode_type)
    {
        case ADDR_MODE_MR: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = bittest_opcode_mr(instr);
        } break;
        case ADDR_MODE_MI: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = 0xba;
            amode.reg = instr;
            assert(amode.immediate_bitsize == 8);
        } break;
        default: assert(0); break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
} 
u8*
emit_cbw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;
    assert(register_equivalent(amode.reg, RAX));
    switch(amode.reg)
    {
        case AX: break;
        case AL: {
            stream = emit_size_override(stream, 16, AX, DIRECT);
        } break;
        case EAX: {
            *stream++ = make_rex(0, 0, 0, 1);
        } break;
        default: assert(0);
    }
    *stream++ = 0x98;
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

u8*
emit_cwd(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x66;
    *stream++ = 0x99;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_cdq(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x99;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_cqo(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = make_rex(0, 0, 0, 1);
    *stream++ = 0x99;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_clc(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xf8;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_cld(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xfc;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_cli(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xfa;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_clts(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x06;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_stc(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xf9;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_std(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xfd;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_sti(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xfb;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_cmc(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xf5;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_cpuid(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0xa2;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_hlt(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0xf4;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_invd(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x08;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_iret(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;

    switch(amode.ptr_bitsize)
    {
        case 16: {
            stream = emit_size_override(stream, 16, AX, DIRECT);
        } break;
        case 32: break;
        case 64: {
            *stream++ = make_rex(0, 0, 0, 1);
        } break;
        default: assert(0);
    }
    *stream++ = 0xcf;
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

u8*
emit_lahf(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x9f;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_sahf(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x9e;
    fill_outinfo(out_info, 1, -1, -1);
    return stream;
}

u8*
emit_popf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;

    switch(amode.ptr_bitsize)
    {
        case 16: {
            stream = emit_size_override(stream, 16, AX, DIRECT);
        } break;
        case 64: break;
        default: assert(0);
    }
    *stream++ = 0x9d;
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

u8*
emit_pushf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;

    switch(amode.ptr_bitsize)
    {
        case 16: {
            stream = emit_size_override(stream, 16, AX, DIRECT);
        } break;
        case 64: break;
        default: assert(0);
    }
    *stream++ = 0x9c;
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

u8*
emit_rdmsr(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x32;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_rdpmc(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x33;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_rdtsc(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x31;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_rsm(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0xaa;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_sysenter(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x34;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_sysexit(Instr_Emit_Result* out_info, u8* stream, bool b64)
{
    if(b64) *stream++ = make_rex(0, 0, 0, 1);
    *stream++ = 0x0f;
    *stream++ = 0x35;
    fill_outinfo(out_info, (b64) ? 3 : 2, -1, -1);
    return stream;
}

u8*
emit_wbinvd(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x09;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_wrmsr(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x0f;
    *stream++ = 0x30;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_xlat(Instr_Emit_Result* out_info, u8* stream, bool b64)
{
    if(b64) *stream++ = make_rex(0, 0, 0, 1);
    *stream++ = 0xd7;
    fill_outinfo(out_info, (b64) ? 2 : 1, -1, -1);
    return stream;
}

#define LLDT_DIGIT 2
#define LLSW_DIGIT 6
#define LTR_DIGIT 3
#define STR_DIGIT 1
#define SGDT_DIGIT 0
#define SIDT_DIGIT 1
#define SLDT_DIGIT 0

static u8*
emit_l(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, u8 digit, u8 op)
{
    assert((amode.addr_mode == DIRECT && register_get_bitsize(amode.rm) == 16) || (amode.addr_mode != DIRECT && register_get_bitsize(amode.rm) >= 32));
    bool extended = register_is_extended(amode.rm);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = op;
    amode.ptr_bitsize = 32; // ignore 16 bit size override
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.reg = digit;
    if(amode.addr_mode == DIRECT)
        amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lldt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_l(out_info, stream, amode, LLDT_DIGIT, 0);
}

u8*
emit_lmsw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_l(out_info, stream, amode, LLSW_DIGIT, 1);
}

u8*
emit_ltr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_l(out_info, stream, amode, LTR_DIGIT, 0);
}

u8*
emit_str(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_l(out_info, stream, amode, STR_DIGIT, 0);
}

u8*
emit_sgdt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_l(out_info, stream, amode, SGDT_DIGIT, 1);
}

u8*
emit_sidt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_l(out_info, stream, amode, SIDT_DIGIT, 1);
}

u8*
emit_sldt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_l(out_info, stream, amode, SLDT_DIGIT, 0);
}

#define INVPLG_DIGIT 7
// DIRECT mode for this instruction produces the instruction SWAPGS
u8*
emit_invplg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_M);
    assert(amode.addr_mode != DIRECT);
    assert(amode.ptr_bitsize == 8);

    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x01;
    amode.reg = INVPLG_DIGIT;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_instruction(out_info, stream, amode, opcode);
}

#define SMSW_DIGIT 4
u8*
emit_smsw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_M);
    assert(amode.ptr_bitsize == 16 || amode.addr_mode == DIRECT);
    if(amode.addr_mode != DIRECT)
        amode.ptr_bitsize = 32; // ignore 16 bit override prefix

    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x01;
    amode.reg = SMSW_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

static u8*
emit_ver(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, u8 digit)
{
    assert(amode.mode_type == ADDR_MODE_M);

    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x00;
    amode.reg = digit;
    amode.flags |= (ADDRMODE_FLAG_NO_REXW|((amode.addr_mode == DIRECT) ? ADDRMODE_FLAG_NO_SIZE_OVERRIDE: 0));
    if(amode.addr_mode != DIRECT)
        amode.ptr_bitsize = 64; // this is to ignore 16bit size override
    return emit_instruction(out_info, stream, amode, opcode);
}

#define VERR_DIGIT 4
#define VERW_DIGIT 5
u8*
emit_verr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_ver(out_info, stream, amode, VERR_DIGIT);
}

u8*
emit_verw(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_ver(out_info, stream, amode, VERW_DIGIT);
} 
u8*
emit_jcc_prefix(u8* stream, s32 rel_bitsize)
{
    if(rel_bitsize == 16)
        *stream++ = 0x66;
    if(rel_bitsize > 8)
        *stream++ = 0x0f;
    return stream;
}

u8*
emit_jcc(Instr_Emit_Result* out_info, u8* stream, X64_Jump_Conditional_Short condition, s32 rel, s32 rel_bitsize)
{
    assert(rel_bitsize == 8 || rel_bitsize == 16 || rel_bitsize == 32);

    u8* start = stream;
    s8 imm_offset = -1;

    stream = emit_jcc_prefix(stream, rel_bitsize);
    *stream++ = (u8)(condition + ((rel_bitsize > 8) ? 0x10 : 0));
    imm_offset = (s8)(stream - start);
    
    // Takes into account the size of the current instruction
    if((s32)rel <= 0)
        rel -= (rel_bitsize / 8 + (stream - start));
    else
        rel += (rel_bitsize / 8 + (stream - start));

    stream = emit_value_raw(stream, rel, rel_bitsize);

    fill_outinfo(out_info, stream - start, -1, imm_offset);

    return stream;
}

u8*
emit_jecxz(Instr_Emit_Result* out_info, u8* stream, s8 rel)
{
    *stream++ = 0x67; // 32 bit size override prefix
    *stream++ = (u8)JECXZ;
    *stream++ = (u8)((rel <= 0) ? (rel - 2) : (rel + 2));
    fill_outinfo(out_info, 3, -1, 2);
    return stream;
}

u8*
emit_jrcxz(Instr_Emit_Result* out_info, u8* stream, s8 rel)
{
    *stream++ = (u8)JECXZ;
    // Takes into account the size of the current instruction
    *stream++ = (u8)((rel <= 0) ? (rel - 2) : (rel + 2));
    fill_outinfo(out_info, 2, -1, 1);
    return stream;
}

u8*
emit_loopcc(Instr_Emit_Result* out_info, u8* stream, X64_Loop_Short instr, s8 rel)
{
    *stream++ = (u8)instr;
    // Takes into account the size of the current instruction
    *stream++ = (u8)((rel <= 0) ? (rel - 2) : (rel + 2));
    fill_outinfo(out_info, 2, -1, 1);
    return stream;
}

/*
m16:16, m16:32 & m16:64 — A memory operand containing a far pointer composed of two numbers. The
number to the left of the colon corresponds to the pointer's segment selector. The number to the right
corresponds to its offset.
*/
#define JMP_M_DIGIT 4
#define JMP_M_SEG_DIGIT 5
u8*
emit_jmp(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    switch(amode.mode_type)
    {
        case ADDR_MODE_D: {
            assert(amode.immediate_bitsize == 8 || amode.immediate_bitsize == 32);
            if(amode.immediate_bitsize == 8)
            {
                const s8 instruction_size = 2;
                *stream++ = 0xeb;
                // Takes into account the size of the current instruction
                if((s8)amode.immediate <= 0)
                    *stream++ = (u8)(amode.immediate - instruction_size);
                else
                    *stream++ = (u8)(amode.immediate + instruction_size);
                fill_outinfo(out_info, 1 + sizeof(u8), -1, 1);
            }
            else
            {
                *stream++ = 0xe9;
                // Takes into account the size of the current instruction
                if((s32)amode.immediate <= 0)
                    *(u32*)stream = (u32)(amode.immediate - (1 + sizeof(u32)));
                else
                    *(u32*)stream = (u32)(amode.immediate + (1 + sizeof(u32)));
                stream += sizeof(u32);
                fill_outinfo(out_info, 1 + sizeof(u32), -1, 1);
            }
        } break;
        case ADDR_MODE_M: {
            s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;            
            assert(bitsize == 64);
            X64_Opcode opcode = {.byte_count = 1};
            opcode.bytes[0] = 0xff;
            amode.reg = JMP_M_DIGIT;
            if(bitsize == 64)
                amode.flags |= ADDRMODE_FLAG_NO_REXW;
            stream = emit_instruction(out_info, stream, amode, opcode);
        } break;
    }
    return stream;
}

u8*
emit_fjmp(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_M)
    {
        s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;            
        assert(bitsize == 64);
        X64_Opcode opcode = {.byte_count = 1};
        opcode.bytes[0] = 0xff;
        amode.reg = JMP_M_SEG_DIGIT;
        if(bitsize == 64)
            amode.flags |= ADDRMODE_FLAG_NO_REXW;
        stream = emit_instruction(out_info, stream, amode, opcode);
    }
    return stream;
}

#define CALL_M_DIGIT 2
#define CALL_M_SEG_DIGIT 3

u8*
emit_call(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    switch(amode.mode_type)
    {
        case ADDR_MODE_D: {
            assert(amode.immediate_bitsize == 32);
            *stream++ = 0xe8;
            // Takes into account the size of the current instruction
            if((s32)amode.immediate <= 0)
                *(u32*)stream = (u32)(amode.immediate - (1 + sizeof(u32)));
            else
                *(u32*)stream = (u32)(amode.immediate + (1 + sizeof(u32)));
            stream += sizeof(u32);
            fill_outinfo(out_info, 1 + sizeof(u32), -1, 1);
        } break;
        case ADDR_MODE_M: {
            s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;            
            assert(bitsize == 64);
            X64_Opcode opcode = {.byte_count = 1};
            opcode.bytes[0] = 0xff;
            amode.reg = CALL_M_DIGIT;
            if(bitsize == 64)
                amode.flags |= ADDRMODE_FLAG_NO_REXW;
            stream = emit_instruction(out_info, stream, amode, opcode);
        } break;
    }
    return stream;
}

u8*
emit_fcall(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_M)
    {
        s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;            
        assert(bitsize == 64);
        X64_Opcode opcode = {.byte_count = 1};
        opcode.bytes[0] = 0xff;
        amode.reg = CALL_M_SEG_DIGIT;
        if(bitsize == 64)
            amode.flags |= ADDRMODE_FLAG_NO_REXW;
        stream = emit_instruction(out_info, stream, amode, opcode);
    }
    return stream;
}

#define LGDT_DIGIT 2
u8*
emit_lgdt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_M)
    {
        s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;            
        assert(bitsize == 64);
        X64_Opcode opcode = {.byte_count = 2};
        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = 0x01;
        amode.reg = LGDT_DIGIT;
        if(bitsize == 64)
            amode.flags |= ADDRMODE_FLAG_NO_REXW;
        stream = emit_instruction(out_info, stream, amode, opcode);
    }
    return stream;
}

#define LIDT_DIGIT 3
u8*
emit_lidt(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_M)
    {
        s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;            
        assert(bitsize == 64);
        X64_Opcode opcode = {.byte_count = 2};
        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = 0x01;
        amode.reg = LIDT_DIGIT;
        if(bitsize == 64)
            amode.flags |= ADDRMODE_FLAG_NO_REXW;
        stream = emit_instruction(out_info, stream, amode, opcode);
    }
    return stream;
}

u8*
emit_ud(Instr_Emit_Result* out_info, u8* stream, X64_UD_Instruction instr, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_RM)
    {
        X64_Opcode opcode = {.byte_count = 2};
        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = (u8)instr;
        stream = emit_instruction(out_info, stream, amode, opcode);
    }
    else if(amode.mode_type == ADDR_MODE_ZO)
    {
        // UD2
        *stream++ = 0x0f;
        *stream++ = (u8)instr;
    }
    else
    {
        assert(0);
    }
    return stream;
} 
/*
• 2EH — CS segment override (use with any branch instruction is reserved).
• 36H — SS segment override prefix (use with any branch instruction is reserved).
• 3EH — DS segment override prefix (use with any branch instruction is reserved).
• 26H — ES segment override prefix (use with any branch instruction is reserved).
• 64H — FS segment override prefix (use with any branch instruction is reserved).
• 65H — GS segment override prefix (use with any branch instruction is reserved).
*/

// ------------
u8*
emit_mov(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    s8 extra_byte = 0;

    switch(amode.mode_type)
    {
        case ADDR_MODE_OI: {
            // TODO(psv): assert invalid cases here
            opcode.bytes[0] = ((bitsize > 8) ? 0xB8 : 0xB0) | register_representation(amode.rm);
        } break;
        case ADDR_MODE_MI: {
            assert(value_bitsize(amode.immediate) <= bitsize && amode.immediate_bitsize < 64);

            u8 op = 0xC6;               // imm8
            if(bitsize > 8) op += 1;    // 0xC7
            opcode.bytes[0] = op;

            if(amode.addr_mode == DIRECT && amode.immediate_bitsize == 16 && register_get_bitsize(amode.rm) > 16)
                amode.immediate_bitsize = 32;
            else if(amode.addr_mode != DIRECT && amode.immediate_bitsize == 16 && amode.ptr_bitsize > 16)
                amode.immediate_bitsize = 32;
            amode.reg = 0; // /0
        } break;
        case ADDR_MODE_MR: {
            opcode.bytes[0] = (bitsize > 8) ? 0x89 : 0x88;
            if(register_is_segment(amode.reg))
                opcode.bytes[0] = 0x8C;
            if (amode.addr_mode != DIRECT && amode.ptr_bitsize == 16)
                amode.ptr_bitsize = 32;
        } break;
        case ADDR_MODE_RM: {
            opcode.bytes[0] = (bitsize > 8) ? 0x8B : 0x8A;
            if(register_is_segment(amode.reg))
                opcode.bytes[0] = 0x8E;
            if (amode.addr_mode != DIRECT && amode.ptr_bitsize == 16)
                amode.ptr_bitsize = 32;
        } break;
        case ADDR_MODE_FD: {
            opcode.bytes[0] = (bitsize > 8) ? 0xA1 : 0xA0;
            if(amode.rm != REGISTER_NONE)
            {
                switch(amode.moffs_base)
                {
                    case CS: *stream++ = 0x2e; break;
                    case SS: *stream++ = 0x36; break;
                    case DS: *stream++ = 0x3e; break;
                    case ES: *stream++ = 0x26; break;
                    case FS: *stream++ = 0x64; break;
                    case GS: *stream++ = 0x65; break;
                    default: break;
                }
                extra_byte = 1;
            }
        } break;
        case ADDR_MODE_TD: {
            opcode.bytes[0] = (bitsize > 8) ? 0xA3 : 0xA2;
            if(amode.rm != REGISTER_NONE)
            {
                switch(amode.moffs_base)
                {
                    case CS: *stream++ = 0x2e; break;
                    case SS: *stream++ = 0x36; break;
                    case DS: *stream++ = 0x3e; break;
                    case ES: *stream++ = 0x26; break;
                    case FS: *stream++ = 0x64; break;
                    case GS: *stream++ = 0x65; break;
                    default: break;
                }
                extra_byte = 1;
            }
        } break;
    }

    u8* result = emit_instruction(out_info, stream, amode, opcode);
    if(out_info && extra_byte > 0)
    {
        out_info->instr_byte_size += extra_byte;
        out_info->immediate_offset += ((out_info->immediate_offset != -1) ? extra_byte : 0);
        out_info->diplacement_offset += ((out_info->diplacement_offset != -1) ? extra_byte : 0);
    }
    return result;
}

u8*
emit_cmovcc(Instr_Emit_Result* out_info, u8* stream, X64_CMOVcc_Instruction instr, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (u8)instr;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_setcc(Instr_Emit_Result* out_info, u8* stream, X64_SETcc_Instruction instr, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT && amode.ptr_bitsize == ADDR_BYTEPTR);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (u8)instr;
    amode.reg = RAX;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lar(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_RM);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x02;

    assert((amode.addr_mode == DIRECT && register_get_bitsize(amode.reg) == register_get_bitsize(amode.rm)) ||
        (amode.addr_mode != DIRECT && register_get_bitsize(amode.rm) >= 32 && amode.ptr_bitsize == 16));
    amode.ptr_bitsize = register_get_bitsize(amode.reg);

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lsl(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_RM);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x03;

    assert((amode.addr_mode == DIRECT && register_get_bitsize(amode.reg) == register_get_bitsize(amode.rm)) ||
        (amode.addr_mode != DIRECT && register_get_bitsize(amode.rm) >= 32 && amode.ptr_bitsize == 16));
    amode.ptr_bitsize = register_get_bitsize(amode.reg);

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_mov_debug_reg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_MR || amode.mode_type == ADDR_MODE_RM);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    if(amode.mode_type == ADDR_MODE_MR)
        opcode.bytes[1] = 0x21;
    else if(amode.mode_type == ADDR_MODE_RM)
        opcode.bytes[1] = 0x23;

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_mov_control_reg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_MR || amode.mode_type == ADDR_MODE_RM);

    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    if(amode.mode_type == ADDR_MODE_MR)
        opcode.bytes[1] = 0x20;
    else if(amode.mode_type == ADDR_MODE_RM)
        opcode.bytes[1] = 0x22;

    return emit_instruction(out_info, stream, amode, opcode);
} 
#define INC_DIGIT 0
#define DEC_DIGIT 1
#define NOT_DIGIT 2
#define NEG_DIGIT 3
#define MUL_DIGIT 4
#define DIV_DIGIT 6
#define IDIV_DIGIT 7
#define IMUL_DIGIT 5
#define NOP_DIGIT 0

u8*
emit_mul(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = MUL_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_div(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = DIV_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_idiv(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = IDIV_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_neg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = NEG_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_not(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = NOT_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_dec(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xfe : 0xff;
    amode.reg = DEC_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_inc(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xfe : 0xff;
    amode.reg = INC_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_nop(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_ZO)
    {
        return emit_single_byte_instruction(out_info, stream, 0x90);
    }
    else
    {
        s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
        X64_Opcode opcode = {.byte_count = 2};
        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = 0x1f;
        amode.reg = NOP_DIGIT;
        return emit_instruction(out_info, stream, amode, opcode);
    }
}

u8*
emit_imul(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    
    if(amode.immediate_bitsize > 0)
    {
        assert((register_get_bitsize(amode.reg) > 8 && amode.immediate_bitsize == 8) || 
               (register_get_bitsize(amode.reg) == amode.immediate_bitsize) ||
               (register_get_bitsize(amode.reg) == 64 && amode.immediate_bitsize == 32));
        opcode.bytes[0] = (amode.immediate_bitsize == 8) ? 0x6B : 0x69;
    }
    else if(amode.reg != REGISTER_NONE)
    {
        assert(register_get_bitsize(amode.reg) > 8);

        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = 0xaf;
        opcode.byte_count = 2;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_movsxd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = 0x63;
    bool extended = register_is_extended(amode.rm);

    assert(register_get_bitsize(amode.reg) > 8);
    //assert((amode.addr_mode != DIRECT) || register_get_bitsize(amode.reg) > 32 && bitsize == 32);

    if(register_get_bitsize(amode.reg) == 64)
    {
        // We require a 32 ptrsize just to know that we are using a up-cast 32bit->64bit sign extended
        assert(amode.ptr_bitsize == 32 || amode.addr_mode == DIRECT);
        // override the 32, because the instruction encodes 32 by default since its an up-cast.
        if (amode.addr_mode == DIRECT)
            amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        else
            amode.ptr_bitsize = 64;
    }
    
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_movsx(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    bool extended = register_is_extended(amode.rm);

    if(bitsize == 16)
    {
        opcode.bytes[1] = 0xbf;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
            {
                amode.flags |= ADDRMODE_FLAG_REXW;
                amode.ptr_bitsize = register_get_bitsize(amode.rm); // just to ignore the 16 bit override prefix
            }
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if (register_get_bitsize(amode.reg) == 32)
            amode.ptr_bitsize = 32;
    }
    else
    {
        opcode.bytes[1] = 0xbe;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
                amode.flags |= ADDRMODE_FLAG_REXW;
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if(register_get_bitsize(amode.reg) == 16)
            amode.ptr_bitsize = 16;
    }

    assert(register_get_bitsize(amode.reg) > 8);

    return emit_instruction(out_info, stream, amode, opcode);
}

// TODO(psv): test this
u8*
emit_movzx(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    bool extended = register_is_extended(amode.rm);

    if(bitsize == 16)
    {
        opcode.bytes[1] = 0xb6;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
            {
                amode.flags |= ADDRMODE_FLAG_REXW;
                amode.ptr_bitsize = register_get_bitsize(amode.rm); // just to ignore the 16 bit override prefix
            }
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if (register_get_bitsize(amode.reg) == 32)
            amode.ptr_bitsize = 32;
    }
    else
    {
        opcode.bytes[1] = 0xb7;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
                amode.flags |= ADDRMODE_FLAG_REXW;
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if(register_get_bitsize(amode.reg) == 16)
            amode.ptr_bitsize = 16;
    }

    assert(register_get_bitsize(amode.reg) > 8);

    return emit_instruction(out_info, stream, amode, opcode);
} 
u8*
emit_shift(Instr_Emit_Result* out_info, u8* stream, X64_Shift_Instruction instr, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    switch(amode.mode_type)
    {
        case ADDR_MODE_M1: {
            opcode.bytes[0] = (bitsize > 8) ? 0xD1 : 0xD0;
            amode.reg = instr;
        } break;
        case ADDR_MODE_MC: {
            opcode.bytes[0] = (bitsize > 8) ? 0xD3 : 0xD2;
            amode.reg = instr;
        } break;
        case ADDR_MODE_MI: {
            assert(amode.immediate_bitsize == 8);
            opcode.bytes[0] = (bitsize > 8) ? 0xC1 : 0xC0;
            amode.reg = instr;
        } break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_test(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;

    X64_Opcode opcode = {.byte_count = 1};
    switch(amode.mode_type)
    {
        case ADDR_MODE_I: {
            assert(amode.immediate_bitsize <= 32);
            assert(register_equivalent(amode.rm, RAX));
            opcode.bytes[0] = (bitsize > 8) ? 0xA9 : 0xA8;
        } break;
        case ADDR_MODE_MI: {
            assert(MIN(32, bitsize) == amode.immediate_bitsize);
            opcode.bytes[0] = (bitsize > 8) ? 0xF7 : 0xF6;
            amode.reg = 0;
        } break;
        case ADDR_MODE_MR: {
            opcode.bytes[0] = (bitsize > 8) ? 0x85 : 0x84;
        } break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}


u8*
emit_shld(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(((amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize) > 8);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    if(amode.mode_type == ADDR_MODE_MRI)
    {
        opcode.bytes[1] = 0xa4;
    }
    else if(amode.mode_type == ADDR_MODE_MRC)
    {
        opcode.bytes[1] = 0xa5;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_shrd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(((amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize) > 8);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    if(amode.mode_type == ADDR_MODE_MRI)
    {
        opcode.bytes[1] = 0xac;
    }
    else if(amode.mode_type == ADDR_MODE_MRC)
    {
        opcode.bytes[1] = 0xad;
    }
    return emit_instruction(out_info, stream, amode, opcode);
} 
u8*
emit_p3(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, u8 prefix, u8 op)
{
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = op;
    return emit_instruction_prefixed(out_info, stream, amode, opcode, prefix);
}

u8*
emit_p2(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, u8 op)
{
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = op;
    return emit_instruction(out_info, stream, amode, opcode);
}

// ----------------------------------------------------------------------
// ------------------------------ SSE -----------------------------------
u8* 
emit_addps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x58);
}

u8* 
emit_addss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x58);
}

u8* 
emit_subps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5c);
}

u8* 
emit_subss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5c);
}

u8* 
emit_andps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x54);
}

u8* 
emit_orps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x56);
}

u8* 
emit_xorps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x57);
}

u8* 
emit_andnotps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x55);
}

u8* 
emit_divps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5e);
}

u8* 
emit_divss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5e);
}

u8* 
emit_mulps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x59);
}

u8* 
emit_mulss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x59);
}

u8* 
emit_cmpps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p2(out_info, stream, amode, 0xC2);
}

u8* 
emit_cmpss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p3(out_info, stream, amode, 0xf3, 0xC2);
}

u8* 
emit_comiss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2f);
}

u8* 
emit_ucomiss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2e);
}

u8* 
emit_cvtpi2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2a);
}

u8* 
emit_cvtps2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2d);
}

u8* 
emit_cvttps2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2c);
}

u8* 
emit_cvtsi2ss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf3, 0x2a);
}

u8* 
emit_cvtss2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf3, 0x2d);
}

u8* 
emit_cvttss2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf3, 0x2c);
}

// Load
u8* 
emit_movaps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    switch(amode.mode_type)
    {
        case ADDR_MODE_A:
            return emit_p2(out_info, stream, amode, 0x28);
        case ADDR_MODE_B:
            return emit_p2(out_info, stream, amode, 0x29);
        default: break;
    }
    return stream;
}

u8* 
emit_movups(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x10);
}

u8* 
emit_movhps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x16);
}

u8* 
emit_movlps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x12);
}

u8* 
emit_movss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x10);
}

// ----------------------------------------------------------------------
// ------------------------------ SSE2 ----------------------------------

u8* 
emit_addpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x58);
}

u8* 
emit_addsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x58);
}

u8* 
emit_subpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5c);
}

u8* 
emit_subsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x5c);
}

u8* 
emit_andpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x54);
}

u8* 
emit_orpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x56);
}

u8* 
emit_xorpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x57);
}

u8* 
emit_andnotpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x55);
}

u8* 
emit_divpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5e);
}

u8* 
emit_divsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x5e);
}

u8* 
emit_mulpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x59);
}

u8* 
emit_mulsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x59);
}

u8* 
emit_cmppd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p3(out_info, stream, amode, 0x66, 0xC2);
}

u8* 
emit_cmpsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p3(out_info, stream, amode, 0xf2, 0xC2);
}

u8* 
emit_comisd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2f);
}

u8* 
emit_ucomisd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2e);
}

u8* 
emit_cvtpi2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2a);
}

u8* 
emit_cvtpd2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2d);
}

u8* 
emit_cvtss2sd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5a);
}

u8* 
emit_cvtsd2ss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x5a);
}

u8* 
emit_cvtdq2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0xe6);
}

u8* 
emit_cvtdq2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5b);
}

u8* 
emit_cvtpd2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0xe6);
}

u8* 
emit_cvtpd2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5a);
}

u8* 
emit_cvtps2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5b);
}

u8* 
emit_cvtps2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5a);
}

u8* 
emit_cvtsd2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x2d);
}

u8* 
emit_cvtsi2sd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x2a);
}

u8* 
emit_cvttpd2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0xe6);
}

u8* 
emit_cvttpd2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2c);
}

u8* 
emit_cvttps2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5b);
}

u8* 
emit_cvttsd2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x2c);
}

// Load
u8* 
emit_movapd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    switch(amode.mode_type)
    {
        case ADDR_MODE_A:
            return emit_p3(out_info, stream, amode, 0x66, 0x28);
        case ADDR_MODE_B:
            return emit_p3(out_info, stream, amode, 0x66, 0x29);
        default: break;
    }
    return stream;
}

u8* 
emit_movupd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0x66, 0x10);
}

u8* 
emit_movhpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_p3(out_info, stream, amode, 0x66, 0x16);
}

u8* 
emit_movlpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_p3(out_info, stream, amode, 0x66, 0x12);
}

u8* 
emit_movsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x10);
} 
u8*
emit_int3(Instr_Emit_Result* out_info, u8* stream)
{
    return emit_single_byte_instruction(out_info, stream, 0xcc);
}

u8*
emit_int1(Instr_Emit_Result* out_info, u8* stream)
{
    return emit_single_byte_instruction(out_info, stream, 0xf1);
}

u8*
emit_int(Instr_Emit_Result* out_info, u8* stream, u8 rel)
{
    *stream++ = 0xcd;
    *stream++ = rel;
    fill_outinfo(out_info, 2, -1, 1);
    return stream;
}

u8*
emit_leave(Instr_Emit_Result* out_info, u8* stream)
{
    return emit_single_byte_instruction(out_info, stream, 0xc9);
}

u8*
emit_leave16(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x66;
    *stream++ = 0xc9;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_ret(Instr_Emit_Result* out_info, u8* stream, X64_Ret_Instruction ret, u16 imm)
{
    if(imm == 0) 
    {
        switch(ret)
        {
            case RET_NEAR: {
                *stream++ = 0xc3;
            } break;
            case RET_FAR: {
                *stream++ = 0xcb;
            } break;            
        }
        fill_outinfo(out_info, 1, -1, -1);
    }
    else
    {
        switch(ret)
        {
            case RET_NEAR: {
                *stream++ = 0xc2;
                *(uint16_t*)stream = imm;
                stream += sizeof(uint16_t);
            } break;
            case RET_FAR: {
                *stream++ = 0xca;
                *(uint16_t*)stream = imm;
                stream += sizeof(uint16_t);
            } break;
        }
        fill_outinfo(out_info, 3, -1, 1);
    }
    return stream;
}

#define PUSH_DIGIT 6
#define POP_DIGIT 0

u8*
emit_push(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;
    switch(amode.mode_type)
    {
        case ADDR_MODE_I: {
            s8 imm_offset = 1;
            switch(amode.immediate_bitsize)
            {
                case 8: {
                    *stream++ = 0x6A;
                } break;
                case 16: {
                    *stream++ = 0x66;
                    imm_offset = 2;
                } 
                case 32: {
                    *stream++ =0x68;
                } break;
            }
            stream = emit_value_raw(stream, amode.immediate, amode.immediate_bitsize);
            fill_outinfo(out_info, stream - start, -1, imm_offset);
        } break;
        case ADDR_MODE_ZO: {
            assert(amode.reg == FS || amode.reg == GS);

            *stream++ = 0x0F;
            if(amode.reg == FS)
                *stream++ = 0xA0;
            else if(amode.reg == GS)
                *stream++ = 0xA8;
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_O: {
            assert(register_get_bitsize(amode.reg) == 16 || register_get_bitsize(amode.reg) == 64);
            
            if(register_get_bitsize(amode.reg) == 16)
                *stream++ = 0x66;
            if(register_is_extended(amode.reg))
                *stream++ = make_rex(1, 0, 0, 0);
            *stream++ = 0x50 | (0x7 & register_representation(amode.reg));            
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_M: {
            s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
            assert(bitsize == 16 || bitsize == 64);
            X64_Opcode opcode = {.byte_count = 1};
            opcode.bytes[0] = 0xff;
            amode.reg = PUSH_DIGIT;
            if(bitsize == 64)
                amode.flags |= ADDRMODE_FLAG_NO_REXW;

            stream = emit_instruction(out_info, stream, amode, opcode);
        } break;
    }
    return stream;
}

u8* 
emit_pop(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;
    switch(amode.mode_type)
    {
        case ADDR_MODE_ZO: {
            assert(amode.reg == FS || amode.reg == GS);

            if(amode.ptr_bitsize == 16)
                *stream++ = 0x66;

            *stream++ = 0x0F;
            if(amode.reg == FS)
                *stream++ = 0xA1;
            else if(amode.reg == GS)
                *stream++ = 0xA9;
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_O: {
            assert(register_get_bitsize(amode.reg) == 16 || register_get_bitsize(amode.reg) == 64);
            
            if(register_get_bitsize(amode.reg) == 16)
                *stream++ = 0x66;
            if(register_is_extended(amode.reg))
                *stream++ = make_rex(1, 0, 0, 0);
            *stream++ = 0x58 | (0x7 & register_representation(amode.reg));            
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_M: {
            s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
            assert(bitsize == 16 || bitsize == 64);
            X64_Opcode opcode = {.byte_count = 1};
            opcode.bytes[0] = 0x8f;
            amode.reg = POP_DIGIT;
            if(bitsize == 64)
                amode.flags |= ADDRMODE_FLAG_NO_REXW;

            stream = emit_instruction(out_info, stream, amode, opcode);
        } break;
    }
    return stream;
}

u8* 
emit_enter(Instr_Emit_Result* out_info, u8* stream, u16 storage_size, u8 lex_nest_level, bool b16)
{
    u8* start = stream;
    if(b16) *stream++ = 0x66;
    *stream++ = 0xc8;
    s8 imm_offset = (s8)(stream - start);
    *((u16*)stream) = storage_size;
    stream += sizeof(u16);
    *stream++ = lex_nest_level;
    fill_outinfo(out_info, (s8)(stream - start), -1, imm_offset);
    return stream;
}

u8*
emit_in(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.mode_type == ADDR_MODE_I)
    {
        assert(amode.immediate_bitsize == 8);
        opcode.bytes[0] = (register_get_bitsize(amode.rm) == 8) ? 0xe4 : 0xe5;
        assert(amode.rm == REGISTER_NONE || register_equivalent(RAX, amode.rm));
    }
    else if(amode.mode_type == ADDR_MODE_ZO)
    {
        opcode.bytes[0] = (amode.ptr_bitsize == 8) ? 0xec : 0xed;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_out(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.mode_type == ADDR_MODE_I)
    {
        assert(amode.immediate_bitsize == 8);
        opcode.bytes[0] = (register_get_bitsize(amode.rm) == 8) ? 0xe6 : 0xe7;
        assert(amode.rm == REGISTER_NONE || register_equivalent(RAX, amode.rm));
    }
    else if(amode.mode_type == ADDR_MODE_ZO)
    {
        opcode.bytes[0] = (amode.ptr_bitsize == 8) ? 0xee : 0xef;
    }
    return emit_instruction(out_info, stream, amode, opcode);
} 
u8*
emit_strinstr(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize, u8 op)
{
    u8* start = stream;
    switch(ptr_bitsize)
    {
        case 8:  *stream++ = op; break;
        case 16: {
            *stream++ = 0x66;
            *stream++ = op;
        } break;
        case 32: *stream++ = op; break;
        case 64: {
            *stream++ = make_rex(0, 0, 0, 1);
            *stream++ = op;
        } break;
        default: assert(0); break;
    }
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

#define CMPS 0xa7
#define CMPS8 0xa6
u8*
emit_cmps(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? CMPS8 : CMPS);
}

#define SCAS 0xaf
#define SCAS8 0xae
u8*
emit_scas(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? SCAS8 : SCAS);
}

#define STOS 0xab
#define STOS8 0xaa
u8*
emit_stos(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? STOS8 : STOS);
}

#define LODS 0xad
#define LODS8 0xac
u8*
emit_lods(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? LODS8 : LODS);
}

#define MOVS 0xa5
#define MOVS8 0xa4
u8*
emit_movs(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? MOVS8 : MOVS);
}

u8*
emit_cmpxchg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(amode.mode_type == ADDR_MODE_MR);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (bitsize == 8) ? 0xb0 : 0xb1;
    return emit_instruction(out_info, stream, amode, opcode);
}

#define INS 0x6d
#define INS8 0x6c
u8*
emit_ins(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    u8* start = stream;
    switch(ptr_bitsize)
    {
        case 8:  *stream++ = INS8; break;
        case 16: {
            *stream++ = 0x66;
            *stream++ = INS;
        } break;
        case 32: *stream++ = INS; break;
        default: assert(0); break;
    }
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

#define OUTS 0x6f
#define OUTS8 0x6e
u8*
emit_outs(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    u8* start = stream;
    switch(ptr_bitsize)
    {
        case 8:  *stream++ = OUTS8; break;
        case 16: {
            *stream++ = 0x66;
            *stream++ = OUTS;
        } break;
        case 32: *stream++ = OUTS; break;
        default: assert(0); break;
    }
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
} 
 
#endif // HO_ASSEMBLER_IMPLEMENT 
