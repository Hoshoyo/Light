#pragma once
#include "ast.h"

typedef enum {
    TYPE_CHECK_OK = 0,
    TYPE_CHECK_FATAL    = (1 << 0),
    TYPE_CHECK_ERROR    = (1 << 1),
    TYPE_CHECK_WARNING  = (1 << 2),
} Light_Type_Check_Error;

Light_Type_Check_Error top_typecheck(Light_Ast** top_level);