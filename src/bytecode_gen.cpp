#include "bytecode_gen.h"
#include <ho_system.h>
#include "interpreter.h"
#include "decl_check.h"
#include "type.h"
#include "type_table.h"

#if defined(_WIN64)
void* load_address_of_external_function(string* name, HMODULE library);
HMODULE load_library_dynamic(string* library);
#elif defined(__linux__)
typedef void* HMODULE;
#include <dlfcn.h>
void* load_address_of_external_function(string* name, void* library);
void* load_library_dynamic(string* library);
#endif

/*
	*********************************************
	*********** Register Allocation *************
	*********************************************
*/

struct Register_Allocation{
	bool allocated;
	bool in_stack;
	s32  age;
};
// TODO(psv): thread safety
static Register_Allocation register_allocated[NUM_REGS];
static Registers* register_allocation_stack;
static Registers* register_saved_stack;

u32 instruction_regsize_from_type(Type_Instance* type) {
	switch (type->kind) {
	case KIND_PRIMITIVE: {
		switch (type->primitive) {
		case TYPE_PRIMITIVE_BOOL:
			return INSTR_DWORD;
		case TYPE_PRIMITIVE_S8:
		case TYPE_PRIMITIVE_U8:
			return INSTR_BYTE;
		case TYPE_PRIMITIVE_S16:
		case TYPE_PRIMITIVE_U16:
			return INSTR_WORD;
		case TYPE_PRIMITIVE_S32:
		case TYPE_PRIMITIVE_U32:
			return INSTR_DWORD;
		case TYPE_PRIMITIVE_R32:
			return INSTR_FLOAT_32;
		case TYPE_PRIMITIVE_S64:
		case TYPE_PRIMITIVE_U64:
			return INSTR_QWORD;
		case TYPE_PRIMITIVE_R64:
			return INSTR_FLOAT_64;
		}
	}break;
	case KIND_POINTER:
		return INSTR_QWORD;
	}
	return 0;
}

// The oldest register, the one allocated farther back gets allocated in
// case of all allocated.
Registers reg_allocate(Gen_Environment* env, bool floating_point = false) {
	s32 oldest = -1;
	Registers result = NO_REG;

	Registers r_first = (floating_point) ? FR_1 : R_1;
	Registers r_last  = (floating_point) ? FR_3 : R_8;

	for (int i = r_first; i <= r_last; ++i) {
		if (!register_allocated[i].allocated) {
			register_allocated[i].allocated = true;
			register_allocated[i].age = 0;
			result = (Registers)i;
			break;
		}
		if (register_allocated[i].age > oldest) {
			oldest = i;
		}
		register_allocated[i].age++;
	}
	if (result != NO_REG) return result;

	// push the oldest to the stack
	if (!register_allocation_stack) {
		register_allocation_stack = array_create(Registers, 512);
	}
	register_allocated[oldest].in_stack = true;
	register_allocated[oldest].age++;
	array_push(register_allocation_stack, (Registers*)&oldest);

	env->code_offset += push_instruction(env->interp, make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, oldest, NO_REG, 0, 0));

	return (Registers)oldest;
}

// bool indicates if the caller needs to generate pop call for this register
void reg_free(Gen_Environment* env, Registers r) {
	bool pop = false;
	// free the last in the stack rather
	if (register_allocated[r].in_stack) {
		Registers last = *(Registers*)array_pop(register_allocation_stack);
		assert(last == r);
		pop = true;
	}
	register_allocated[r].allocated = false;
	register_allocated[r].in_stack = false;
	register_allocated[r].age = 0;

	if (pop) {
		env->code_offset += push_instruction(env->interp, make_instruction(POP, INSTR_QWORD, SINGLE_REG, r, NO_REG, 0, 0));
	}
}

/*
	********************************************
*/

struct Instruction_Info {
	s64 offset;
	s64 size_bytes;
	u8* absolute_address;
};

#define PRINT_INSTRUCTIONS 0
Instruction_Info
Push(Gen_Environment* env, Instruction inst, u64 next_word = 0) {
	Instruction_Info result = {0};
	result.absolute_address = env->code + env->code_offset;
	result.offset = env->code_offset;

	*(Instruction*)(env->code + env->code_offset) = inst;
	env->code_offset += sizeof(Instruction);

	if (!(inst.flags & IMMEDIATE_OFFSET)) {
		env->code_offset -= sizeof(s64);
	}

	if(inst.flags & IMMEDIATE_VALUE) {
		*(u64*)(env->code + env->code_offset) = next_word;
		env->code_offset += sizeof(u64);
	}

	result.size_bytes = env->code_offset - result.offset;
#if PRINT_INSTRUCTIONS
	print_instruction(env->interp, inst, next_word);
#endif
	return result;
}

