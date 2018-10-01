#include "util.h"
#include "interpreter.h"
#define internal static

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
	if (inst.flags & IMMEDIATE_OFFSET) {
		code_ptr += sizeof(Instruction);
	} else {
		code_ptr += sizeof(Instruction) - sizeof(inst.immediate_offset);
	}
	//print_instruction(inst, 0);
	return (u64)code_ptr;
}

u64 push_instruction(Instruction inst, u64 next_word) {
	push_instruction(inst);
	*(u64*)code_ptr = next_word;	// @todo this should be the size of the immediate value u16 u8 s32 etc..
	code_ptr += sizeof(next_word);
	//print_instruction(inst, next_word);
	return (u64)code_ptr;
}

u64 push_instruction(Instruction inst, u64** out_next_word) {
	push_instruction(inst);
	*out_next_word = (u64*)code_ptr;
	code_ptr += sizeof(**out_next_word);
	//print_instruction(inst, **out_next_word);
	return (u64)code_ptr;
}

u64 add_code_offset(s64 offset) {
	code_ptr += offset;
	return (u64)code_ptr;
}

u64 move_code_offset(s64 offset) {
	code_ptr = code + offset;
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

u64 get_data_segment_address()
{
	return (u64)datas;
}

void init_interpreter(s64 stack_size, s64 heap_size) 
{
	stack = (u8*)ho_bigalloc_rw(stack_size);
	heap  = (u8*)ho_bigalloc_rw(heap_size);
	code  = (u8*)ho_bigalloc_rw(1024 * 1024);
	datas = (u8*)ho_bigalloc_rw(1024 * 1024);

	printf("stack: %p\n", stack);
	printf("heap : %p\n", heap);
	printf("code : %p\n", code);
	printf("datas: %p\n", datas);

	stack = stack + 4 * sizeof(u64);	// this is necessary for external call, in order to not access an address below the stack address

	stack_ptr = stack;
	heap_ptr = heap;
	code_ptr = code;
	datas_ptr = datas;

	reg[R_IP] = (u64)code;
	reg[R_SP] = (u64)stack;
	reg[R_SB] = (u64)stack;

	u64 label = (u64)code_ptr;
	u64 start = (u64)code_ptr;

	{
		const char* str = "Hello World!\n";
		*(u64*)datas_ptr = (u64)str;
	}

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
		const char* str = "iterations : %d\n";
		*(u64*)datas_ptr = (u64)str;
	}
	{
		// mov r3, rsp
		// mov r2, 0
		// push r2
		// push [datas_ptr]
		// mov ss, 8
	// label:
		// extcall printf
		// add rsp, 16
		// add r2, 1
		// mov [r3], r2
		// cmp r2, 10
		// ble label
		// hlt

		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_3, R_SP, 0, 0));

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), (u64)0);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_2, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE|INSTR_QWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), *(u64*)datas_ptr);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 16);

		// label
		push_instruction(make_instruction(EXTCALL, INSTR_QWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)printf);	// 16

		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 16);		// 16
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 1);			// 16
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_3, R_2, 0, 0));				// 8
		push_instruction(make_instruction(CMP, IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 46);			// 16

		push_instruction(make_instruction(BLE, IMMEDIATE_OFFSET, NO_ADDRESSING, NO_REG, NO_REG, 0, -(4*16 + 8)));

		HALT;
	}
#endif
#if 0
	{
		const char* str = "Untitled - Notepad";
		const char* str2 = "Eita!";
		*(u64*)datas_ptr = (u64)str;
		datas_ptr += sizeof(u64);
		*(u64*)datas_ptr = (u64)str2;
	}
	{
		// mov r1, 0
		// push r1
		// push [datas_ptr - 8]
		// push [datas_ptr]
		// push r1
		// callext MessageBoxA
		
		// mov r1, 0
		// push r1
		// push [datas_ptr - 8]
		// callext FindWindowA

		// mov r2, rsp
		// add rsp, 16
		// hlt

		
		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE | INSTR_QWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), *(u64*)datas_ptr);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE | INSTR_QWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), *(u64*)(datas_ptr - 8));
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 4 * 8);

		push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)MessageBoxA);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE | INSTR_QWORD, MEM_TO_REG, R_1, NO_REG, 0, 0), *((u64*)(datas_ptr - 8)));
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 0);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);
		
		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 2 * 8);

		push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)FindWindowA);


		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_2, R_SP, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 16);

		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_2, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_0, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 2 * 8);

		push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)GetWindowRect);

		HALT;

	}
