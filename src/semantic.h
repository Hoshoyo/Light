#pragma once
#include "ast.h"
#include "type.h"
#include "symbol_table.h"

struct Type_Checker 
{

};

struct Infer_Node {
	Ast* node;
	Scope* scope;
};

extern Symbol_Table* symbol_table;
extern Infer_Node* infer_queue;

int check_and_submit_declarations(Ast** ast, Scope* global_scope);
int type_inference(Ast** ast, Scope* global_scope, Type_Table* table);
int do_type_inference(Ast** ast, Scope* global_scope, Type_Table* type_table);
int do_type_check(Ast** ast, Scope* global_scope, Type_Table* type_table);

const u32 DECL_CHECK_PASSED = 1;
const u32 DECL_CHECK_FAILED = 0;