Instruction_Info
Push(Gen_Environment* env, Instruction inst, u64** next_word_address) {
	Instruction_Info ii = Push(env, inst, (u64)0);
	*next_word_address = (u64*)(env->code + ii.offset + sizeof(u64));
	return ii;
}

/*
	********************************************
*/

const u32 EXPR_RESULT_ON_STACK = FLAG(0);
const u32 EXPR_RESULT_ON_REGISTER = FLAG(1);
const u32 EXPR_RESULT_EXT_CALL = FLAG(2);
struct Expr_Generation {
	s64 result_offset_from_sb;
	Registers reg;
	u32 flags;
};

Expr_Generation gen_code_for_expression(Gen_Environment* env, Ast* expr, s64 stack_position = 0, bool address_of = false);

void
PushRegisters(Gen_Environment* env) {
	for (size_t i = 0; i < NUM_REGS; ++i) {
		if (register_allocated[i].allocated) {
			if (!register_allocation_stack) {
				register_saved_stack = array_create(Registers, 512);
			}
			array_push(register_saved_stack, &i);
			Push(env, make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, i, NO_REG, 0, 0));
		}
	}
}

void PopRegisters(Gen_Environment* env) {
	if (!register_saved_stack) return;
	while(array_get_length(register_saved_stack) > 0) {
		Registers r = *(Registers*)array_pop(register_saved_stack);
		Push(env, make_instruction(POP, INSTR_QWORD, SINGLE_REG, r, NO_REG, 0, 0));
	}
}

void
generate_entry_point(Gen_Environment* env) {
	//   push sb				; 8  bytes
	//   push end				; 16 bytes
	//   mov ss, 16				; 16 bytes
	//   jmp main				; 16 bytes
	// end:
	//   hlt					; 8 bytes
	// main:

	env->proc_addressing_queue = array_create(Gen_Proc_Addresses, 16);

	u64* hlt_fill_relative = 0;
	u64* main_fill_relative = 0;
	Instruction_Info last = {0};
	Instruction_Info jmp_main = {0};

	Push(env, make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	Push(env, make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), &hlt_fill_relative);
	Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 2 * 8);
	jmp_main = Push(env, make_instruction(JMP, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), &main_fill_relative);
	last =     Push(env, make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));

	*hlt_fill_relative = (u64)last.absolute_address;
	*main_fill_relative = (u64)(env->code + env->code_offset);
}

void
generate_proc_prologue(Gen_Environment* env, Ast* proc_body) {
	// push sb
	// mov	sb, sp
	// add	sp, stack_size

	// calculate stack size from proc body
	Scope* scope = proc_body->decl_procedure.body->comm_block.block_scope;
	s32 num_decls = scope->decl_count;

	env->stack_size = scope->stack_allocation_offset;
	env->stack_base_offset = 0;
	env->stack_temp_offset = 0;

	Push(env, make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	Push(env, make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_SB, R_SP, 0, 0));
	Push(env, make_instruction(ADD, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), env->stack_size);
}

void
generate_proc_epilogue(Gen_Environment* env) {
	// mov	sp, sb
	// pop	sb
	// pop	ss		; return address
	// jmp	ss		; return from call

	Push(env, make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_SP, R_SB, 0, 0));
	Push(env, make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	Push(env, make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));
	Push(env, make_instruction(JMP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));
}

Expr_Generation generate_code_expr_procedure(Gen_Environment* env, Ast* expr) {
	Expr_Generation result = { 0 };
	Ast* decl = expr->expr_variable.decl;
	assert(decl && decl->node_type == AST_DECL_PROCEDURE);

	Registers r = reg_allocate(env, type_primitive_float(expr->type_return));
	
	if (decl->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) {
		HMODULE lib = load_library_dynamic(&decl->decl_procedure.extern_library_name->value);
		void* address = load_address_of_external_function(&decl->decl_procedure.name->value, lib);
		Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, r, NO_REG, 0, 0), (u64)address);
		result.flags |= EXPR_RESULT_EXT_CALL;
	} else {
		// procedure
		Instruction_Info ii = Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, r, NO_REG, 0, 0), (u64)0xcccccccccccccccc);
		// queue to fill address
		Gen_Proc_Addresses pfill;
		pfill.decl = decl;
		pfill.fill_address = (u64*)(ii.absolute_address + sizeof(Instruction) - sizeof(s64));
		array_push(env->proc_addressing_queue, &pfill);
	}

	result.flags |= EXPR_RESULT_ON_REGISTER;
	result.reg = r;
	return result;
}

