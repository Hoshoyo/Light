#include <common.h>
#include "ast.h"

enum {
    EVAL_ERROR_REPORT = (1 << 0),
};

bool eval_expr_is_constant(Light_Ast* expr, u32 flags, u32* error);
s64  eval_expr_constant_int(Light_Ast* expr, u32* error);
bool eval_literal_primitive(Light_Ast* p);
void eval_directive_sizeof(Light_Ast* directive);
bool eval_directive_run(Light_Ast* directive) ;