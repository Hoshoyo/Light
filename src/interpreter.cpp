#include "util.h"
#include "interpreter.h"
#define internal static

#define PRINT_INSTRUCTIONS 0

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define RUNTIME_BYTECODE_ASSERT_FAIL(X) MessageBoxA(0, X, "Assert Failed", MB_ICONERROR)
#elif defined(__linux__)
#define RUNTIME_BYTECODE_ASSERT_FAIL(X) printf(X)
#endif

void print_instruction(Interpreter* interp, Instruction instruction, u64 next_qword);
void print_code(Interpreter* interp);
int  execute(Interpreter* interp, Instruction instruction, u64 next_word);

u64 push_instruction(Interpreter* interp, Instruction inst) {
	*(Instruction*)interp->code_ptr = inst;
	if (inst.flags & IMMEDIATE_OFFSET) {
		interp->code_ptr += sizeof(Instruction);
	} else {
		interp->code_ptr += sizeof(Instruction) - sizeof(inst.immediate_offset);
	}
	print_instruction(interp, inst, 0);
	return (u64)interp->code_ptr;
}

u64 push_instruction(Interpreter* interp, Instruction inst, u64 next_word) {
	push_instruction(interp, inst);
	*(u64*)interp->code_ptr = next_word;	// @todo this should be the size of the immediate value u16 u8 s32 etc..
	interp->code_ptr += sizeof(next_word);
	//print_instruction(inst, next_word);
	return (u64)interp->code_ptr;
}

u64 push_instruction(Interpreter* interp, Instruction inst, u64** out_next_word) {
	push_instruction(interp, inst);
	*out_next_word = (u64*)interp->code_ptr;
	interp->code_ptr += sizeof(**out_next_word);
	//print_instruction(inst, **out_next_word);
	return (u64)interp->code_ptr;
}

u64 add_code_offset(Interpreter* interp, s64 offset) {
	interp->code_ptr += offset;
	return (u64)interp->code_ptr;
}