Expr_Generation generate_code_expr_variable_address(Gen_Environment* env, Ast* expr, s64 stack_position) {
	Expr_Generation result = { 0 };
	Ast* decl = expr->expr_variable.decl;
	assert(decl);

	// variable
	if (decl->decl_variable.flags & DECL_VARIABLE_STACK) {
		s32 stack_offset = decl->decl_variable.stack_offset;

		Registers r = reg_allocate(env, type_primitive_float(expr->type_return));

		// MOV R_X, R_SB
		Push(env, make_instruction(MOV, INSTR_QWORD, REG_TO_REG, r, R_SB, NO_REG, 0));

		if (decl->decl_variable.stack_offset > 0) {
			// ADD R_X, stack_offset
			Push(env, make_instruction(ADD, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, r, NO_REG, 0, 0), decl->decl_variable.stack_offset);
		}
		result.flags |= EXPR_RESULT_ON_REGISTER;
		result.reg = r;
	} else {
		// TODO(psv): get variable in data segment
		assert_msg(0, "not implemented");
	}

	return result;
}

Expr_Generation generate_code_expr_variable_value(Gen_Environment* env, Ast* expr, s64 stack_position) {
	Expr_Generation result = { 0 };
	Ast* decl = expr->expr_variable.decl;
	assert(decl);

	u32 instruction_regsize = instruction_regsize_from_type(expr->type_return);
	s32 stack_offset = decl->decl_variable.stack_offset;
	Registers r = reg_allocate(env, type_primitive_float(expr->type_return));

	if (decl->decl_variable.flags & DECL_VARIABLE_STACK) {
		// When the type is of register size, allocate a register and put the result in it
		if (type_regsize(expr->type_return)) {
			// mov rx, [sb + offset]
			Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_OFFSET | instruction_regsize, REG_PTR_TO_REG, r, R_SB, NO_REG, decl->decl_variable.stack_offset));
			result.flags |= EXPR_RESULT_ON_REGISTER;
			result.reg = r;
		} else {
			// COPY from variable stack offset to stack_position
			if (decl->decl_variable.stack_offset != stack_position) {
				// MOV R_X, R_SB
				Push(env, make_instruction(MOV, INSTR_QWORD, REG_TO_REG, r, R_SB, NO_REG, 0));
				// ADD R_X, stack_offset
				Push(env, make_instruction(ADD, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, r, NO_REG, 0, 0), decl->decl_variable.stack_offset);
				// COPY [R_SB + stack_position], R_X
				Push(env, make_instruction(COPY, IMMEDIATE_OFFSET | IMMEDIATE_VALUE, REG_TO_REG, R_SP, r, NO_REG, 0), stack_position);
			} else {
				// the variable is already where it is supposed to be ...
			}
			result.flags |= EXPR_RESULT_ON_STACK;
			result.result_offset_from_sb = stack_position;
		}
	} else {
		// TODO(psv): get variable in data segment
		assert_msg(0, "not implemented");
	}

	return result;
}

