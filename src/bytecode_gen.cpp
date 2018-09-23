#include "bytecode_gen.h"
#include <ho_system.h>
#include "interpreter.h"
#include "type.h"

void* load_address_of_external_function(string* name, HMODULE library);
HMODULE load_library_dynamic(string* library);

s64 gen_entry_point() {
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

	return end_address - start_address;
}

// returns the number of bytes that advanced
s64 gen_proc_prologue(Ast* proc_body, s64 offset)
{
	// push sb
	// mov	sb, sp
	// add	sp, stack_size

	u64 start_address = move_code_offset(offset);
	s64 stack_size = 0;

	// calculate stack size from proc body
	Scope* scope = proc_body->decl_procedure.body->comm_block.block_scope;
	int num_decls = scope->decl_count;

	// @TODO OPTIMIZE THIS!!!!!
	// @TODO OPTIMIZE THIS!!!!!
	// @TODO OPTIMIZE THIS!!!!!
	// make a data struct on the scope to store all the declarations and its respective offsets within the stack
	if (num_decls > 0) {
		for (int i = 0; i < scope->symb_table.entries_count; ++i) {
			if (scope->symb_table.entries[i].occupied) {
				Ast* decl_node = scope->symb_table.entries[i].decl_node;
				if (decl_node->node_type == AST_DECL_VARIABLE) {
					assert(decl_node->decl_variable.variable_type->type_size_bits != 0);
					stack_size += decl_node->decl_variable.variable_type->type_size_bits / 8;
				}
			}
		}
	}

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

	return end_address - start_address;
}

s64 gen_proc_epilogue(s64 offset) {
	// mov	sp, sb
	// pop	sb
	// pop	ss		; return address
	// jmp	ss		; return from call

	u64 start_address = move_code_offset(offset);

	push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_SP, R_SB, 0, 0));
	push_instruction(make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	u64 r = push_instruction(make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));
	u64 end_address = push_instruction(make_instruction(JMP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));

	u64 diff = end_address - r;

	return end_address - start_address;
}

void bytecode_generate(Ast** top_level) {
	s64 offset = 0;
	offset = gen_entry_point();

	for (size_t i = 0; i < array_get_length(top_level); ++i) {
		Ast* node = top_level[i];

		if (node->node_type == AST_DECL_PROCEDURE) {
			offset = gen_proc_prologue(node, offset);
			offset = gen_proc_epilogue(offset);
		}
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
#error "os not supported"
#endif