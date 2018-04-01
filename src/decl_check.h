#pragma once

#include "ast.h"

typedef u32 Decl_Error;

const u32 DECL_OK		     = 0;
const u32 DECL_ERROR_FATAL	 = FLAG(1);
const u32 DECL_ERROR_WARNING = FLAG(2);
const u32 DECL_QUEUED_TYPE   = FLAG(3);


Decl_Error decl_check_top_level(Scope* global_scope, Ast** ast_top_level);
Ast*       decl_from_name(Scope* scope, Token* name);
Type_Instance* resolve_type(Scope* scope, Type_Instance* type);


Decl_Error report_semantic_error(Decl_Error e, char* fmt, ...);
Decl_Error report_type_error(Decl_Error e, char* fmt, ...);
void report_error_location(Token* tok);
void report_error_location(Ast* node);

void DEBUG_print_scope_decls(Scope* scope);