Expr_Generation generate_binary_expression(Gen_Environment* env, Ast* expr, s64 stack_position, bool address_of) {
	Expr_Generation result = { 0 };

	bool left_address_of = (expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS) ? true : address_of;

	// Calculate both sides
	Expr_Generation left = gen_code_for_expression(env, expr->expr_binary.left, stack_position, left_address_of);
	Expr_Generation right = gen_code_for_expression(env, expr->expr_binary.right, stack_position, false);
	u16 instruction = 0;

	u16 flags = instruction_regsize_from_type(expr->type_return);
	flags |= (type_primitive_int_signed(expr->type_return)) ? SIGNED : 0;

	bool comparison = false;

	switch (expr->expr_binary.op) {
		case OP_BINARY_PLUS:			instruction = ADD; break; // +
		case OP_BINARY_MINUS:			instruction = SUB; break; // -
		case OP_BINARY_MULT:			instruction = MUL; break; // *
		case OP_BINARY_DIV:				instruction = DIV; break; // /
		case OP_BINARY_MOD:				instruction = MOD; break; // %
		case OP_BINARY_AND:				instruction = AND; break; // &
		case OP_BINARY_OR:				instruction = OR;  break; // |
		case OP_BINARY_XOR:				instruction = XOR; break; // ^
		case OP_BINARY_SHL:				instruction = SHL; break; // <<
		case OP_BINARY_SHR:				instruction = SHR; break; // >>

		case OP_BINARY_LT:				instruction = LT; comparison = true; break; // <
		case OP_BINARY_GT:				instruction = GT; comparison = true; break; // >
		case OP_BINARY_LE:				instruction = LE; comparison = true; break; // <=
		case OP_BINARY_GE:				instruction = GE; comparison = true; break; // >=
		case OP_BINARY_EQUAL:			instruction = EQ; comparison = true; break; // ==
		case OP_BINARY_NOT_EQUAL:		instruction = NE; comparison = true; break; // !=

		case OP_BINARY_LOGIC_AND:		instruction = AND; break; // &&
		case OP_BINARY_LOGIC_OR:		instruction = OR; break;  // ||

																  // TODO(psv): dot and vector acessing operators
		case OP_BINARY_DOT:				assert_msg(0, "unimplemented bytecode generation for dot operator"); break; // .
		case OP_BINARY_VECTOR_ACCESS:	// []
		{
			assert(right.flags & EXPR_RESULT_ON_REGISTER); // because is an integer expression
			assert(left.flags & EXPR_RESULT_ON_REGISTER);  // because is an lvalue address

			// TODO(psv): copy when not register size
			// left side is address of where to put, right side is the offset
			size_t type_size = expr->type_return->type_size_bits / 8;
			if (address_of) {
				// calculate address only
				if (type_size >= 1) {
					Push(env, make_instruction(MUL, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, right.reg, NO_REG, 0, 0), (u64)type_size);
					Push(env, make_instruction(ADD, INSTR_QWORD, REG_TO_REG, left.reg, right.reg, 0, 0));
				}
			} else {
				// multiply by size of type to get the right offset
				Push(env, make_instruction(MUL, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, right.reg, NO_REG, 0, 0), (u64)type_size);

				if (expr->expr_binary.left->type_return->kind == KIND_ARRAY) {
					// Array bounds check
					Registers abc = reg_allocate(env);
					Push(env, make_instruction(MOV, INSTR_QWORD, REG_TO_REG, abc, right.reg, 0, 0));
					Push(env, make_instruction(CMP, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, abc, NO_REG, 0, 0), (u64)type_size * expr->expr_binary.left->type_return->array_desc.dimension);
					Push(env, make_instruction(LT, INSTR_QWORD, SINGLE_REG, abc, NO_REG, 0, 0));
					Push(env, make_instruction(ASSERT, INSTR_QWORD, SINGLE_REG, abc, NO_REG, 0, 0));
				}

				// get value in address using offset
				// mov L, [L + R]
				Push(env, make_instruction(MOV, flags | REGISTER_OFFSET, REG_PTR_TO_REG, left.reg, left.reg, right.reg, 0));
			}
			result.reg = left.reg;
			result.flags |= EXPR_RESULT_ON_REGISTER;
			reg_free(env, right.reg);

			return result;
		} break;
		default: assert_msg(0, "invalid expression in bytecode generation");  break;
	}

	if (!comparison) {
		Push(env, make_instruction(instruction, flags, REG_TO_REG, left.reg, right.reg, 0, 0));
	} else {
		Push(env, make_instruction(CMP, flags, REG_TO_REG, left.reg, right.reg, 0, 0));
		Push(env, make_instruction(instruction, flags, SINGLE_REG, left.reg, NO_REG, 0, 0));
	}

	result.flags |= EXPR_RESULT_ON_REGISTER;
	reg_free(env, right.reg);
	result.reg = left.reg;

	return result;
}

