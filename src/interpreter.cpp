#include "util.h"
#include "ho_system.h"
#define internal static
#define FLAG(X) (1 << X);
#define REG_SIZE 8

#define HALT push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0))

// external functions
extern "C" void call_external(void* proc_address, u8* call_stack_ptr, u64 size_stack);
extern "C" void call_external_DEBUG(void* proc_address, u8* call_stack_ptr, u64 size_stack);

// Instruction type
const u16 HLT  = 0;
const u16 ADD  = 1;
const u16 SUB  = 2;
const u16 MUL  = 3;
const u16 DIV  = 4;
const u16 MOD  = 5;
const u16 BEQ  = 6;
const u16 BNE  = 7;
const u16 BLT  = 8;
const u16 BLE  = 9;
const u16 BGT  = 10;
const u16 BGE  = 11;
const u16 JMP  = 12;
const u16 PUSH = 13;	// @todo
const u16 POP  = 14;	// @todo
const u16 CALL = 15;	// @todo
const u16 RET  = 16;	// @todo
const u16 CMP  = 17;
const u16 MOV  = 18;
const u16 EXTCALL = 19;
const u16 AND  = 20;
const u16 OR   = 21;
const u16 XOR  = 22;
const u16 NOT  = 23;

// Instruction addressing
const u8 NO_ADDRESSING		= 0;
const u8 REG_TO_REG			= 1;
const u8 REG_TO_REG_PTR		= 2;
const u8 REG_PTR_TO_REG		= 3;
const u8 MEM_TO_REG			= 4;
const u8 MEM_PTR_TO_REG		= 5;
const u8 REG_TO_MEM_PTR		= 6;
const u8 SINGLE_REG			= 7;
const u8 SINGLE_REG_PTR     = 8;
const u8 SINGLE_MEM			= 9;
const u8 SINGLE_MEM_PTR		= 10;

// Instruction flags
const u16 IMMEDIATE_OFFSET	= FLAG(0);
const u16 REGISTER_OFFSET	= FLAG(1);
const u16 SIGNED			= FLAG(2);
const u16 INDIRECT_ADDR		= FLAG(3); // @todo
const u16 INSTR_BYTE		= FLAG(4);
const u16 INSTR_WORD		= FLAG(5);
const u16 INSTR_DWORD		= FLAG(6);
const u16 INSTR_QWORD		= FLAG(7);
const u16 IMMEDIATE_VALUE   = FLAG(8);

// Flags register
//const s64 FLAGS_REG_OVERFLOW	= FLAG(0);	// @todo
//const s64 FLAGS_REG_CARRY		= FLAG(1);	// @todo
const s64 FLAGS_REG_ZERO		= FLAG(2);
const s64 FLAGS_REG_SIGN		= FLAG(3);

enum Registers {
	R_IP = 0,

	R_1 = 1,
	R_2 = 2,
	R_3 = 3,
	R_4 = 4,
	R_5 = 5,
	R_6 = 6,
	R_7 = 7,
	R_8 = 8,
	
	R_SP = 9,
	R_SB = 10,
	R_SS = 11,

	R_FLAGS = 12,

