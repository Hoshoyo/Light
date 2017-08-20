#include "util.h"
#include "ho_system.h"
#define internal static
#define FLAG(X) (1 << X);

// Instruction type
const u16 ADD  = 1;
const u16 SUB  = 2;
const u16 MUL  = 3;
const u16 DIV  = 4;
const u16 MOD  = 5;
const u16 BEQ  = 6;
const u16 BLT  = 7;
const u16 BLE  = 8;
const u16 BGT  = 9;
const u16 BGE  = 10;
const u16 JMP  = 11;
const u16 PUSH = 12;
const u16 POP  = 13;
const u16 CALL = 14;
const u16 RET  = 15;
const u16 HLT  = 16;

// Instruction addressing
const u8 REG_TO_REG			= 1;
const u8 REG_TO_REG_PTR		= 2;
const u8 REG_PTR_TO_REG		= 3;
const u8 MEM_TO_REG			= 4;
const u8 MEM_PTR_TO_REG		= 5;
const u8 REG_TO_MEM			= 6;
const u8 REG_TO_MEM_PTR		= 7;
const u8 SINGLE_REG			= 8;
const u8 SINGLE_MEM			= 9;

// Instruction flags
const u16 IMMEDIATE_OFFSET	= FLAG(0);
const u16 REGISTER_OFFSET	= FLAG(1);
const u16 SIGNED			= FLAG(2);
const u16 INDIRECT_ADDR		= FLAG(3);
const u16 PTR_BYTE			= FLAG(4);
const u16 PTR_WORD			= FLAG(5);
const u16 PTR_DWORD			= FLAG(6);
const u16 PTR_QWORD			= FLAG(7);

struct Instruction {
	u16 type;
	u16 flags;

	u8 addressing;
	u8 left_reg;
	u8 right_reg;
	u8 offset_reg;

	s32 immediate_offset;
};

enum Registers {
	R_IP = 0,
	R_SP = 1,
	R_SB = 2,
	R_1 = 3,
	R_2 = 4,
	R_3 = 5,
	R_4 = 6,
	R_5 = 7,
	R_6 = 8,
	R_7 = 9,
	R_8 = 10,
	NUM_REGS = 11,
};

internal u8* stack;
internal u8* heap;
internal u8* code;
internal bool running;

void print_instruction(u64 instruction, u64 next_qword);
void execute(u64 instruction, u64 next_word);

void init_interpreter(s64 stack_size = 1024 * 1024, s64 heap_size = 1024 * 1024) 
{
	stack = (u8*)ho_bigalloc_rw(stack_size);
	heap  = (u8*)ho_bigalloc_rw(heap_size);
	code  = (u8*)ho_bigalloc_rw(1024 * 1024);
}

int run_interpreter()
{
	s64 reg[NUM_REGS] = {};
	running = true;	
	while (running) {
		s64 address = *(s64*)reg[R_IP];
		u64 instruction = *((u64*)(code + address));
		u64 immediate = *((u64*)(code + address + 8));
		execute(instruction, immediate);

		print_instruction(instruction, immediate);
	}
}

void execute(u64 instruction, u64 next_word)
{

}

void print_instruction(u64 instruction, u64 next_qword)
{
	Instruction inst = *(Instruction*)instruction;
	switch (inst.type) {
		case ADD  : printf("ADD "); break;
		case SUB  : printf("SUB "); break;
		case MUL  : printf("MUL "); break;
		case DIV  : printf("DIV "); break;
		case MOD  : printf("MOD "); break;
		case BEQ  : printf("BEQ "); break;
		case BLT  : printf("BLT "); break;
		case BLE  : printf("BLE "); break;
		case BGT  : printf("BGT "); break;
		case BGE  : printf("BGE "); break;
		case JMP  : printf("JMP "); break;
		case PUSH : printf("PUSH "); break;
		case POP  : printf("POP "); break;
		case CALL : printf("CALL "); break;
		case RET  : printf("RET "); break;
		default   : printf("UNKNOWN "); break;
	}

	switch (inst.addressing) {
	case REG_TO_REG	    : printf("R_%d, R_%d", inst.left_reg, inst.right_reg); break;
	case MEM_TO_REG     : printf("R_%d, %x", inst.left_reg, next_qword); break;
	case REG_TO_MEM		: printf("%x, R_%d", next_qword, inst.right_reg); break;
	case SINGLE_REG		: printf("R_%d", inst.left_reg); break;
	case SINGLE_MEM		: printf("%x", next_qword); break;

	case REG_TO_REG_PTR: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("[R_%d + %x], R_%d", inst.left_reg, inst.immediate_offset, inst.right_reg);
		else if(inst.flags & REGISTER_OFFSET)
			printf("[R_%d + R_%d], R_%d", inst.left_reg, inst.offset_reg, inst.right_reg);
		else
			printf("[R_%d], R_%d", inst.left_reg, inst.right_reg);
	}break;
	case REG_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("R_%d, [R_%d + %x]", inst.left_reg, inst.right_reg, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			printf("R_%d, [R_%d + R_%d]", inst.left_reg, inst.right_reg, inst.offset_reg);
		else
			printf("R_%d, [R_%d]", inst.left_reg, inst.right_reg);
	}break;
	case MEM_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("R_%d, [%x + %x]", inst.left_reg, next_qword, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			printf("R_%d, [%x + R_%d]", inst.left_reg, next_qword, inst.offset_reg);
		else
			printf("R_%d, [%x]", inst.left_reg, next_qword);
	}break;
	case REG_TO_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET)
			printf("[%x + %x], R_%d", next_qword, inst.immediate_offset, inst.right_reg);
		else if(inst.flags & REGISTER_OFFSET)
			printf("[%x + R_%d], R_%d", next_qword, inst.offset_reg, inst.right_reg);
		else
			printf("[%x], R_%d", next_qword, inst.right_reg);
	}break;
	}

	if (inst.flags & PTR_BYTE) {
		printf(" byte ");
	} else if (inst.flags & PTR_WORD) {
		printf(" word ");
	} else if (inst.flags & PTR_DWORD) {
		printf(" dword ");
	} else if (inst.flags & PTR_QWORD) {
		printf(" qword ");
	}

	if (inst.flags & SIGNED) {
		printf("signed");
	} else {
		printf("unsigned");
	}

	printf("\n");
}