u64 move_code_offset(Interpreter* interp, s64 offset) {
	interp->code_ptr = interp->code + offset;
	return (u64)interp->code_ptr;
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

Instruction make_instruction(u16 type, u16 flags, u8 addressing, u8 left_reg, u8 right_reg, u8 offset_reg, s32** immediate_offset) {
	Instruction res;
	res.type = type;
	res.flags = flags;
	res.addressing = addressing;
	res.left_reg = left_reg;
	res.right_reg = right_reg;
	res.offset_reg = offset_reg;
	*immediate_offset = (s32*)&res.immediate_offset;
	return res;
}

Interpreter init_interpreter(s64 stack_size, s64 heap_size) 
{
	Interpreter interp = {0};

	interp.stack = (u8*)ho_bigalloc_rw(stack_size);
	interp.heap  = (u8*)ho_bigalloc_rw(heap_size);
	interp.code  = (u8*)ho_bigalloc_rw(1024 * 1024);
	interp.datas = (u8*)ho_bigalloc_rw(1024 * 1024);

#if DEBUG
	printf("stack: %p\n", interp.stack);
	printf("heap : %p\n", interp.heap);
	printf("code : %p\n", interp.code);
	printf("datas: %p\n", interp.datas);
#endif

	interp.stack = interp.stack + 4 * sizeof(u64);	// this is necessary for external call, in order to not access an address below the stack address

	interp.stack_ptr = interp.stack;
	interp.heap_ptr = interp.heap;
	interp.code_ptr = interp.code;
	interp.datas_ptr = interp.datas;

	interp.reg[R_IP] = (u64)interp.code;
	interp.reg[R_SP] = (u64)interp.stack;
	interp.reg[R_SB] = (u64)interp.stack;

	return interp;
}

int run_interpreter(Interpreter* interp)
{
	//print_code(interp);

	interp->running = true;
	int instruction_num = 0;
	while (interp->running) {
		u64 address = interp->reg[R_IP];
		Instruction instruction = *(Instruction*)interp->reg[R_IP];
		if(instruction.flags & IMMEDIATE_OFFSET){
			instruction.immediate_offset = *((u64*)(address + REG_SIZE));
			address += REG_SIZE;
		}
		u64 immediate = *((u64*)(address + REG_SIZE));
		instruction_num++;
#if PRINT_INSTRUCTIONS
		print_instruction(interp, instruction, immediate);
#endif
		if (execute(interp, instruction, immediate)) break;
	}
#if 1
	printf("interpreter exited with code %d\n", interp->reg[R_0]);
#endif
	return 0;
}

template <typename T>
int execute_instruction(Interpreter* interp, Instruction inst, u64 next_word)
{	
	s64* reg = interp->reg;
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
		write_register = true;
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

	if (inst.flags & INVERT_OPS) {
		T temp = ui_left;
		ui_left = ui_right;
		ui_right = temp;
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
	case NOT:   ui_left = ~ui_left; break;

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
	case EQ: {
		ui_left = reg[R_FLAGS] & FLAGS_REG_ZERO;
		write_register = true;
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
	case NE:{
		ui_left = !(reg[R_FLAGS] & FLAGS_REG_ZERO);
		write_register = true;
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
	case LT:{
		ui_left = (reg[R_FLAGS] & FLAGS_REG_SIGN) != 0;
		write_register = true;
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
	case GT:{
		ui_left = (reg[R_FLAGS] & FLAGS_REG_SIGN) == 0 && !(reg[R_FLAGS] & FLAGS_REG_ZERO);
		write_register = true;
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
	case LE:{
		ui_left = (reg[R_FLAGS] & FLAGS_REG_SIGN) != 0 || reg[R_FLAGS] & FLAGS_REG_ZERO;
		write_register = true;
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
	case GE:{
		ui_left = (reg[R_FLAGS] & FLAGS_REG_SIGN) == 0;
		write_register = true;
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
		reg[R_SP] += sizeof(ui_left);
		write_memory = true;
	}break;
	case POP: {
		assert(inst.addressing == SINGLE_REG);
		reg[R_SP] -= sizeof(ui_left);
		ui_left = *(T*)reg[R_SP];
		write_register = true;
	}break;
	case EXTCALL: {
		//assert(inst.flags & IMMEDIATE_VALUE);
		// put arguments on the stack and call the function
		u64 ret = call_external((void*)ui_left, (u64)reg[R_SP], (u64)reg[R_SS]);
		reg[R_0] = ret;
		reg[R_SP] -= reg[R_SS];		
	}break;
	case COPY: {
		memcpy((void*)ui_left, (void*)ui_right, next_word);
	}break;
	case ASSERT: {
		if (ui_left == 0) {
			RUNTIME_BYTECODE_ASSERT_FAIL("Array bounds check failed");
			exit(-1);
		}
	}break;
	case HLT: {
		return 1;
	}break;
	}
	
	if (write_register) {
		reg[inst.left_reg] = ui_left;
	}
	if (write_memory) {
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
int execute_float_instruction(Interpreter* interp, Instruction inst, u64 next_word) {
	s64* reg = interp->reg;
	T ui_left, ui_right;
	bool write_memory = false;
	bool write_register = false;
	bool advance_ip = true;
	u64 address_to_write = 0;

	switch (inst.addressing) {
	case REG_TO_REG: {
		ui_left = *(T*)&reg[inst.left_reg];
		ui_right = *(T*)&reg[inst.right_reg];
		write_register = true;
	}break;
	case MEM_TO_REG: {
		ui_left = *(T*)&reg[inst.left_reg];
		ui_right = (T)(*(r64*)&next_word);
		write_register = true;
	}break;
	case SINGLE_REG: {
		ui_left = *(T*)&reg[inst.left_reg];
		write_register = true;
	}break;
	case SINGLE_MEM: {
		ui_left = (T)(*(r64*)&next_word);
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
		ui_right = *(T*)&reg[inst.right_reg];
		write_memory = true;
	}break;
	case REG_PTR_TO_REG: {
		ui_left = *(T*)&reg[inst.left_reg];
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
		if(inst.flags & INSTR_FLOAT_32) {
			reg[inst.left_reg] = (s32)*(u32*)&ui_left;
		} else {
			reg[inst.left_reg] = (s64)*(s64*)&ui_left;
		}
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

int execute(Interpreter* interp, Instruction inst, u64 next_word)
{
	int status = 0;
	if(inst.flags & INSTR_FLOAT_32) {
		status = execute_float_instruction<r32>(interp, inst, next_word);
	} else if (inst.flags & INSTR_FLOAT_64) {
		status = execute_float_instruction<r64>(interp, inst, next_word);
	} else if (inst.flags & SIGNED) {
		if (inst.flags & INSTR_BYTE)
			status = execute_instruction<s8>(interp, inst, next_word);
		else if (inst.flags & INSTR_WORD)
			status = execute_instruction<s16>(interp, inst, next_word);
		else if(inst.flags & INSTR_DWORD)
			status = execute_instruction<s32>(interp, inst, next_word);
		else
			status = execute_instruction<s64>(interp, inst, next_word);
	} else {
		if(inst.flags & INSTR_BYTE)
			status = execute_instruction<u8>(interp, inst, next_word);
		else if(inst.flags & INSTR_WORD)
			status = execute_instruction<u16>(interp, inst, next_word);
		else if(inst.flags & INSTR_DWORD)
			status = execute_instruction<u32>(interp, inst, next_word);
		else
			status = execute_instruction<u64>(interp, inst, next_word);
	}
	return status;
}

#include <light_arena.h>
static char* reg_name(Interpreter* interp, Light_Arena* arena, u8 r) {
	s64* reg = interp->reg;
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
		case FR_0:		l = sprintf(n, "FR_0[%f]", *(r32*)&reg[FR_0]); 				break;			
		case FR_1:		l = sprintf(n, "FR_1[%f]", *(r32*)&reg[FR_1]); 				break;	
		case FR_2:		l = sprintf(n, "FR_2[%f]", *(r32*)&reg[FR_2]); 				break;	
		case FR_3:		l = sprintf(n, "FR_3[%f]", *(r32*)&reg[FR_3]); 				break;	
		case R_FLAGS:	l = sprintf(n, "R_FLAGS[0x%llx]", reg[R_FLAGS]);	break;
		case NO_REG:	l = sprintf(n, "NO_REG");							break;
		default:        l = sprintf(n, "R_UNK");							break;
	}
	n[l] = 0;

	void* result = arena_alloc(arena, l + 1);
	memcpy(result, n, l + 1);
	return (char*)result;
}

inline s32 instruction_size(Instruction inst) {
	s32 size = sizeof(Instruction) - sizeof(s64);
	if (inst.flags & IMMEDIATE_OFFSET) size += sizeof(s64);
	if (inst.flags & IMMEDIATE_VALUE) size += sizeof(u64);
	return size;
}

void print_instruction(Interpreter* interp, Instruction inst, u64 next_qword)
{
	s64* reg = interp->reg;
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
		case EQ   : l += printf("EQ "); break; 
		case NE   : l += printf("NE "); break;
		case LT   : l += printf("LT "); break;
		case GT   : l += printf("GT "); break;
		case LE   : l += printf("LE "); break;
		case GE   : l += printf("GE "); break;
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
	case REG_TO_REG	    : l += printf("%s, %s", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.right_reg)); break;
	case MEM_TO_REG     : l += printf("%s, 0x%llx", reg_name(interp, sar, inst.left_reg), next_qword); break;
	case SINGLE_REG		: l += printf("%s", reg_name(interp, sar, inst.left_reg)); break;
	case SINGLE_MEM		: l += printf("0x%llx", next_qword); break;

	case SINGLE_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				l += printf("[0x%x + 0x%llx]", next_qword, inst.immediate_offset);
			else
				l += printf("[0x%llx]", inst.immediate_offset);
		} else if(inst.flags & REGISTER_OFFSET) {
			l += printf("[0x%llx + %s]", next_qword, reg_name(interp, sar, inst.offset_reg));
		} else {
			l += printf("[0x%llx]", next_qword);
		}
	}break;
	case SINGLE_REG_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			if (inst.flags & IMMEDIATE_VALUE)
				l += printf("[%s + 0x%llx]", reg_name(interp, sar, inst.left_reg), next_qword);
			else
				l += printf("[%s]", reg_name(interp, sar, inst.left_reg));
		} else if (inst.flags & REGISTER_OFFSET) {
			l += printf("[%s + %s]", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.offset_reg));
		} else {
			l += printf("[%s]", reg_name(interp, sar, inst.left_reg));
		}
	}break;
	case REG_TO_REG_PTR: {
		if(inst.flags & IMMEDIATE_OFFSET)
			l += printf("[%s + 0x%llx], %s", reg_name(interp, sar, inst.left_reg), inst.immediate_offset, reg_name(interp, sar, inst.right_reg));
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("[%s + %s], %s", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.offset_reg), reg_name(interp, sar, inst.right_reg));
		else
			l += printf("[%s], %s", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.right_reg));
	}break;
	case REG_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			l += printf("%s, [%s + 0x%llx]", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.right_reg), inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("%s, [%s + %s]", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.right_reg), reg_name(interp, sar, inst.offset_reg));
		else
			l += printf("%s, [%s]", reg_name(interp, sar, inst.left_reg), reg_name(interp, sar, inst.right_reg));
	}break;
	case MEM_PTR_TO_REG: {
		if(inst.flags & IMMEDIATE_OFFSET)
			l += printf("%s, [0x%llx + 0x%llx]", reg_name(interp, sar, inst.left_reg), next_qword, inst.immediate_offset);
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("%s, [0x%llx + %s]", reg_name(interp, sar, inst.left_reg), next_qword, reg_name(interp, sar, inst.offset_reg));
		else
			l += printf("%s, [0x%llx]", reg_name(interp, sar, inst.left_reg), next_qword);
	}break;
	case REG_TO_MEM_PTR: {
		if (inst.flags & IMMEDIATE_OFFSET)
			l += printf("[%llx + 0x%llx], %s", next_qword, inst.immediate_offset, reg_name(interp, sar, inst.right_reg));
		else if(inst.flags & REGISTER_OFFSET)
			l += printf("[%llx + %s], %s", next_qword, reg_name(interp, sar, inst.offset_reg), reg_name(interp, sar, inst.right_reg));
		else
			l += printf("[0x%llx], %s", next_qword, reg_name(interp, sar, inst.right_reg));
	}break;
	case NO_ADDRESSING: {
		if (inst.flags & IMMEDIATE_OFFSET) {
			l += printf("0x%llx", inst.immediate_offset);
		}
	}break;
	}

	while (l < 50) {
		++l;
		printf(" ");
	}

	s32 isize = instruction_size(inst);

	if (inst.flags & INSTR_BYTE) {
		printf(" byte ");
	} else if (inst.flags & INSTR_WORD) {
		printf(" word ");
	} else if (inst.flags & INSTR_DWORD) {
		printf(" dword ");
	} else if (inst.flags & INSTR_QWORD) {
		printf(" qword ");
	} else {
		printf("       ");
	}

	if (inst.flags & SIGNED) {
		printf(" signed   %d", isize);
	} else {
		printf(" unsigned %d", isize);
	}

	printf("\n");

	arena_free(sar);
}

void print_code(Interpreter* interp) {
	u8* codeptr = interp->code;
	s64 start_ip = interp->reg[R_IP];
	while (true) {
		u64 address = (u64)codeptr;
		Instruction instruction = *(Instruction*)address;
		if (instruction.flags & IMMEDIATE_OFFSET) {
			instruction.immediate_offset = *((u64*)(address + REG_SIZE));
			address += REG_SIZE;
		}
		u64 immediate = *((u64*)(address + REG_SIZE));

		interp->reg[R_IP] = address;
		print_instruction(interp, instruction, immediate);

		if (instruction.type == 0) {
			break;
		}

		codeptr += sizeof(Instruction) - sizeof(s64);

		if (instruction.flags & IMMEDIATE_OFFSET) {
			codeptr += sizeof(s64);
		}
		if (instruction.flags & IMMEDIATE_VALUE) {
			codeptr += sizeof(u64);
		}
	}
	interp->reg[R_IP] = start_ip;
	printf("\n");
}