// stack position indicates the offset within the stack that the expression result should be written to.
Expr_Generation gen_code_for_expression(Gen_Environment* env, Ast* expr, s64 stack_position, bool address_of) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);
	Expr_Generation result = {0};

	u32 instruction_regsize = instruction_regsize_from_type(expr->type_return);
	u32 signed_ = (type_primitive_int_signed(expr->type_return)) ? SIGNED : 0;

	u64 start_address = move_code_offset(env->interp, env->code_offset);
	s64 start_offset = env->code_offset;
	u64 end_address = 0;

	switch (expr->node_type) {

	case AST_EXPRESSION_UNARY: {
		switch (expr->expr_unary.op) {
			case OP_UNARY_ADDRESSOF: {
				result = gen_code_for_expression(env, expr->expr_unary.operand, stack_position, true);
			} break;
			case OP_UNARY_CAST: {
				result = gen_code_for_expression(env, expr->expr_unary.operand, stack_position, address_of);
			} break;
			case OP_UNARY_PLUS: {
				result = gen_code_for_expression(env, expr->expr_unary.operand, stack_position, address_of);
			} break;
			case OP_UNARY_MINUS: {
				result = gen_code_for_expression(env, expr->expr_unary.operand, stack_position, address_of);
				Push(env, make_instruction(SUB, signed_ | instruction_regsize | IMMEDIATE_VALUE | INVERT_OPS, MEM_TO_REG, result.reg, NO_REG, 0, 0), (u64)0);
			} break;
			case OP_UNARY_LOGIC_NOT:
			case OP_UNARY_BITWISE_NOT: {
				result = gen_code_for_expression(env, expr->expr_unary.operand, stack_position, address_of);
				Push(env, make_instruction(NOT, signed_ | instruction_regsize, SINGLE_REG, result.reg, NO_REG, 0, 0));
			} break;
			case OP_UNARY_DEREFERENCE: {
				result = gen_code_for_expression(env, expr->expr_unary.operand, stack_position, address_of);
				Push(env, make_instruction(MOV, INSTR_QWORD, REG_PTR_TO_REG, result.reg, result.reg, 0, 0));
			} break;
			default: 
				assert_msg(0, "invalid unary operation in bytecode generation");
				break;
		}
	} break; // case AST_EXPRESSION_UNARY

	case AST_EXPRESSION_BINARY: {
		return generate_binary_expression(env, expr, stack_position, address_of);
	} break; // case AST_EXPRESSION_BINARY

	case AST_EXPRESSION_LITERAL: {
		Registers result_register = reg_allocate(env, type_primitive_float(expr->type_return));
		result.reg = result_register;

		switch (expr->expr_literal.type) {
		case LITERAL_SINT:
		case LITERAL_BIN_INT:
		case LITERAL_HEX_INT:
		case LITERAL_UINT: {
			switch (expr->type_return->primitive) {
			case TYPE_PRIMITIVE_U8:
			case TYPE_PRIMITIVE_S8:
				Push(env, make_instruction(MOV, INSTR_BYTE | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_U16:
			case TYPE_PRIMITIVE_S16:
				Push(env, make_instruction(MOV, INSTR_WORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_U32:
			case TYPE_PRIMITIVE_S32:
				Push(env, make_instruction(MOV, INSTR_DWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_U64:
			case TYPE_PRIMITIVE_S64:
				Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			}
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_FLOAT: {
			switch(expr->type_return->primitive) {
			case TYPE_PRIMITIVE_R32:
				Push(env, make_instruction(MOV, INSTR_FLOAT_32|IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_R64:
				Push(env, make_instruction(MOV, INSTR_FLOAT_64|IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			}
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_BOOL: {
			Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), (u64)expr->expr_literal.value_bool);
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_POINTER: {
			Push(env, make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), (u64)0);
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_ARRAY: {
			assert_msg(0, "not implemented");
		}break;
		case LITERAL_STRUCT: {
			assert_msg(0, "not implemented");
		} break;
		}

	} break; // case AST_EXPRESSION_LITERAL

	case AST_EXPRESSION_VARIABLE: {
		// Gives the result in a register if in the left side of an assignment
		Ast* decl = expr->expr_variable.decl;

		if (decl->node_type == AST_DECL_PROCEDURE) {
			return generate_code_expr_procedure(env, expr);
		} else {
			if (expr->flags & AST_FLAG_LEFT_ASSIGN || address_of) {
				return generate_code_expr_variable_address(env, expr, stack_position);
			} else {
				return generate_code_expr_variable_value(env, expr, stack_position);
			}
		}

	}break; // case AST_EXPRESSION_VARIABLE

	case AST_EXPRESSION_PROCEDURE_CALL: {
		Expr_Generation result = gen_code_for_expression(env, expr->expr_proc_call.caller, stack_position, true);
		assert(result.flags & EXPR_RESULT_ON_REGISTER);

		u64* after_call_return_value;

		// Save all registers to the stack, not the one holding the procedure address though
		register_allocated[result.reg].allocated = false;
		PushRegisters(env);
		register_allocated[result.reg].allocated = true;

		size_t offset_from_stack_base = 0;
		size_t arguments_size = 0;
		if (expr->expr_proc_call.args_count > 0) {
			// Push every argument to the stack, but first allocate space for it
			for (size_t i = 0; i < expr->expr_proc_call.args_count; ++i) {
				arguments_size += expr->expr_proc_call.args[i]->type_return->type_size_bits / 8;
			}
			size_t addition_stack_size = type_pointer_size() * 2;
			// allocate space for instructions, return address and stack base
			//Push(env, make_instruction(ADD, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), (u64)(arguments_size + addition_stack_size));

			offset_from_stack_base = env->stack_temp_offset;
			env->stack_temp_offset += arguments_size + addition_stack_size;
			env->stack_size += arguments_size + addition_stack_size;
		}

		u32 offset_first_four_args = 0;
		if (expr->expr_proc_call.args_count > 0) {
			// evaluate all arguments and push them from right to left
			for (size_t i = 0; i < expr->expr_proc_call.args_count; ++i) {
				Ast* arg = expr->expr_proc_call.args[i];
				Expr_Generation result = gen_code_for_expression(env, arg, offset_from_stack_base);
				u32 instr_size = instruction_regsize_from_type(arg->type_return);
				if (result.flags & EXPR_RESULT_ON_REGISTER) {
					// Push when result is in register, otherwise the result is in its place already
					Push(env, make_instruction(PUSH, instr_size, SINGLE_REG, result.reg, NO_REG, 0, 0));
				}
				reg_free(env, result.reg);

				u32 s = arg->type_return->type_size_bits / 8;
				assert(s < 255);
				if (i < 4) {
					offset_first_four_args |= ((u8)s << (i * 8));
				}
			}
		}

		reg_free(env, result.reg);

		// push stack base value to be in R_SB[-8]
		// Push(env, make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
		// push return value to be the in R_SB[-8]
		Push(env, make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), &after_call_return_value);

		// make call
		if (result.flags & EXPR_RESULT_EXT_CALL) {
			// 0x 11 22 33 44
			Push(env, make_instruction(PUSH, INSTR_DWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)offset_first_four_args);
			Instruction_Info ac = Push(env, make_instruction(EXTCALL, INSTR_QWORD, SINGLE_REG, result.reg, NO_REG, 0, 0));
			*after_call_return_value = (u64)(ac.absolute_address + ac.size_bytes);
		} else {
			Instruction_Info ac = Push(env, make_instruction(JMP, INSTR_QWORD, SINGLE_REG, result.reg, NO_REG, 0, 0));
			*after_call_return_value = (u64)(ac.absolute_address + ac.size_bytes);
		}

		// dealloc proc arguments in the stack
		Push(env, make_instruction(SUB, INSTR_QWORD|IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), (u64)arguments_size);

		// Restore all registers from the stack
		PopRegisters(env);

		Expr_Generation return_value = { 0 };
		return_value.flags = EXPR_RESULT_ON_REGISTER;
		return_value.reg = R_0;
		return return_value;
	}break; // case AST_EXPRESSION_PROCEDURE_CALL

	default:break;
	}

	return result;
}

/*
	For each node generate code
*/
void gen_code_node(Gen_Environment* env, Ast* node) {
	if (!node) return;

	switch (node->node_type) {

		case AST_DECL_PROCEDURE: {
			if (node->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) {
				break;
			}
			node->decl_procedure.proc_runtime_address = (u64*)(env->code + env->code_offset);

			s32 offset_from_sb = -type_pointer_size() * 2; // -16 return value and stack base

			if (node->decl_procedure.arguments_count) {
				for (s32 i = node->decl_procedure.arguments_count - 1; i >= 0; --i) {
					Ast* arg = node->decl_procedure.arguments[i];
					offset_from_sb -= arg->decl_variable.size_bytes;
					arg->decl_variable.stack_offset = offset_from_sb;
				}
			}

			generate_proc_prologue(env, node);
			gen_code_node(env, node->decl_procedure.body);
			generate_proc_epilogue(env);
		}break;

		case AST_COMMAND_BLOCK: {
			if (node->comm_block.commands) {
				for (size_t i = 0; i < array_get_length(node->comm_block.commands); ++i) {
					Ast* comm = node->comm_block.commands[i];
					gen_code_node(env, comm);
				}
			}
		}break;

		case AST_COMMAND_RETURN: {
			if (node->comm_return.expression) {
				Expr_Generation result = gen_code_for_expression(env, node->comm_return.expression);
				
				reg_free(env, R_0);	// R_0 must be free in order to use it for the return value

				if(result.reg != R_0) {
					Push(env, make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_0, result.reg, 0, 0));
					reg_free(env, result.reg);
				}
			}
			generate_proc_epilogue(env);
		}break;

		case AST_COMMAND_VARIABLE_ASSIGNMENT: {
			Expr_Generation lvalue_result = { 0 };
			if (node->comm_var_assign.lvalue) {
				// The result register holds the address to be used to store the result of the rvalue
				lvalue_result = gen_code_for_expression(env, node->comm_var_assign.lvalue, 0, true);
				assert(lvalue_result.flags & EXPR_RESULT_ON_REGISTER);
			}

			Expr_Generation rvalue_result = gen_code_for_expression(env, node->comm_var_assign.rvalue);
			
			if (node->comm_var_assign.lvalue) {
				u32 instruction_size = instruction_regsize_from_type(node->comm_var_assign.rvalue->type_return);

				if (rvalue_result.flags & EXPR_RESULT_ON_REGISTER) {
					// store register on the address given by lvalue
					Push(env, make_instruction(MOV, instruction_size, REG_TO_REG_PTR, lvalue_result.reg, rvalue_result.reg, 0, 0));
				} else {
					// store memory pointed by rvalue register in memory pointed by lvalue
					size_t rvalue_size = node->comm_var_assign.rvalue->type_return->type_size_bits / 8;
					Push(env, make_instruction(COPY, IMMEDIATE_OFFSET | IMMEDIATE_VALUE | instruction_size, REG_TO_REG, lvalue_result.reg, rvalue_result.reg, NO_REG, 0), rvalue_size);
				}
				reg_free(env, lvalue_result.reg);
			}
			reg_free(env, rvalue_result.reg);
		}break;

		case AST_COMMAND_IF:{
			// conditional expression in a register R_X
			Expr_Generation condition = gen_code_for_expression(env, node->comm_if.condition);
			assert(condition.flags & EXPR_RESULT_ON_REGISTER);

			// cmp R_X, 0|1
			Push(env, make_instruction(CMP, INSTR_QWORD|IMMEDIATE_VALUE, MEM_TO_REG, condition.reg, NO_REG, 0, 0), (u64)0);
			reg_free(env, condition.reg);

			// beq case_false
			Instruction_Info beq = Push(env, make_instruction(BEQ, SIGNED|IMMEDIATE_OFFSET, NO_ADDRESSING, NO_REG, NO_REG, 0, 0));

			// case_true:
			gen_code_node(env, node->comm_if.body_true);

			// jmp if_end;
			u64* end_jump_address;
			Instruction_Info jmp = Push(env, make_instruction(JMP, INSTR_QWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), &end_jump_address);
			((Instruction*)beq.absolute_address)->immediate_offset = jmp.offset - beq.offset + jmp.size_bytes;

			// case_false:
			gen_code_node(env, node->comm_if.body_false);

			// if_end:
			*end_jump_address = (u64)(env->code + env->code_offset);
		}break;

		case AST_DECL_VARIABLE: {
			node->decl_variable.stack_offset = env->stack_base_offset;
			env->stack_base_offset += node->decl_variable.variable_type->type_size_bits / 8;

			// allocate memory on the stack
			// Push(env, make_instruction(ADD, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), (u64)0);

			if (node->decl_variable.assignment) {
				// TODO(psv): make the result be directly to the memory allocated
				Expr_Generation result = gen_code_for_expression(env, node->decl_variable.assignment, node->decl_variable.stack_offset);
				if (result.flags & EXPR_RESULT_ON_REGISTER) {
					Push(env, make_instruction(MOV, INSTR_QWORD|IMMEDIATE_OFFSET, REG_TO_REG_PTR, R_SB, result.reg, 0, node->decl_variable.stack_offset));
					reg_free(env, result.reg);
				}
			}
		}break;

		case AST_COMMAND_FOR: {
			Gen_Loop_Stack ls = {};
			array_push(env->loop_stack, &ls);
			
			// conditional expression in a register R_X
			u64 start_address = (u64)(env->code + env->code_offset);

			Expr_Generation condition = gen_code_for_expression(env, node->comm_for.condition);
			assert(condition.flags & EXPR_RESULT_ON_REGISTER);

			// cmp R_X, 0|1
			Push(env, make_instruction(CMP, INSTR_QWORD|IMMEDIATE_VALUE, MEM_TO_REG, condition.reg, NO_REG, 0, 0), (u64)0);
			reg_free(env, condition.reg);

			// beq for_end
			Instruction_Info beq = Push(env, make_instruction(BEQ, SIGNED|IMMEDIATE_OFFSET, NO_ADDRESSING, NO_REG, NO_REG, 0, 0));

			// case_true:
			gen_code_node(env, node->comm_for.body);

			// jmp for_begin;
			Instruction_Info jmp = Push(env, make_instruction(JMP, INSTR_QWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), start_address);
			((Instruction*)beq.absolute_address)->immediate_offset = jmp.offset - beq.offset + jmp.size_bytes;
			// if_end:

			Gen_Loop_Stack* loop_stack = (Gen_Loop_Stack*)array_pop(env->loop_stack);

			if (loop_stack->fill_before) {
				for (size_t i = 0; i < array_get_length(loop_stack->fill_before); ++i) {
					*loop_stack->fill_before[i] = (u64)(start_address);
				}
				array_release(loop_stack->fill_before);
			}
			if (loop_stack->fill_after) {
				for (size_t i = 0; i < array_get_length(loop_stack->fill_after); ++i) {
					*loop_stack->fill_after[i] = (u64)(env->code + env->code_offset);
				}
				array_release(loop_stack->fill_after);
			}
		}break;

		case AST_COMMAND_BREAK: {
			Instruction_Info ii = Push(env, make_instruction(JMP, INSTR_QWORD|IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)0xcccccccccccccccc);
			u64* address_to_fill = (u64*)&((Instruction*)ii.absolute_address)->immediate_offset;

			s64 lvl = 1;
			if (node->comm_break.level) {
				lvl = node->comm_break.level->expr_literal.value_s64;
			}
			size_t stack_size = array_get_length(env->loop_stack);
			assert(stack_size >= lvl);
			if (!env->loop_stack[stack_size - lvl].fill_after) {
				env->loop_stack[stack_size - lvl].fill_after = array_create(u64*, 4);
			}
			array_push(env->loop_stack[stack_size - lvl].fill_after, &address_to_fill);
		} break;

		default: break;
	}
}

void bytecode_generate(Interpreter* interp, Ast** top_level) {
	TIME_FUNC();
	Gen_Environment env = {};
	env.interp = interp;
	env.code = interp->code;

	env.loop_stack = array_create(Gen_Loop_Stack, 16);

	generate_entry_point(&env);

	for (size_t i = 0; i < array_get_length(top_level); ++i) {
		Ast* node = top_level[i];

		gen_code_node(&env, node);
	}

	Push(&env, make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));

	for (size_t i = 0; i < array_get_length(env.proc_addressing_queue); ++i) {
		Ast* decl = env.proc_addressing_queue[i].decl;
		*env.proc_addressing_queue[i].fill_address = (u64)decl->decl_procedure.proc_runtime_address;
	}
}


/*
	Load libraries dynamically
*/

#ifdef _WIN64
// @TODO linux version
HMODULE load_library_dynamic(string* library) {
	char* c_name_library = make_c_string(*library);
	HMODULE lib = LoadLibraryA(c_name_library);
	free(c_name_library);
	return lib;
}

void free_library(HMODULE lib) {
	FreeLibrary(lib);
}

// @TODO linux version
// @TODO OPTIMIZE, load library once for all external calls
void* load_address_of_external_function(string* name, HMODULE library) {
	char* c_name = make_c_string(*name);
	void* mem = GetProcAddress(library, c_name);
	free(c_name);
	return mem;
}
#else
void* load_library_dynamic(string* library) {
	// "/lib/x86_64-linux-gnu/libc.so.6"
	char* libname = make_c_string(*library);
	void* lib = dlopen((const char*)libname, RTLD_LAZY);
#if DEBUG
	printf("loaded library %s at %p\n", libname, lib);
#endif
	free(libname);

	return lib;
}
void* load_address_of_external_function(string* name, void* library) {
	char* procname = make_c_string(*name);
	void* proc = dlsym(library, (const char*)procname);
#if DEBUG
	printf("loaded procedure %s at %p\n", procname, proc);
#endif
	free(procname);

	return proc;
}

void free_library(void* library) {
	dlclose(library);
}
#endif
