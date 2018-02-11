#pragma once
#include "ast.h"
#include "type.h"
#include "symbol_table.h"

struct Type_Checker 
{

};

struct Infer_Node {
	Ast* node;
};

extern Symbol_Table* symbol_table;
extern Infer_Node* infer_queue;

int check_and_submit_declarations(Ast** ast, Scope* global_scope);
int decl_type_inference(Ast** ast, Type_Table* table);
int do_type_inference(Ast** ast, Scope* global_scope, Type_Table* type_table);
int do_type_check(Ast** ast, Type_Table* type_table);
Ast* is_declared(Ast* node);

const u32 DECL_CHECK_PASSED = 1;
const u32 DECL_CHECK_FAILED = 0;