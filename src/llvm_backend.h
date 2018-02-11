#pragma once
#include "ast.h"
#include "type.h"

struct LLVM_Code_Generator {
	char buffer[4096];
	u32  ptr;

	u32  temp;

	int sprint(char* msg, ...);
	u32 alloc_temp();
};


void llvm_generate_ir(Ast** toplevel);

void llvm_emit_ir_for_node(LLVM_Code_Generator* cg, Ast* node);

u32 llvm_emit_ir_for_expression(LLVM_Code_Generator* cg, Ast* expr);
void llvm_emit_ir_for_type(LLVM_Code_Generator* cg, Type_Instance* type, bool convert_void_to_i8 = false);
void llvm_emit_ir_for_literal(LLVM_Code_Generator* cg, Ast_Literal* lit_exp);

void llvm_instr_emit_store(LLVM_Code_Generator* cg, Type_Instance* type_variable, Ast* value, u32 temp_reg);



void llvm_emit_function_attributes(LLVM_Code_Generator* cg);