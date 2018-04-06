#pragma once
#include "type.h"
#include "type_table.h"
#include "ast.h"
#include "decl_check.h"
#include "error.h"

Type_Instance* infer_from_expression(Ast* expr, Decl_Error* error, bool report_undeclared, bool lval = false);
Type_Instance* type_transform_weak_to_strong(Type_Instance* weak, Type_Instance* strong, Ast* expr, Decl_Error* error);
Type_Instance* type_strength_resolve(Type_Instance* t1, Type_Instance* t2, Ast* expr1, Ast* expr2, Decl_Error* error);
Decl_Error type_update_weak(Ast* expr, Type_Instance* strong);

Type_Error type_check(Scope* scope, Ast** ast);