#endif
#if 0
	{
		// mov r1, 1
		// push r1
		// mov r1, 2
		// push r1
		// mov r1, 3
		// push r1
		// mov r1, 4
		// push r1
		// mov r1, 5
		// push r1
		// mov r1, 6
		// push r1

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 1);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 2);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 3);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 4);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 5);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 6);
		push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, R_SP, R_1, 0, 0));
		push_instruction(make_instruction(ADD, IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), 8);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 6 * 8);

		push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)teste);

		HALT;

	}
#endif
#if 0
	{
		push_instruction(make_instruction(PUSH, INSTR_DWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 1);
		push_instruction(make_instruction(PUSH, INSTR_DWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 2);

		push_instruction(make_instruction(POP, INSTR_DWORD, SINGLE_REG, R_1, NO_REG, 0, 0));

		push_instruction(make_instruction(PUSH, IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 3);
		push_instruction(make_instruction(PUSH, IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 4);
		push_instruction(make_instruction(PUSH, IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 5);
		push_instruction(make_instruction(PUSH, IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 6);

		push_instruction(make_instruction(MOV, IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 6 * 8);
		push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)teste);

		HALT;
	}
#endif
#if 0 
	{
		push_instruction(make_instruction(MOV, INSTR_BYTE | SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, R_1, NO_REG, 0, 0), 120);
		push_instruction(make_instruction(MOV, INSTR_BYTE | SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, R_2, NO_REG, 0, 0), 75);
		push_instruction(make_instruction(ADD, INSTR_BYTE | SIGNED, REG_TO_REG, R_1, R_2, 0, 0));
		push_instruction(make_instruction(PUSH, INSTR_BYTE, SINGLE_REG, R_1, NO_REG, 0, 0));
		push_instruction(make_instruction(POP, SIGNED|INSTR_BYTE, SINGLE_REG, R_2, NO_REG, 0, 0));
	}
#endif
}

#define PRINT_INSTRUCTIONS 1
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
	printf("interpreter exited with code %d\n", reg[R_0]);
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

	// addressing mode
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

	// instruction decode
	switch (inst.type) {
	case ADD:	ui_left = ui_left + ui_right; break;
	case SUB:	ui_left = ui_left - ui_right; break;
	case MUL:	ui_left = ui_left * ui_right; break;
	case DIV:	ui_left = ui_left / ui_right; break;
	case MOD:	ui_left = ui_left % ui_right; break;
	case SHL:   ui_left = ui_left << ui_right; break;
	case SHR:   ui_left = ui_left >> ui_right; break;
	case OR:    ui_left = ui_left | ui_right; break;
	case AND:   ui_left = ui_left & ui_right; break;
	case XOR:   ui_left = ui_left ^ ui_right; break;

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
	case PUSH: {
		assert(inst.addressing == SINGLE_MEM || inst.addressing == SINGLE_REG || inst.addressing == SINGLE_REG_PTR || inst.addressing == SINGLE_MEM);
		address_to_write = reg[R_SP];
		reg[R_SP] += sizeof(u64);
		write_memory = true;
	}break;
	case POP: {
		assert(inst.addressing == SINGLE_REG);
		reg[R_SP] -= sizeof(u64);
		ui_left = *(T*)reg[R_SP];
		write_register = true;
	}break;
	case EXTCALL: {
		assert(inst.flags & IMMEDIATE_VALUE);
		// put arguments on the stack and call the function
		u64 ret = call_external((void*)ui_left, (u64)reg[R_SP], (u64)reg[R_SS]);
		reg[R_0] = ret;
		reg[R_SP] -= reg[R_SS];		
	}break;
	case COPY: {
		memcpy((void*)ui_left, (void*)ui_right, next_word);
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

template <typename T>
int execute_float_instruction(Instruction inst, u64 next_word) {
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

	// instruction decode
	switch (inst.type) {
	case ADD:	ui_left = ui_left + ui_right; break;
	case SUB:	ui_left = ui_left - ui_right; break;
	case MUL:	ui_left = ui_left * ui_right; break;
	case DIV:	ui_left = ui_left / ui_right; break;

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

	case MOV: {
		ui_left = ui_right;
	}break;
	case PUSH: {
		assert(inst.addressing == SINGLE_MEM || inst.addressing == SINGLE_REG || inst.addressing == SINGLE_REG_PTR || inst.addressing == SINGLE_MEM);
		address_to_write = reg[R_SP];
		reg[R_SP] += sizeof(u64);
		write_memory = true;
	}break;
	case POP: {
		assert(inst.addressing == SINGLE_REG);
		reg[R_SP] -= sizeof(u64);
		ui_left = *(T*)reg[R_SP];
		write_register = true;
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
	if(inst.flags & INSTR_FLOAT_32) {
		status = execute_float_instruction<r32>(inst, next_word);
	} else if (inst.flags & INSTR_FLOAT_64) {
		status = execute_float_instruction<r64>(inst, next_word);
	} else if (inst.flags & SIGNED) {
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

#include <light_arena.h>
static char* reg_name(Light_Arena* arena, u8 r) {
	char n[64] = { 0 };
	int l = 0;

	switch (r) {
		case R_0:		l = sprintf(n, "R_0[0x%llx]", reg[R_0]);			break;
		case R_1:		l = sprintf(n, "R_1[0x%llx]", reg[R_1]);			break;
		case R_2:		l = sprintf(n, "R_2[0x%llx]", reg[R_2]);			break;
		case R_3:		l = sprintf(n, "R_3[0x%llx]", reg[R_3]);			break;
		case R_4:		l = sprintf(n, "R_4[0x%llx]", reg[R_4]);			break;
		case R_5:		l = sprintf(n, "R_5[0x%llx]", reg[R_5]);			break;
		case R_6:		l = sprintf(n, "R_6[0x%llx]", reg[R_6]);			break;
		case R_7:		l = sprintf(n, "R_7[0x%llx]", reg[R_7]);			break;
		case R_8:		l = sprintf(n, "R_8[0x%llx]", reg[R_8]);			break;
		case R_IP:		l = sprintf(n, "R_IP[0x%llx]", reg[R_IP]);			break;
		case R_SP:		l = sprintf(n, "R_SP[0x%llx]", reg[R_SP]);			break;
		case R_SB:		l = sprintf(n, "R_SB[0x%llx]", reg[R_SB]);			break;
		case R_SS:		l = sprintf(n, "R_SS[0x%llx]", reg[R_SS]);			break;
		case FR_0:		l = sprintf(n, "FR_0[%f]", reg[FR_0]); 				break;			
		case FR_1:		l = sprintf(n, "FR_1[%f]", reg[FR_1]); 				break;	
		case FR_2:		l = sprintf(n, "FR_2[%f]", reg[FR_2]); 				break;	
		case FR_3:		l = sprintf(n, "FR_3[%f]", reg[FR_3]); 				break;	
		case R_FLAGS:	l = sprintf(n, "R_FLAGS[0x%llx]", reg[R_FLAGS]);	break;
		case NO_REG:	l = sprintf(n, "NO_REG");							break;
		default:        l = sprintf(n, "R_UNK");							break;
	}
	n[l] = 0;

	void* result = arena_alloc(arena, l + 1);
	memcpy(result, n, l + 1);
	return (char*)result;
}

void print_instruction(Instruction inst, u64 next_qword)
{
	Light_Arena* sar = arena_create(2048);
	int l = 0;
	printf("0x%llx: ", reg[R_IP]);
	switch (inst.type) {
		case ADD  : l += printf("ADD "); break;
		case SUB  : l += printf("SUB "); break;
		case MUL  : l += printf("MUL "); break;
		case DIV  : l += printf("DIV "); break;
		case MOD  : l += printf("MOD "); break;
		case BEQ  : l += printf("BEQ "); break;
		case BNE  : l += printf("BNE "); break;
		case BLT  : l += printf("BLT "); break;
		case BLE  : l += printf("BLE "); break;
		case BGT  : l += printf("BGT "); break;
		case BGE  : l += printf("BGE "); break;
		case JMP  : l += printf("JMP "); break;
		case PUSH : l += printf("PUSH "); break;
		case POP  : l += printf("POP "); break;
		case CALL : l += printf("CALL "); break;
		case RET  : l += printf("RET "); break;
		case MOV  : l += printf("MOV "); break;
		case CMP  : l += printf("CMP "); break;
		case AND  : l += printf("AND "); break;
		case OR   : l += printf("OR "); break;
		case XOR  : l += printf("XOR "); break;
		case NOT  : l += printf("NOT "); break;
		case SHL  : l += printf("SHL "); break;
		case SHR  : l += printf("SHR "); break;
		case EXTCALL: l += printf("EXTERNAL CALL: "); break;
		case COPY : l += printf("COPY "); break;
		case HLT  : l += printf("HLT "); break;
		default   : l += printf("UNKNOWN "); break;
	}

	switch (inst.addressing) {
	case REG_TO_REG	    : l += printf("%s, %s", reg_name(sar, inst.left_reg), reg_name(sar, inst.right_reg)); break;
	case MEM_TO_REG     : l += printf("%s, 0x%llx", reg_name(sar, inst.left_reg), next_qword); break;
	case SINGLE_REG		: l += printf("%s", reg_name(sar, inst.left_reg)); break;
	case SINGLE_MEM		: l += printf("0x%llx", next_qword); break;

	case SINGLE_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				l += printf("[0x%x + 0x%llx]", next_qword, inst.immediate_offset);
			else
				l += printf("[0x%llx]", inst.immediate_offset);
		} else if(inst.flags & REGISTER_OFFSET) {
			l += printf("[0x%llx + %s]", next_qword, reg_name(sar, inst.offset_reg));
		} else {
			l += printf("[0x%llx]", next_qword);
		}
	}break;
	case SINGLE_REG_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				l += printf("[%s + 0x%llx]", reg_name(sar, inst.left_reg), next_qword);
			else
				l += printf("[%s]", reg_name(sar, inst.left_reg));
		} else if (inst.flags & REGISTER_OFFSET) {
			l += printf("[%s + %s]", reg_name(sar, inst.left_reg), reg_name(sar, inst.offset_reg));
		} else {
			l += printf("[%s]", reg_name(sar, inst.left_reg));
		}
	}break;
	case REG_TO_REG_PTR: {
		if(inst.flags & IMMEDIATE_OFFSET)
			l += printf("[%s + 0x%llx], %s", reg_name(sar, inst.left_reg), inst.immediate_offset, reg_name(sar, inst.right_reg));
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("[%s + %s], %s", reg_name(sar, inst.left_reg), reg_name(sar, inst.offset_reg), reg_name(sar, inst.right_reg));
		else
			l += printf("[%s], %s", reg_name(sar, inst.left_reg), reg_name(sar, inst.right_reg));
	}break;
	case REG_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			l += printf("%s, [%s + 0x%llx]", reg_name(sar, inst.left_reg), reg_name(sar, inst.right_reg), inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("%s, [%s + %s]", reg_name(sar, inst.left_reg), reg_name(sar, inst.right_reg), reg_name(sar, inst.offset_reg));
		else
			l += printf("%s, [%s]", reg_name(sar, inst.left_reg), reg_name(sar, inst.right_reg));
	}break;
	case MEM_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			l += printf("%s, [0x%llx + 0x%llx]", reg_name(sar, inst.left_reg), next_qword, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("%s, [0x%llx + %s]", reg_name(sar, inst.left_reg), next_qword, reg_name(sar, inst.offset_reg));
		else
			l += printf("%s, [0x%llx]", reg_name(sar, inst.left_reg), next_qword);
	}break;
	case REG_TO_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET)
			l += printf("[%llx + 0x%llx], %s", next_qword, inst.immediate_offset, reg_name(sar, inst.right_reg));
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("[%llx + %s], %s", next_qword, reg_name(sar, inst.offset_reg), reg_name(sar, inst.right_reg));
		else
			l += printf("[0x%llx], %s", next_qword, reg_name(sar, inst.right_reg));
	}break;
	case NO_ADDRESSING: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			l += printf("0x%llx", inst.immediate_offset);
		}
	}break;
	}

	while (l < 35) {
		++l;
		printf(" ");
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

	arena_free(sar);
}
