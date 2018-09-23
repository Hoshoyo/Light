#pragma once
#include "ho_system.h"

extern "C" u64 call_external(void* proc_address, u64 call_stack_ptr, u64 size_stack);

#define FLAG(X) (1 << X)
#define REG_SIZE 8

// Instruction type
const u16 HLT = 0;
const u16 ADD = 1;
const u16 SUB = 2;
const u16 MUL = 3;
const u16 DIV = 4;
const u16 MOD = 5;
const u16 BEQ = 6;
const u16 BNE = 7;
const u16 BLT = 8;
const u16 BLE = 9;
const u16 BGT = 10;
const u16 BGE = 11;
const u16 JMP = 12;
const u16 PUSH = 13;
const u16 POP = 14;
const u16 CALL = 15;	// @todo
const u16 RET = 16;	// @todo
const u16 CMP = 17;
const u16 MOV = 18;
const u16 EXTCALL = 19;
const u16 AND = 20;
const u16 OR = 21;
const u16 XOR = 22;
const u16 NOT = 23;
const u16 SHL = 24;
const u16 SHR = 25;

// Instruction addressing
const u8 NO_ADDRESSING = 0;
const u8 REG_TO_REG = 1;
const u8 REG_TO_REG_PTR = 2;
const u8 REG_PTR_TO_REG = 3;
const u8 MEM_TO_REG = 4;
const u8 MEM_PTR_TO_REG = 5;
const u8 REG_TO_MEM_PTR = 6;
const u8 SINGLE_REG = 7;
const u8 SINGLE_REG_PTR = 8;
const u8 SINGLE_MEM = 9;
const u8 SINGLE_MEM_PTR = 10;

// Instruction flags
const u16 IMMEDIATE_OFFSET = FLAG(0);
const u16 REGISTER_OFFSET = FLAG(1);
const u16 SIGNED = FLAG(2);
const u16 INDIRECT_ADDR = FLAG(3); // @todo
const u16 INSTR_BYTE = FLAG(4);
const u16 INSTR_WORD = FLAG(5);
const u16 INSTR_DWORD = FLAG(6);
const u16 INSTR_QWORD = FLAG(7);
const u16 IMMEDIATE_VALUE = FLAG(8);

// Flags register
//const s64 FLAGS_REG_OVERFLOW	= FLAG(0);	// @todo
//const s64 FLAGS_REG_CARRY		= FLAG(1);	// @todo
const s64 FLAGS_REG_ZERO = FLAG(2);
const s64 FLAGS_REG_SIGN = FLAG(3);

enum Registers {

	R_0 = 0,
	R_1 = 1,
	R_2 = 2,
	R_3 = 3,
	R_4 = 4,
	R_5 = 5,
	R_6 = 6,
	R_7 = 7,
	R_8 = 8,

	R_IP = 9,
	R_SP = 10,
	R_SB = 11,
	R_SS = 12,

	R_FLAGS = 13,

	NO_REG = 14,
	NUM_REGS = 15,
};

struct Instruction {
	u16 type;
	u16 flags;

	u8 addressing;
	u8 left_reg;
	u8 right_reg;
	u8 offset_reg;

	s64 immediate_offset;
};

void init_interpreter(s64 stack_size = 1024 * 1024, s64 heap_size = 1024 * 1024);
int run_interpreter();

u64 move_code_offset(s64 offset);
u64 add_code_offset(s64 offset);

u64 get_data_segment_address();

u64 push_instruction(Instruction inst);
u64 push_instruction(Instruction inst, u64 next_word);
u64 push_instruction(Instruction inst, u64** out_next_word);
Instruction make_instruction(u16 type, u16 flags, u8 addressing, u8 left_reg, u8 right_reg, u8 offset_reg, s32 immediate_offset);

#define HALT push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0))
