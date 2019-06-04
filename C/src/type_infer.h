#pragma once
#include <common.h>
#include "type.h"
#include "top_typecheck.h"

Light_Type* type_infer_expression(Light_Ast* expr, u32* error);
Light_Type* type_infer_strenghten_type(Light_Scope* scope, Light_Type* type, u32* error);
Light_Type* type_infer_propagate(Light_Type* type, Light_Ast* expr, u32* error);

Light_Type_Check_Error type_infer_type_mismatch_error(Light_Token* location, Light_Type* left, Light_Type* right);