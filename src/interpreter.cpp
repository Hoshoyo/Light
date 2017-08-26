#include "util.h"
#include "ho_system.h"
#define internal static
#define FLAG(X) (1 << X);
#define REG_SIZE 8

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

	R_FLAGS = 11,

	NO_REG = 12,
	NUM_REGS = 13,
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

void print_instruction(u64 instruction, u64 next_qword);
int execute(u64 instruction, u64 next_word);

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
		*(u64*)datas_ptr = (u64)-(8 * 4); datas_ptr += sizeof(u64);
	}

	{
		// IMMEDIATE_OFFSET | REGISTER_OFFSET | SIGNED | IMMEDIATE_VALUE
		u64 label = (u64)code_ptr;
		u64 start = (u64)code_ptr;

		// mov r2, datas_ptr
		// mov r1, 5
		// sub r1, 1
		// extcall print
		// cmp r1, 0
		// bge [r2]
		// hlt
		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_2, NO_REG, 0, 0), (u64)datas_ptr - 8);		// --¬
		start = push_instruction(make_instruction(MOV, IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), 5);					//    |
		push_instruction(make_instruction(SUB, SIGNED | IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_1, NO_REG, NO_REG, 0), 1);			// <--
		label = push_instruction(make_instruction(CMP, SIGNED | IMMEDIATE_VALUE | INSTR_DWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(BGE, SIGNED, SINGLE_REG_PTR, R_2, NO_REG, 0, 0));
		push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));
		
	}
}

#define PRINT_INSTRUCTIONS 1
int run_interpreter()
{
	running = true;	
	while (running) {
		u64 address = reg[R_IP];
		u64 instruction = *(u64*)reg[R_IP];
		u64 immediate = *((u64*)(address + REG_SIZE));

#ifdef PRINT_INSTRUCTIONS
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
		// put arguments on the stack
		// --
		// call
		void(*func)() = (void(*)())ui_left;
		func();
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

int execute(u64 instruction, u64 next_word)
{
	Instruction inst = *(Instruction*)&instruction;
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

void print_instruction(u64 instruction, u64 next_qword)
{
	Instruction inst = *(Instruction*)&instruction;
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
	case MEM_TO_REG     : printf("R_%d, 0x%x", inst.left_reg, next_qword); break;
	case SINGLE_REG		: printf("R_%d", inst.left_reg); break;
	case SINGLE_MEM		: printf("%x", next_qword); break;

	case SINGLE_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				printf("[0x%x + 0x%x]", next_qword, inst.immediate_offset);
			else
				printf("[0x%x]", inst.immediate_offset);
		} else if(inst.flags & REGISTER_OFFSET) {
			printf("[0x%x + R_%d]", next_qword, inst.offset_reg);
		} else {
			printf("[0x%x]", next_qword);
		}
	}break;
	case SINGLE_REG_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				printf("[R_%d + 0x%x]", inst.left_reg, next_qword);
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
			printf("[R_%d + 0x%x], R_%d", inst.left_reg, inst.immediate_offset, inst.right_reg);
		else if(inst.flags & REGISTER_OFFSET)
			printf("[R_%d + R_%d], R_%d", inst.left_reg, inst.offset_reg, inst.right_reg);
		else
			printf("[R_%d], R_%d", inst.left_reg, inst.right_reg);
	}break;
	case REG_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("R_%d, [R_%d + 0x%x]", inst.left_reg, inst.right_reg, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			printf("R_%d, [R_%d + R_%d]", inst.left_reg, inst.right_reg, inst.offset_reg);
		else
			printf("R_%d, [R_%d]", inst.left_reg, inst.right_reg);
	}break;
	case MEM_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			printf("R_%d, [0x%x + 0x%x]", inst.left_reg, next_qword, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			printf("R_%d, [0x%x + R_%d]", inst.left_reg, next_qword, inst.offset_reg);
		else
			printf("R_%d, [0x%x]", inst.left_reg, next_qword);
	}break;
	case REG_TO_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET)
			printf("[%x + 0x%x], R_%d", next_qword, inst.immediate_offset, inst.right_reg);
		else if(inst.flags & REGISTER_OFFSET)
			printf("[%x + R_%d], R_%d", next_qword, inst.offset_reg, inst.right_reg);
		else
			printf("[0x%x], R_%d", next_qword, inst.right_reg);
	}break;
	case NO_ADDRESSING: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			printf("0x%x", inst.immediate_offset);
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
