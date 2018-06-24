#pragma once
#include <stdio.h>
#include <ho_system.h>
#include "ast.h"
#include "lexer.h"
#include <stdarg.h>

typedef u32 Type_Error;

const u32 TYPE_OK			= 0;
const u32 TYPE_ERROR_FATAL = FLAG(1);
const u32 TYPE_QUEUED		= FLAG(2);

void report_error_location(Token* location);
void report_error_location(Ast*   location);

Type_Error report_type_error(Type_Error e, char* fmt, ...);
Type_Error report_type_error(Type_Error e, Token* location, char* fmt, ...);
Type_Error report_type_error(Type_Error e, Ast*   location, char* fmt, ...);

Type_Error report_decl_error(Type_Error e, char* fmt, ...);
Type_Error report_decl_error(Type_Error e, Token* location, char* fmt, ...);
Type_Error report_decl_error(Type_Error e, Ast*   location, char* fmt, ...);

Type_Error report_semantic_error(Type_Error e, char* fmt, ...);
Type_Error report_semantic_error(Type_Error e, Token* location, char* fmt, ...);
Type_Error report_semantic_error(Type_Error e, Ast*   location, char* fmt, ...);

void report_dependencies_error(Ast** infer_queue);