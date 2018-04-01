#pragma once
#include "type.h"
#include "type_table.h"
#include "ast.h"
#include "decl_check.h"

Type_Instance* infer_from_expression(Ast* expr, Decl_Error* error, bool report_undeclared, bool lval = false);