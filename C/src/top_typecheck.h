#pragma once
#include <common.h>
#include "ast.h"

typedef enum {
    TYPE_OK = 0,
    TYPE_FATAL    = (1 << 0),
    TYPE_ERROR    = (1 << 1),
    TYPE_WARNING  = (1 << 2),
} Light_Type_Error;

#define TYPE_STRONG(T) !((T)->flags & TYPE_FLAG_WEAK)
#define TYPE_WEAK(T) ((T)->flags & TYPE_FLAG_WEAK)

Light_Type_Error top_typecheck(Light_Ast** top_level, Light_Scope* global_scope);