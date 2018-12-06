#pragma once
#include "ast.h"
#include "error.h"

Type_Error     decl_check_top_level(Scope* global_scope, Ast** ast_top_level);
Type_Error     decl_insert_into_symbol_table(Ast* node, Token* name, char* descriptor);
Ast*           decl_from_name(Scope* scope, Token* name);
Ast*           decl_from_name_scoped(Scope* scope, Token* name);
Type_Instance* resolve_type(Scope* scope, Type_Instance* type, bool rep_undeclared, Type_Error* error);

Type_Error report_undeclared(Token* name);
Ast** type_decl_array_get();

void DEBUG_print_scope_decls(Scope* scope);
void DEBUG_print_scope_decls(Ast** top);