	NO_REG = 13,
	NUM_REGS = 14,
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

internal u8* stack_ptr = 0;
internal u8* heap_ptr = 0;
internal u8* code_ptr = 0;
internal u8* datas_ptr = 0;

internal u8* stack;
internal u8* heap;
internal u8* code;
internal u8* datas;
internal bool running;
s64 reg[NUM_REGS] = {};

void print_instruction(Instruction instruction, u64 next_qword);
int execute(Instruction instruction, u64 next_word);

u64 push_instruction(Instruction inst) {
	*(Instruction*)code_ptr = inst;
	if (inst.flags & IMMEDIATE_OFFSET)
		code_ptr += sizeof(Instruction);
	else
		code_ptr += sizeof(Instruction) - sizeof(inst.immediate_offset);
	return (u64)code_ptr;
}

u64 push_instruction(Instruction inst, u64 next_word) {
	push_instruction(inst);
	*(u64*)code_ptr = next_word;	// @todo this should be the size of the immediate value u16 u8 s32 etc..
	code_ptr += sizeof(next_word);
	return (u64)code_ptr;
}

Instruction make_instruction(u16 type, u16 flags, u8 addressing, u8 left_reg, u8 right_reg, u8 offset_reg, s32 immediate_offset) {
	Instruction res;
	res.type = type;
	res.flags = flags;
	res.addressing = addressing;
	res.left_reg = left_reg;
	res.right_reg = right_reg;
	res.offset_reg = offset_reg;
	res.immediate_offset = immediate_offset;
	return res;
}

void teste(char* str, int d)
{
	printf("%s %d\n", str, d);
}

void init_interpreter(s64 stack_size = 1024 * 1024, s64 heap_size = 1024 * 1024) 
{
	stack = (u8*)ho_bigalloc_rw(stack_size);
	heap  = (u8*)ho_bigalloc_rw(heap_size);
	code  = (u8*)ho_bigalloc_rw(1024 * 1024);
	datas = (u8*)ho_bigalloc_rw(1024 * 1024);

	stack_ptr = stack;
	heap_ptr = heap;
	code_ptr = code;
	datas_ptr = datas;

	reg[R_IP] = (u64)code;
	reg[R_SP] = (u64)stack;
	reg[R_SB] = (u64)stack;

	{
		const char* str = "iterations :";
		*(u64*)datas_ptr = (u64)str;
		//*(u64*)datas_ptr = (u64)-(8 * 4); datas_ptr += sizeof(u64);
	}

	u64 label = (u64)code_ptr;
	u64 start = (u64)code_ptr;
#if 0
	{
		// mov r1, 0
		// mov r2, 10
		// label:
		// mov [heap + r1], r2
		// add r1, 8
		// sub r2, 1
		// cmp r2, 0
		// bgt label
		// hlt

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 10);
		push_instruction(make_instruction(MOV, REGISTER_OFFSET | IMMEDIATE_VALUE, REG_TO_MEM_PTR, NO_REG, R_2, R_1, 0), (u64)heap);
		push_instruction(make_instruction(ADD, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 8);
		push_instruction(make_instruction(SUB, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 1);
		push_instruction(make_instruction(CMP, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(BGT, SIGNED | IMMEDIATE_OFFSET, NO_ADDRESSING, NO_REG, NO_REG, 0, -64));
		push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));

	}
#endif
#if 0
	{
		// IMMEDIATE_OFFSET | REGISTER_OFFSET | SIGNED | IMMEDIATE_VALUE

		// mov r2, datas_ptr
		// mov r1, 5
		// sub r1, 1
		// extcall print
		// cmp r1, 0
		// bge [r2]
		// hlt
		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), (u64)datas_ptr - 8);
		start = push_instruction(make_instruction(MOV, IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), 5);
		push_instruction(make_instruction(SUB, SIGNED | IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_1, NO_REG, NO_REG, 0), 1);
		label = push_instruction(make_instruction(CMP, SIGNED | IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(BGE, SIGNED, SINGLE_REG_PTR, R_2, NO_REG, 0, 0));
		push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));
		
	}
#endif
	// PUSH val
	// mov r1, val
	// mov [rsp], r1
	// add rsp, 8

	// POP val
	// sub rsp, 8
	// mov val, [rsp]

	{
		// push datas_ptr
		// mov r2, 0
		// mov ss, 8
	// label:
		// push r2
		// extcall printf
		// sub rsp, 8
		// add r2, 1
		// cmp r2, 10
		// jle label
		// hlt

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE|INSTR_QWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), *(u64*)datas_ptr);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 0);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 8);

		// label
		push_instruction(make_instruction(MOV, 0, REG_TO_REG, R_1, R_2, 0, 0));
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(EXTCALL, INSTR_QWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)teste);

		push_instruction(make_instruction(SUB, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 1);
		push_instruction(make_instruction(CMP, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 10);

		push_instruction(make_instruction(BLE, SIGNED | IMMEDIATE_OFFSET, NO_ADDRESSING, NO_REG, NO_REG, 0, -80 - 16));
		HALT;
	}
}

#define PRINT_INSTRUCTIONS 0 
int run_interpreter()
{
	running = true;	
	while (running) {
		u64 address = reg[R_IP];
		Instruction instruction = *(Instruction*)reg[R_IP];
		if(instruction.flags & IMMEDIATE_OFFSET){
			instruction.immediate_offset = *((u64*)(address + REG_SIZE));
			address += REG_SIZE;
		}
		u64 immediate = *((u64*)(address + REG_SIZE));

#if PRINT_INSTRUCTIONS
		print_instruction(instruction, immediate);
#endif
		if (execute(instruction, immediate)) break;
	}
	return 0;
}

