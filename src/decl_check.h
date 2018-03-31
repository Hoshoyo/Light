#pragma once

#include "ast.h"

typedef u32 Decl_Error;

const u32 DECL_OK		     = 0;
const u32 DECL_ERROR_FATAL	 = FLAG(1);
const u32 DECL_ERROR_WARNING = FLAG(2);
const u32 DECL_QUEUED_TYPE   = FLAG(3);

Decl_Error report_semantic_error(Decl_Error e, char* fmt, ...);
Decl_Error decl_check_top_level(Scope* global_scope, Ast** ast_top_level);
Ast*       decl_from_name(Scope* scope, Token* name);


void DEBUG_print_scope_decls(Scope* scope);