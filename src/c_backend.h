#pragma once
#include "ast.h"
#include "type.h"
#include "type_table.h"

struct C_Code_Generator {
	char* in_filename;

	char* buffer;
	u32   ptr;

	char* defer_buffer;
	u32   defer_ptr;

	s64 loop_id;
	s64 alloc_loop_id();

	int sprint(char* msg, ...);
	int sprint_data(Ast_Data* data);
	bool deferring;

	void defer_flush();

    void emit_type(Type_Instance** type_table, Type_Instance* type, Token* name = 0);
	
	void emit_array_assignment(Type_Instance** type_table, Ast* decl);
	void emit_array_assignment_from_base(Type_Instance** type_table, s64 offset, Ast* expr);
	void emit_array_assignment_to_temp(Type_Instance** type_table, Ast* expr);
	
	void emit_struct_assignment(Type_Instance** type_table, Ast* decl);
	void emit_struct_assignment_from_base(Type_Instance** type_table, s64 offset, Ast* expr);
	void emit_struct_assignment_to_temp(Type_Instance** type_table, Ast* expr);

    void emit_decl(Type_Instance** type_table, Ast* decl, bool forward = false);
	void emit_proc(Type_Instance** type_table, Ast* decl);
	void emit_command(Type_Instance** type_table, Ast* comm);
	void emit_expression(Type_Instance** type_table, Ast* expr);
	void emit_expression_binary(Type_Instance** type_table, Ast* expr);
	void emit_temp_assignment(Type_Instance** type_table, Ast* expr);
	void emit_default_value(Type_Instance* type);
	void emit_typedef(Type_Instance** type_table, Type_Instance* type, Token* name, char* prefix);
	void emit_function_typedef(Type_Instance** type_table, Type_Instance* type);

	void emit_data_decl(Ast* decl);

    int  c_generate_top_level(Ast** toplevel, Type_Instance** type_table);
};

void c_generate(Ast** toplevel, Type_Instance** type_table, char* filename, char* compiler_path, string* libs_to_link);

extern bool print_debug_c;