#pragma once
#include <common.h>
#include "ast.h"

typedef enum {
    TYPE_OK = 0,
    TYPE_FATAL    = (1 << 0),
    TYPE_ERROR    = (1 << 1),
    TYPE_WARNING  = (1 << 2),
    TYPE_QUEUED   = (1 << 3),
} Light_Type_Error;

#define TYPE_STRONG(T) !((T)->flags & TYPE_FLAG_WEAK)
#define TYPE_WEAK(T) ((T)->flags & TYPE_FLAG_WEAK)

Light_Type_Error decl_check_redefinition(Light_Scope* scope, Light_Ast* node, Light_Token* token);
Light_Type_Error top_typecheck(Light_Ast** top_level, Light_Scope* global_scope);
void             typecheck_information_pass_decl(Light_Ast* node, u32 flags, u32* decl_error);
Light_Type*      typecheck_resolve_type(Light_Scope* scope, Light_Type* type, u32 flags, u32* error);