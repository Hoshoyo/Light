#include "ast.h"
#include "utils/allocator.h"

static int32_t
ast_new_id() {
    static id = 0;
    return id++;
}

Light_Ast* 
ast_new_typedef(Light_Scope* scope, Light_Type* type, Light_Token* name) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_TYPEDEF;
    result->scope_at = scope;
    result->type = 0;
    result->flags = 0;
    result->id = ast_new_id();

    result->decl_typedef.name = name;
    result->decl_typedef.type_referenced = type;

    return result;
}