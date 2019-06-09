#pragma once
#include <common.h>
#include "type.h"
#include "top_typecheck.h"

Light_Type* type_alias_root(Light_Type* type);

Light_Type* type_infer_expression(Light_Ast* expr, u32* error);
Light_Type* type_infer_propagate(Light_Type* type, Light_Ast* expr, u32* error);
Light_Ast*  type_infer_decl_from_name(Light_Scope* scope, Light_Token* name);
Light_Ast*  find_enum_field_decl(Light_Scope* scope, Light_Token* ident, u32* error);

Light_Type_Check_Error type_infer_type_mismatch_error(Light_Token* location, Light_Type* left, Light_Type* right);

