#pragma once
#include "ast.h"
#include "type.h"
#include "type_table.h"

struct LLVM_Code_Generator {
	char* in_filename;

	char strlit_buffer[2048];
	char buffer[4096];
	u32  ptr;
	u32  strlit_ptr;

	s32  temp;
	s32  br_label_temp;
	s32  str_lit_temp;

	int sprint(char* msg, ...);
	int sprint_strlit(char* msg, ...);
	s32 alloc_temp_register();
	s32 gen_branch_label();
	s32 alloc_strlit_temp();
	void reset_temp();

	u32 string_literal_temp;
	Ast** deferred_string_literals;

	void llvm_emit_type_decls(Type_Instance** type_table);

	// literal strings
	s32  llvm_define_string_literal(Ast_Expr_Literal* lit);

	// expressions
	s32  llvm_emit_expression(Ast* expr);
	s32  llvm_emit_binary_expression(Ast* expr);
	s32  llvm_emit_assignment(Ast* expr);

	// general
	void llvm_emit_node(Ast* node);
	void llvm_emit_type(Type_Instance* type, u32 flags = 0);

	void llvm_generate_temporary_code();
};

void llvm_generate_ir(Ast** toplevel, Type_Instance** type_table, char* filename);

// helper functions
bool ast_node_is_embeded_literal(Ast* node);
bool ast_node_is_embeded_variable(Ast* node);