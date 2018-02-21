#pragma once
#include "ast.h"
#include "type.h"

struct LLVM_Code_Generator {
	char types_buffer[1024];
	u32 types_ptr;
	void add_struct_type(Type_Instance* type);
	int sprint_type(char* msg, ...);

	char buffer[4096];
	u32  ptr;
	u32  temp;

	int sprint(char* msg, ...);
	u32 alloc_temp();

	u32 string_literal_temp;
	Ast** deferred_string_literals;
};

// returns number that goes into <n> in the global string name
// @__str.<n>
u32 llvm_defer_string_literal(LLVM_Code_Generator* cg, Ast* node);

void llvm_generate_ir(Ast** toplevel);

void llvm_emit_ir_for_node(LLVM_Code_Generator* cg, Ast* node);

u32 llvm_emit_ir_for_expression(LLVM_Code_Generator* cg, Ast* expr);

void llvm_emit_ir_for_type_code(LLVM_Code_Generator* cg, Type_Instance* type, bool convert_void_to_i8 = false);
void llvm_emit_ir_for_type_typedecl(LLVM_Code_Generator* cg, Type_Instance* type, bool convert_void_to_i8 = false);

void llvm_emit_ir_for_literal(LLVM_Code_Generator* cg, Ast_Literal* lit_exp);

void llvm_instr_emit_store(LLVM_Code_Generator* cg, Type_Instance* type_variable, Ast* value, u32 temp_reg);

void llvm_emit_function_attributes(LLVM_Code_Generator* cg);