#pragma once
#include "ast.h"
#include "type.h"
#include "type_table.h"

struct C_Code_Generator {
	char* in_filename;

	char buffer[1 << 16];
	u32  ptr;

	int sprint(char* msg, ...);

    void emit_type(Type_Instance* type, Token* name = 0);
    void emit_decl(Ast* decl);
	void emit_proc(Ast* decl);
	void emit_command(Ast* comm);
	void emit_expression(Ast* expr);
	void emit_expression_binary(Ast* expr);
    int  c_generate_top_level(Ast** toplevel, Type_Instance** type_table);
};

void c_generate(Ast** toplevel, Type_Instance** type_table, char* filename);