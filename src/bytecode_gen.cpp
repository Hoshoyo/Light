#include "bytecode_gen.h"
#include <ho_system.h>
#include "interpreter.h"
#include "decl_check.h"
#include "type.h"

#if defined(_WIN64)
void* load_address_of_external_function(string* name, HMODULE library);
HMODULE load_library_dynamic(string* library);
#elif defined(__linux__)
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
static Registers* register_stack;

// The oldest register, the one allocated farther back gets allocated in
// case of all allocated.
Registers reg_allocate(Gen_Environment* env, bool floating_point = false) {
	s32 oldest = -1;
	Registers result = NO_REG;

	Registers r_first = (floating_point) ? FR_0 : R_0;
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
	if (!register_stack) {
		register_stack = array_create(Registers, 512);
	}
	register_allocated[oldest].in_stack = true;
	register_allocated[oldest].age++;
	array_push(register_stack, (Registers*)&oldest);

	env->code_offset += push_instruction(make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, oldest, NO_REG, 0, 0));

	return (Registers)oldest;
}

// bool indicates if the caller needs to generate pop call for this register
void reg_free(Gen_Environment* env, Registers r) {
	bool pop = false;
	// free the last in the stack rather
	if (register_allocated[r].in_stack) {
		Registers last = *(Registers*)array_pop(register_stack);
		assert(last == r);
		pop = true;
	}
	register_allocated[r].allocated = false;
	register_allocated[r].in_stack = false;
	register_allocated[r].age = 0;

	if (pop) {
		env->code_offset += push_instruction(make_instruction(POP, INSTR_QWORD, SINGLE_REG, r, NO_REG, 0, 0));
	}
}

/*
	********************************************
*/


void gen_entry_point(Gen_Environment* env) {
	// push sb				; 8  bytes
	// push next_address	; 16 bytes
	// mov ss, 16			; 16 bytes
	// jmp main				; 16 bytes
	// hlt					; 8 bytes
	// main: 

	u64 start_address = move_code_offset(0);

	u64 o1 = push_instruction(make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	u64* hlt_relative_address;
	u64 o2 = push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), &hlt_relative_address);
	u64 o3 = push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 2 * 8);
	u64* main_relative_address;
	u64 o4 = push_instruction(make_instruction(JMP, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), &main_relative_address);
	u64 end_address = push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));

	*hlt_relative_address = o4;
	*main_relative_address = end_address;

	env->code_offset += end_address - start_address;
}

void gen_proc_prologue(Gen_Environment* env, Ast* proc_body)
{
	// push sb
	// mov	sb, sp
	// add	sp, stack_size

	u64 start_address = move_code_offset(env->code_offset);
	s64 stack_size = 0;

	// calculate stack size from proc body
	Scope* scope = proc_body->decl_procedure.body->comm_block.block_scope;
	int num_decls = scope->decl_count;

	printf("stack offset: %d\n", scope->stack_allocation_offset);
	stack_size = scope->stack_allocation_offset;
	env->stack_size = stack_size;
	env->stack_base_offset = 0;
	env->stack_temp_offset = 0;

	push_instruction(make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_SB, R_SP, 0, 0));
	u64 end_address = push_instruction(make_instruction(ADD, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), stack_size);

#if 0
	char* add = (char*)get_data_segment_address();
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 0);
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), *(u64*)get_data_segment_address());		// check SINGLE_MEM_PTR
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), *(u64*)get_data_segment_address());
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 0);

	push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 8 * 4);

	Token t;
	t.value = MAKE_STRING("MessageBoxA");

	string libname = MAKE_STRING("user32.dll");
	HMODULE libdll = load_library_dynamic(&libname);
	void * func = load_address_of_external_function(&t.value, libdll);
	end_address = push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)func);
#endif

	env->code_offset += end_address - start_address;
}

void gen_proc_epilogue(Gen_Environment* env) {
	// mov	sp, sb
	// pop	sb
	// pop	ss		; return address
	// jmp	ss		; return from call

	u64 start_address = move_code_offset(env->code_offset);

	push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_SP, R_SB, 0, 0));
	push_instruction(make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	u64 r = push_instruction(make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));
	u64 end_address = push_instruction(make_instruction(JMP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));

	u64 diff = end_address - r;

	env->code_offset += end_address - start_address;
}

