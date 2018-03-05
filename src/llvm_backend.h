#pragma once
#include "ast.h"
#include "type.h"

struct LLVM_Code_Generator {
	char buffer[4096];
	u32  ptr;
	u32  temp;

	int sprint(char* msg, ...);
	u32 alloc_temp();
	void reset_temp();

	u32 string_literal_temp;
	Ast** deferred_string_literals;

	void llvm_emit_type_decls(Type_Table* type_table);

	u32  llvm_emit_expression(Ast* expr);
	void llvm_emit_node(Ast* node);
	void llvm_emit_type(Type_Instance* type, u32 flags = 0);
};

void llvm_generate_ir(Ast** toplevel, Type_Table* type_table);