template <typename T>
int execute_instruction(Instruction inst, u64 next_word)
{	
	T ui_left, ui_right;
	bool write_memory = false;
	bool write_register = false;
	bool advance_ip = true;
	u64 address_to_write = 0;

	switch (inst.addressing) {
	case REG_TO_REG: {
		ui_left = reg[inst.left_reg];
		ui_right = reg[inst.right_reg];
		write_register = true;
	}break;
	case MEM_TO_REG: {
		ui_left = reg[inst.left_reg];
		ui_right = next_word;
		write_register = true;
	}break;
	case SINGLE_REG: {
		ui_left = reg[inst.left_reg];
	}break;
	case SINGLE_MEM: {
		ui_left = next_word;
	}break;
	case SINGLE_MEM_PTR: {
		assert(inst.flags & IMMEDIATE_VALUE);
		if (inst.flags & IMMEDIATE_OFFSET) {
			address_to_write = next_word + inst.immediate_offset;
		} else if (inst.flags & REGISTER_OFFSET) {
			address_to_write = next_word + reg[inst.offset_reg];
		} else {
			address_to_write = next_word;
		}
		ui_left = *(T*)(address_to_write);
	}break;
	case SINGLE_REG_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			address_to_write = reg[inst.left_reg] + inst.immediate_offset;
		} else if (inst.flags & REGISTER_OFFSET) {
			address_to_write = reg[inst.left_reg] + reg[inst.offset_reg];
		} else {
			address_to_write = reg[inst.left_reg];
		}
		ui_left = *(T*)(address_to_write);
	}break;
	case REG_TO_REG_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			address_to_write = reg[inst.left_reg] + inst.immediate_offset;
		} else if (inst.flags & REGISTER_OFFSET) {
			address_to_write = reg[inst.left_reg] + reg[inst.offset_reg];
		} else {
			address_to_write = reg[inst.left_reg];
		}
		ui_left = *(T*)(address_to_write);
		ui_right = reg[inst.right_reg];
		write_memory = true;
	}break;
	case REG_PTR_TO_REG: {
		ui_left = reg[inst.left_reg];
		if (inst.flags & IMMEDIATE_OFFSET)
			ui_right = *(T*)(reg[inst.right_reg] + inst.immediate_offset);
		else if (inst.flags & REGISTER_OFFSET)
			ui_right = *(T*)(reg[inst.right_reg] + reg[inst.offset_reg]);
		else
			ui_right = *(T*)reg[inst.right_reg];
		write_register = true;
	}break;
	case MEM_PTR_TO_REG: {
		ui_left = reg[inst.left_reg];
		if (inst.flags & IMMEDIATE_OFFSET)
			ui_right = *(T*)(next_word + inst.immediate_offset);
		else if (inst.flags & REGISTER_OFFSET)
			ui_right = *(T*)(next_word + reg[inst.offset_reg]);
		else
			ui_right = *(T*)next_word;
		write_register = true;
	}break;
	case REG_TO_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			address_to_write = next_word + inst.immediate_offset;
		} else if (inst.flags & REGISTER_OFFSET) {
			address_to_write = next_word + reg[inst.offset_reg];
		} else {
			address_to_write = next_word;
		}
		ui_left = *(T*)(address_to_write);
		ui_right = reg[inst.right_reg];
		write_memory = true;
	}break;
	}
	switch (inst.type) {
	case ADD:	ui_left = ui_left + ui_right; break;
	case SUB:	ui_left = ui_left - ui_right; break;
	case MUL:	ui_left = ui_left * ui_right; break;
	case DIV:	ui_left = ui_left / ui_right; break;
	case MOD:	ui_left = ui_left % ui_right; break;

	case CMP: {
		if (ui_left < ui_right)
			reg[R_FLAGS] |= FLAGS_REG_SIGN;
		else
			reg[R_FLAGS] &= ~FLAGS_REG_SIGN;
		if (ui_left == ui_right)
			reg[R_FLAGS] |= FLAGS_REG_ZERO;
		else
			reg[R_FLAGS] &= ~FLAGS_REG_ZERO;
	}break;

	case BEQ: {
		if (reg[R_FLAGS] & FLAGS_REG_ZERO) {
			if (inst.flags & IMMEDIATE_OFFSET)
				reg[R_IP] += inst.immediate_offset;
			else
				reg[R_IP] += ui_left;
			advance_ip = false;
		}
	}break;
	case BNE: {
		if (!(reg[R_FLAGS] & FLAGS_REG_ZERO)) {
			if (inst.flags & IMMEDIATE_OFFSET)
				reg[R_IP] += inst.immediate_offset;
			else
				reg[R_IP] += ui_left;
			advance_ip = false;
		}
	}break;
	case BLT: {
		if ((reg[R_FLAGS] & FLAGS_REG_SIGN) != 0) {
			if (inst.flags & IMMEDIATE_OFFSET)
				reg[R_IP] += inst.immediate_offset;
			else
				reg[R_IP] += ui_left;
			advance_ip = false;
		}
	}break;
	case BGT: {
		if ((reg[R_FLAGS] & FLAGS_REG_SIGN) == 0 && !(reg[R_FLAGS] & FLAGS_REG_ZERO)) {
			if (inst.flags & IMMEDIATE_OFFSET)
				reg[R_IP] += inst.immediate_offset;
			else
				reg[R_IP] += ui_left;
			advance_ip = false;
		}
	}break;
	case BLE: {
		if ((reg[R_FLAGS] & FLAGS_REG_SIGN) != 0 || reg[R_FLAGS] & FLAGS_REG_ZERO) {
			if (inst.flags & IMMEDIATE_OFFSET)
				reg[R_IP] += inst.immediate_offset;
			else
				reg[R_IP] += ui_left;
			advance_ip = false;
		}
	}break;
	case BGE: {
		if ((reg[R_FLAGS] & FLAGS_REG_SIGN) == 0) {
			if (inst.flags & IMMEDIATE_OFFSET)
				reg[R_IP] += inst.immediate_offset;
			else
				reg[R_IP] += ui_left;
			advance_ip = false;
		}
	}break;
	case JMP: {
		advance_ip = false;
		reg[R_IP] = ui_left;
	}break;
	case MOV: {
		ui_left = ui_right;
	}break;
	case EXTCALL: {
		assert(inst.flags & IMMEDIATE_VALUE);
		// put arguments on the stack and call the function
#if _DEBUG
		call_external_DEBUG((void*)ui_left, stack, (u64)reg[R_SS]);
#else
		call_external((void*)ui_left, stack, (u64)reg[R_SS]);
#endif
	}break;
	case HLT: {
		return 1;
	}break;
	}
	
	if (write_register) {
		reg[inst.left_reg] = ui_left;
	} else if (write_memory) {
		*(T*)address_to_write = ui_left;
	}
	if (advance_ip) {
		s64 advance = REG_SIZE;
		if (inst.flags & IMMEDIATE_OFFSET) advance += sizeof(inst.immediate_offset);
		if (inst.flags & IMMEDIATE_VALUE) advance += sizeof(u64);	// @todo this should be the size of the immediate value u16 u8 s32 etc..
		reg[R_IP] += advance;
	}

	return 0;
}