const u32 EXPR_RESULT_ON_STACK    = FLAG(0);
const u32 EXPR_RESULT_ON_REGISTER = FLAG(0);
struct Expr_Generation {
	s64 result_offset_from_sb;
	Registers reg;
	u32 flags;
};

Expr_Generation gen_code_for_expression(Gen_Environment* env, Ast* expr, s64 stack_position = 0) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);
	Expr_Generation result = {0};

	u64 start_address = move_code_offset(env->code_offset);
	s64 start_offset = env->code_offset;
	u64 end_address = 0;

	switch (expr->node_type) {

	case AST_EXPRESSION_BINARY: {
		// Calculate both sides
		Expr_Generation left = gen_code_for_expression(env, expr->expr_binary.left);
		Expr_Generation right = gen_code_for_expression(env, expr->expr_binary.right);
		u16 instruction = 0;

		start_address = move_code_offset(env->code_offset);

		switch (expr->expr_binary.op) {
			case OP_BINARY_PLUS:			instruction = ADD; break; // +
			case OP_BINARY_MINUS:			instruction = SUB; break; // -
			case OP_BINARY_MULT:			instruction = MUL; break; // *
			case OP_BINARY_DIV:				instruction = DIV; break; // /
			case OP_BINARY_MOD:				instruction = MOD; break; // %
			case OP_BINARY_AND:				instruction = AND; break; // &
			case OP_BINARY_OR:				instruction = OR; break;  // |
			case OP_BINARY_XOR:				instruction = XOR; break; // ^
			case OP_BINARY_SHL:				instruction = SHL; break; // <<
			case OP_BINARY_SHR:				instruction = SHR; break; // >>

			case OP_BINARY_LT:				// <
			case OP_BINARY_GT:				// >
			case OP_BINARY_LE:				// <=
			case OP_BINARY_GE:				// >=
			case OP_BINARY_EQUAL:			// ==
			case OP_BINARY_NOT_EQUAL:		// !=
				instruction = CMP;
				break;

			case OP_BINARY_LOGIC_AND:		instruction = AND; break; // &&
			case OP_BINARY_LOGIC_OR:		instruction = OR; break;  // ||

			// TODO(psv): dot and vector acessing operators
			case OP_BINARY_DOT:				assert_msg(0, "unimplemented bytecode generation for dot operator"); break; // .
			case OP_BINARY_VECTOR_ACCESS:	assert_msg(0, "unimplemented bytecode generation for vector acessing"); break; // []
			default: assert_msg(0, "invalid expression in bytecode generation");  break;
		}

		u16 flags = INSTR_DWORD | SIGNED;
		end_address = push_instruction(make_instruction(instruction, flags, REG_TO_REG, left.reg, right.reg, 0, 0));
		result.flags |= EXPR_RESULT_ON_REGISTER;
		reg_free(env, right.reg);
		result.reg = left.reg;
		env->code_offset += end_address - start_address;
	} break; // case AST_EXPRESSION_BINARY

	case AST_EXPRESSION_LITERAL: {
		Registers result_register = reg_allocate(env, type_primitive_float(expr->type_return));
		result.reg = result_register;

		start_address = move_code_offset(env->code_offset);
		switch (expr->expr_literal.type) {
		case LITERAL_SINT:
		case LITERAL_BIN_INT:
		case LITERAL_HEX_INT:
		case LITERAL_UINT: {
			switch (expr->type_return->primitive) {
			case TYPE_PRIMITIVE_U8:
			case TYPE_PRIMITIVE_S8:
				end_address = push_instruction(make_instruction(MOV, INSTR_BYTE | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_U16:
			case TYPE_PRIMITIVE_S16:
				end_address = push_instruction(make_instruction(MOV, INSTR_WORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_U32:
			case TYPE_PRIMITIVE_S32:
				end_address = push_instruction(make_instruction(MOV, INSTR_DWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_U64:
			case TYPE_PRIMITIVE_S64:
				end_address = push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			}
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_FLOAT: {
			switch(expr->type_return->primitive) {
			case TYPE_PRIMITIVE_R32:
				end_address = push_instruction(make_instruction(MOV, INSTR_DWORD|INSTR_FLOAT_32|IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			case TYPE_PRIMITIVE_R64:
				end_address = push_instruction(make_instruction(MOV, INSTR_DWORD|INSTR_FLOAT_64|IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), expr->expr_literal.value_u64);
				break;
			}
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_BOOL: {
			end_address = push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), (u64)expr->expr_literal.value_bool);
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_POINTER: {
			end_address = push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, result_register, NO_REG, 0, 0), (u64)0);
			result.flags |= EXPR_RESULT_ON_REGISTER;
		}break;
		case LITERAL_ARRAY: {
			assert_msg(0, "not implemented");
		}break;
		case LITERAL_STRUCT: {
			assert_msg(0, "not implemented");
		} break;
		}

		env->code_offset += end_address - start_address;
	} break; // case AST_EXPRESSION_LITERAL

	case AST_EXPRESSION_VARIABLE: {
		// Gives the result in a register if in the left side of an assignment

		start_address = move_code_offset(env->code_offset);
		Ast* decl = decl_from_name(expr->scope, expr->expr_variable.name);
		assert(decl);
		if (decl->decl_variable.flags & DECL_VARIABLE_STACK) {
			s32 stack_offset = decl->decl_variable.stack_offset;

			Registers r = reg_allocate(env, type_primitive_float(expr->type_return));

			if (expr->flags & AST_FLAG_LEFT_ASSIGN) {
				// MOV R_X, R_SB
				push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, r, R_SB, NO_REG, 0));
				// ADD R_X, stack_offset
				end_address = push_instruction(make_instruction(ADD, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, r, NO_REG, 0, 0), decl->decl_variable.stack_offset);
				result.flags |= EXPR_RESULT_ON_REGISTER;
				result.reg = r;
			} else {
				// When the type is of register size, allocate a register and put the result in it
				if (type_regsize(expr->type_return)) {
					// mov rx, [sb + offset]
					end_address = push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_OFFSET, REG_PTR_TO_REG, r, R_SB, NO_REG, decl->decl_variable.stack_offset));
					result.flags |= EXPR_RESULT_ON_REGISTER;
					result.reg = r;
				} else {
					// COPY from variable stack offset to stack_position
					if (decl->decl_variable.stack_offset != stack_position) {
						// MOV R_X, R_SB
						push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, r, R_SB, NO_REG, 0));
						// ADD R_X, stack_offset
						push_instruction(make_instruction(ADD, SIGNED | IMMEDIATE_VALUE, MEM_TO_REG, r, NO_REG, 0, 0), decl->decl_variable.stack_offset);

						// COPY [R_SB + stack_position], R_X
						end_address = push_instruction(make_instruction(COPY, IMMEDIATE_OFFSET | IMMEDIATE_VALUE, REG_TO_REG, R_SP, r, NO_REG, 0), stack_position);
					} else {
						// the variable is already where it is supposed to be ...
					}
					result.flags |= EXPR_RESULT_ON_STACK;
					result.result_offset_from_sb = stack_position;
				}
			}	
		} else {
			// TODO(psv): get variable in data segment
			assert_msg(0, "not implemented");
		}
		if (end_address) {
			env->code_offset += end_address - start_address;
		}
	}break; // case AST_EXPRESSION_VARIABLE

	case AST_EXPRESSION_PROCEDURE_CALL: {

	}break; // case AST_EXPRESSION_PROCEDURE_CALL

	default:break;
	}

	return result;
}

