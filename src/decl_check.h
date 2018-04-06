#pragma once
#include "ast.h"
#include "error.h"

Decl_Error     decl_check_top_level(Scope* global_scope, Ast** ast_top_level);
Ast*           decl_from_name(Scope* scope, Token* name);
Type_Instance* resolve_type(Scope* scope, Type_Instance* type, bool rep_undeclared);
Decl_Error     decl_insert_into_symbol_table(Ast* node, Token* name, char* descriptor);

Decl_Error report_undeclared(Token* name);

void DEBUG_print_scope_decls(Scope* scope);
void DEBUG_print_scope_decls(Ast** top);