int execute(Instruction inst, u64 next_word)
{
	int status = 0;
	if (inst.flags & SIGNED) {
		if (inst.flags & INSTR_BYTE)
			status = execute_instruction<s8>(inst, next_word);
		else if (inst.flags & INSTR_WORD)
			status = execute_instruction<s16>(inst, next_word);
		else if(inst.flags & INSTR_DWORD)
			status = execute_instruction<s32>(inst, next_word);
		else
			status = execute_instruction<s64>(inst, next_word);
	} else {
		if(inst.flags & INSTR_BYTE)
			status = execute_instruction<u8>(inst, next_word);
		else if(inst.flags & INSTR_WORD)
			status = execute_instruction<u16>(inst, next_word);
		else if(inst.flags & INSTR_DWORD)
			status = execute_instruction<u32>(inst, next_word);
		else
			status = execute_instruction<u64>(inst, next_word);
	}
	return status;
}

void print_instruction(Instruction inst, u64 next_qword)
{
	switch (inst.type) {
		case ADD  : printf("ADD "); break;
		case SUB  : printf("SUB "); break;
		case MUL  : printf("MUL "); break;
		case DIV  : printf("DIV "); break;
		case MOD  : printf("MOD "); break;
		case BEQ  : printf("BEQ "); break;
		case BNE  : printf("BNE "); break;
		case BLT  : printf("BLT "); break;
		case BLE  : printf("BLE "); break;
		case BGT  : printf("BGT "); break;
		case BGE  : printf("BGE "); break;
		case JMP  : printf("JMP "); break;
		case PUSH : printf("PUSH "); break;
		case POP  : printf("POP "); break;
		case CALL : printf("CALL "); break;
		case RET  : printf("RET "); break;
		case MOV  : printf("MOV "); break;
		case CMP  : printf("CMP "); break;
		case AND  : printf("AND "); break;
		case OR   : printf("OR "); break;
		case XOR  : printf("XOR "); break;
		case NOT  : printf("NOT "); break;
		case EXTCALL: printf("EXTERNAL CALL: "); break;
		case HLT  : printf("HLT "); break;
		default   : printf("UNKNOWN "); break;
	}

	switch (inst.addressing) {
	case REG_TO_REG	    : printf("R_%d, R_%d", inst.left_reg, inst.right_reg); break;
	case MEM_TO_REG     : printf("R_%d, 0x%llx", inst.left_reg, next_qword); break;
	case SINGLE_REG		: printf("R_%d", inst.left_reg); break;
	case SINGLE_MEM		: printf("%llx", next_qword); break;

	case SINGLE_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				printf("[0x%x + 0x%llx]", next_qword, inst.immediate_offset);
			else
				printf("[0x%llx]", inst.immediate_offset);
		} else if(inst.flags & REGISTER_OFFSET) {
			printf("[0x%llx + R_%d]", next_qword, inst.offset_reg);
		} else {
			printf("[0x%llx]", next_qword);
		}
	}break;
	case SINGLE_REG_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				printf("[R_%d + 0x%llx]", inst.left_reg, next_qword);
			else
				printf("[R_%d]", inst.left_reg);
		} else if (inst.flags & REGISTER_OFFSET) {
			printf("[R_%d + R_%d]", inst.left_reg, inst.offset_reg);
		} else {
			printf("[R_%d]", inst.left_reg);
		}
	}break;
	case REG_TO_REG_PTR: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("[R_%d + 0x%llx], R_%d", inst.left_reg, inst.immediate_offset, inst.right_reg);
		else if(inst.flags & REGISTER_OFFSET)
			printf("[R_%d + R_%d], R_%d", inst.left_reg, inst.offset_reg, inst.right_reg);
		else
			printf("[R_%d], R_%d", inst.left_reg, inst.right_reg);
	}break;
	case REG_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("R_%d, [R_%d + 0x%llx]", inst.left_reg, inst.right_reg, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			printf("R_%d, [R_%d + R_%d]", inst.left_reg, inst.right_reg, inst.offset_reg);
		else
			printf("R_%d, [R_%d]", inst.left_reg, inst.right_reg);
	}break;
	case MEM_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("R_%d, [0x%llx + 0x%llx]", inst.left_reg, next_qword, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			printf("R_%d, [0x%llx + R_%d]", inst.left_reg, next_qword, inst.offset_reg);
		else
			printf("R_%d, [0x%llx]", inst.left_reg, next_qword);
	}break;
	case REG_TO_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET)
			printf("[%llx + 0x%llx], R_%d", next_qword, inst.immediate_offset, inst.right_reg);
		else if(inst.flags & REGISTER_OFFSET)
			printf("[%llx + R_%d], R_%d", next_qword, inst.offset_reg, inst.right_reg);
		else
			printf("[0x%llx], R_%d", next_qword, inst.right_reg);
	}break;
	case NO_ADDRESSING: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			printf("0x%llx", inst.immediate_offset);
		}
	}break;
	}

	if (inst.flags & INSTR_BYTE) {
		printf(" byte ");
	} else if (inst.flags & INSTR_WORD) {
		printf(" word ");
	} else if (inst.flags & INSTR_DWORD) {
		printf(" dword ");
	} else if (inst.flags & INSTR_QWORD) {
		printf(" qword ");
	}

	if (inst.flags & SIGNED) {
		printf(" signed");
	} else {
		printf(" unsigned");
	}

	printf("\n");
}