void gen_register_to_stack(Gen_Environment* env, Registers r, s64 stack_offset) {
	u64 start_address = move_code_offset(env->code_offset);
	u64 end = push_instruction(make_instruction(MOV, INSTR_QWORD|IMMEDIATE_OFFSET, REG_TO_REG_PTR, R_SB, r, 0, stack_offset));
	env->code_offset += end - start_address;
}

void gen_reg_addr_to_stack(Gen_Environment* env, Registers r, s64 stack_offset) {

}

/*
	For each node generate code
*/
void gen_code_node(Gen_Environment* env, Ast* node) {
	if (!node) return;

	u64 start_address = move_code_offset(env->code_offset);
	u64 end_address = start_address;
	s64 start_offset = env->code_offset;

	switch (node->node_type) {

		case AST_DECL_PROCEDURE: {
			gen_proc_prologue(env, node);
			gen_code_node(env, node->decl_procedure.body);
			gen_proc_epilogue(env);
		}break;

		case AST_COMMAND_BLOCK: {
			for (size_t i = 0; i < array_get_length(node->comm_block.commands); ++i) {
				Ast* comm = node->comm_block.commands[i];
				gen_code_node(env, comm);
			}
		}break;

		case AST_COMMAND_RETURN: {
			
			if (node->comm_return.expression) {
				Expr_Generation result = gen_code_for_expression(env, node->comm_return.expression);
				
				reg_free(env, R_0);	// R_0 must be free in order to use it for the return value

				if (result.reg != R_0) {
					end_address = push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_0, result.reg, 0, 0));
					reg_free(env, result.reg);
				}
			}
			if (end_address != start_address) {
				env->code_offset += end_address - start_address;
			}
		}break;

		case AST_COMMAND_VARIABLE_ASSIGNMENT: {
			// The result register holds the address to be used to store the result of the rvalue
			Expr_Generation lvalue_result = gen_code_for_expression(env, node->comm_var_assign.lvalue);
			assert(lvalue_result.flags & EXPR_RESULT_ON_REGISTER);

			Expr_Generation rvalue_result = gen_code_for_expression(env, node->comm_var_assign.rvalue);

			start_address = move_code_offset(env->code_offset);
			if (rvalue_result.flags & EXPR_RESULT_ON_REGISTER) {
				// store register on the address given by lvalue
				end_address = push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG_PTR, lvalue_result.reg, rvalue_result.reg, 0, 0));
			} else {
				// store memory pointed by rvalue register in memory pointed by lvalue
				size_t rvalue_size = node->comm_var_assign.rvalue->type_return->type_size_bits / 8;
				end_address = push_instruction(make_instruction(COPY, IMMEDIATE_OFFSET | IMMEDIATE_VALUE, REG_TO_REG, lvalue_result.reg, rvalue_result.reg, NO_REG, 0), rvalue_size);
			}
			reg_free(env, lvalue_result.reg);
			reg_free(env, rvalue_result.reg);
			env->code_offset += end_address - start_address;
		}break;

		case AST_DECL_VARIABLE: {
			node->decl_variable.stack_offset = env->stack_base_offset;
			env->stack_base_offset += node->decl_variable.variable_type->type_size_bits / 8;

			if (node->decl_variable.assignment) {
				// TODO(psv): make the result be directly to the memory allocated
				Expr_Generation result = gen_code_for_expression(env, node->decl_variable.assignment, node->decl_variable.stack_offset);
				if (result.flags & EXPR_RESULT_ON_REGISTER) {
					gen_register_to_stack(env, result.reg, node->decl_variable.stack_offset);
					reg_free(env, result.reg);
				}
			}
		}break;

		default: break;
	}
}

void bytecode_generate(Ast** top_level) {
	Gen_Environment env = {};
	gen_entry_point(&env);

	for (size_t i = 0; i < array_get_length(top_level); ++i) {
		Ast* node = top_level[i];

		gen_code_node(&env, node);
	}
	HALT;
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
	void* lib = dlopen((const char*)library->data, RTLD_LAZY);
	printf("loaded library %s at %p\n", library->data, lib);
	return lib;
}
void* load_address_of_external_function(string* name, void* library) {
	void* proc = dlsym(library, (const char*)name->data);
	printf("loaded procedure %s at %p\n", name->data, proc);
	return proc;
}

void free_library(void* library) {
	dlclose(library);
}
#endif