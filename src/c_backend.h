#pragma once
#include "ast.h"
#include "type.h"
#include "type_table.h"

struct C_Code_Generator {
	char* in_filename;

	char* buffer;
	u32   ptr;

	s64 loop_id;
	s64 alloc_loop_id();

	int sprint(char* msg, ...);


    void emit_type(Type_Instance* type, Token* name = 0);
	
	void emit_array_assignment(Ast* decl);
	void emit_array_assignment_from_base(s64 offset, Ast* expr);
	
	void emit_struct_assignment(Ast* decl);
	void emit_struct_assignment_from_base(s64 offset, Ast* expr);

    void emit_decl(Ast* decl, bool forward = false);
	void emit_proc(Ast* decl);
	void emit_command(Ast* comm);
	void emit_expression(Ast* expr);
	void emit_expression_binary(Ast* expr);
	void emit_default_value(Type_Instance* type);
	void emit_typedef(Type_Instance* type, Token* name, char* prefix);

	void emit_data_decl(Ast* decl);

    int  c_generate_top_level(Ast** toplevel, Type_Instance** type_table);
};

void c_generate(Ast** toplevel, Type_Instance** type_table, char* filename);