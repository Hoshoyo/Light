#include "code_generator.h"
#include "interpreter.h"
#include "symbol_table.h"

// returns the number of bytes that advanced
s64 gen_proc_prologue(Ast* proc_body, s64 offset)
{
	// push sb
	// mov	sb, sp
	// add	sp, stack_size

	u64 start_address = move_code_offset(offset);
	s64 stack_size = 0;

	// calculate stack size from proc body
	Scope* scope = proc_body->block.scope;
	int num_decls = scope->num_declarations;

	// @TODO OPTIMIZE THIS!!!!!
	// @TODO OPTIMIZE THIS!!!!!
	// @TODO OPTIMIZE THIS!!!!!
	// make a data struct on the scope to store all the declarations and its respective offsets within the stack
	if (num_decls > 0) {
		for (int i = 0; i < scope->symb_table->max_entries; ++i) {
			if (scope->symb_table->entries[i].used) {
				Ast* decl_node = scope->symb_table->entries[i].node;
				if (decl_node->node == AST_NODE_VARIABLE_DECL) {
					assert(decl_node->var_decl.type->type_size != 0);
					stack_size += decl_node->var_decl.type->type_size;
				}
			}
		}
	}

	push_instruction(make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	push_instruction(make_instruction(MOV, INSTR_QWORD, REG_TO_REG, R_SB, R_SP, 0, 0));
	u64 end_address = push_instruction(make_instruction(ADD, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SP, NO_REG, 0, 0), stack_size);

#if 1
	char* add = (char*)get_data_segment_address();
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 0);
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), *(u64*)get_data_segment_address());		// check SINGLE_MEM_PTR
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), *(u64*)get_data_segment_address());
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), 0);

	push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 8 * 4);

	Token t;
	t.value = MAKE_STRING("MessageBoxA");
	s64 index = -1;
	do {
		if(scope->num_declarations > 0)
			index = scope->symb_table->entry_exist(&t);	
		if (index != -1) break;
		scope = scope->parent;
	} while (scope != 0);
	Ast* entry = scope->symb_table->entries[index].node;

	end_address = push_instruction(make_instruction(EXTCALL, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), (u64)entry->proc_decl.external_runtime_address);
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
	push_instruction(make_instruction(POP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));
	u64 end_address = push_instruction(make_instruction(JMP, INSTR_QWORD, SINGLE_REG, R_SS, NO_REG, 0, 0));

	return end_address - start_address;
}

s64 gen_entry_point() {
	// push sb				; 8  bytes
	// push next_address	; 16 bytes
	// mov ss, 16			; 16 bytes
	// jmp main				; 16 bytes
	// hlt					; 8 bytes
// main: 

	u64 start_address = move_code_offset(0);

	push_instruction(make_instruction(PUSH, INSTR_QWORD, SINGLE_REG, R_SB, NO_REG, 0, 0));
	push_instruction(make_instruction(PUSH, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), start_address + 56);
	push_instruction(make_instruction(MOV, INSTR_QWORD | IMMEDIATE_VALUE, MEM_TO_REG, R_SS, NO_REG, 0, 0), 2 * 8);
	push_instruction(make_instruction(JMP, INSTR_QWORD | IMMEDIATE_VALUE, SINGLE_MEM, NO_REG, NO_REG, 0, 0), start_address + 64);
	u64 end_address = push_instruction(make_instruction(HLT, 0, 0, NO_REG, NO_REG, 0, 0));

	return end_address - start_address;
}

//
// Arguments:
// node				- The Ast node which the code will be generated
// offset			- The offset with which the first instruction will be generated at
//
// Return values:
//  0				- did not generate code but exited gracefully
// -1				- an error occurred on code generation
// [1, 0xffffffff]  - offset in bytes of the next empty space after the last instruction generated
//
s64 gen_interpreter_code_for_node(Ast* node, s64 offset)
{
	if (node->is_decl) {
		switch (node->node) {
			
			case AST_NODE_PROC_DECLARATION: {
				assert(node->proc_decl.body);
				offset += gen_proc_prologue(node->proc_decl.body, offset);
				gen_proc_epilogue(offset);
			}break;
			case AST_NODE_DIRECTIVE: {
				// @INCOMPLETE
				Token* lib = node->directive.literal_argument->expression.literal_exp.lit_tok;
				Token* proc = node->directive.declaration->proc_decl.name;
				HMODULE loaded_library = load_library_dynamic(&lib->value);
				void* extcall = load_address_of_external_function(&proc->value, loaded_library);
				node->directive.declaration->proc_decl.external_runtime_address = extcall;
			}break;
			case AST_NODE_PROC_FORWARD_DECL:
			case AST_NODE_STRUCT_DECLARATION:
			case AST_NODE_VARIABLE_DECL:		return 0;

			default: return -1;
		}
	} else {
		return 0;
		switch (node->node) {
			case AST_NODE_BINARY_EXPRESSION: {

			}break;
			case AST_NODE_BLOCK: {

			}break;
			case AST_NODE_BREAK_STATEMENT: {

			}break;
			case AST_NODE_CONTINUE_STATEMENT: {

			}break;
			case AST_NODE_EXPRESSION_ASSIGNMENT: {

			}break;
			case AST_NODE_IF_STATEMENT: {

			}break;
			case AST_NODE_LITERAL_EXPRESSION: {

			}break;
			case AST_NODE_NAMED_ARGUMENT: {

			}break;
			case AST_NODE_PROCEDURE_CALL: {

			}break;
			case AST_NODE_RETURN_STATEMENT: {

			}break;
			case AST_NODE_UNARY_EXPRESSION: {

			}break;
			case AST_NODE_VARIABLE_EXPRESSION: {

			}break;
		}
	}
}

int generate_interpreter_code(Ast** ast)
{
	s64 address = gen_entry_point();

	int num_decls = array_get_length(ast);
	for (int i = 0; i < num_decls; ++i) {
		Ast* node = ast[i];
		int res = gen_interpreter_code_for_node(node, address);
	}
	return 0;
}

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