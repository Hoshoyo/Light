#pragma once
#include "type.h"
#include "type_table.h"
#include "ast.h"
#include "decl_check.h"
#include "error.h"

const u32 TYPE_INFER_REPORT_UNDECLARED = FLAG(0);
Type_Instance* infer_from_expression(Ast* expr, Type_Error* error, u32 flags);

Type_Instance* infer_from_binary_expression(Ast* expr, Type_Error* error, u32 flags);
Type_Instance* infer_from_literal_expression(Ast* expr, Type_Error* error, u32 flags);
Type_Instance* infer_from_procedure_call(Ast* expr, Type_Error* error, u32 flags);
Type_Instance* infer_from_unary_expression(Ast* expr, Type_Error* error, u32 flags);
Type_Instance* infer_from_variable_expression(Ast* expr, Type_Error* error, u32 flags);