#include "ast.h"
#include "type.h"
#include "utils/utils.h"
#include "utils/allocator.h"
#include "eval.h"
#include <stdio.h>
#include <assert.h>
#include <light_array.h>

static int32_t
ast_new_id() {
    static int32_t id = 0;
    return id++;
}

static int32_t
scope_new_id() {
    static int32_t id = 1;
    return id++;
}

Light_Scope* 
light_scope_new(Light_Ast* creator_node, Light_Scope* parent, uint32_t flags) {
    Light_Scope* scope = light_alloc(sizeof(Light_Scope));
    
    assert(parent);

    scope->id = scope_new_id();
    scope->decl_count = 0;
    scope->creator_node = creator_node;
    scope->parent = parent;
    scope->flags = flags;
    scope->level = parent->level + 1;

    return scope;
}

Light_Ast* 
ast_new_expr_dot(Light_Scope* scope, Light_Ast* left, Light_Token* identifier) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_DOT;
    result->scope_at = scope;
    result->type = 0;
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_dot.left = left;
    result->expr_dot.identifier = identifier;

    return result;
}

Light_Ast* 
ast_new_expr_variable(Light_Scope* scope, Light_Token* name) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_VARIABLE;
    result->scope_at = scope;
    result->type = 0;
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_variable.name = name;
    result->expr_variable.decl = 0;

    return result;
}

Light_Ast* 
ast_new_expr_proc_call(Light_Scope* scope, Light_Ast* caller, Light_Ast** arguments, s32 args_count, Light_Token* op) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_PROCEDURE_CALL;
    result->scope_at = scope;
    result->type = 0;
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_proc_call.arg_count = args_count;
    result->expr_proc_call.args = arguments;
    result->expr_proc_call.caller_expr = caller;
    result->expr_proc_call.token = op;

    return result;
}

Light_Ast* 
ast_new_expr_unary(Light_Scope* scope, Light_Ast* operand, Light_Token* op_token, Light_Operator_Unary op) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_UNARY;
    result->scope_at = scope;
    result->type = 0;
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    if(operand->type && operand->type->flags & TYPE_FLAG_INTERNALIZED) {
        switch(op){
            case OP_UNARY_LOGIC_NOT:{
                result->type = type_primitive_get(TYPE_PRIMITIVE_BOOL);
            }break;
            case OP_UNARY_BITWISE_NOT:
            case OP_UNARY_MINUS:
            case OP_UNARY_PLUS:{
                result->type = operand->type;
            }break;
            case OP_UNARY_ADDRESSOF: {
                result->type = type_new_pointer(operand->type);
            }break;
            case OP_UNARY_DEREFERENCE:{
                if(operand->type->kind == TYPE_KIND_POINTER){
                    result->type = operand->type->pointer_to;
                }
            }break;
            case OP_UNARY_CAST:
                break;
            default: assert(0); break;
        }
    }
    
    result->expr_unary.flags = 0;
    result->expr_unary.op = op;
    result->expr_unary.operand = operand;
    result->expr_unary.token_op = op_token;
    result->expr_unary.type_to_cast = 0;

    return result;
}

Light_Ast* 
ast_new_expr_binary(Light_Scope* scope, Light_Ast* left, Light_Ast* right, Light_Token* op_token, Light_Operator_Binary op) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_BINARY;
    result->scope_at = scope;
    result->type = 0;
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_binary.left = left;
    result->expr_binary.right = right;
    result->expr_binary.op = op;
    result->expr_binary.token_op = op_token;

    return result;
}

Light_Ast* 
ast_new_expr_literal_array(Light_Scope* scope, Light_Token* token, Light_Ast** array_exprs) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_LITERAL;
    result->scope_at = scope;
    result->type = type_from_token(token);
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_literal.token = token;
    result->expr_literal.type = LITERAL_ARRAY;
    result->expr_literal.array_exprs = array_exprs;
    result->expr_literal.array_strong_type = 0;
    result->expr_literal.storage_class = STORAGE_CLASS_STACK;
    
    return result;
}

Light_Ast* 
ast_new_expr_literal_struct(Light_Scope* scope, Light_Token* token, Light_Ast** struct_exprs) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_LITERAL;
    result->scope_at = scope;
    result->type = type_from_token(token);
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_literal.type = LITERAL_STRUCT;
    result->expr_literal.token = token;
    result->expr_literal.struct_exprs = struct_exprs;
    result->expr_literal.storage_class = STORAGE_CLASS_STACK;
    
    return result;
}

Light_Ast*
ast_new_expr_literal_primitive(Light_Scope* scope, Light_Token* token) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_EXPRESSION_LITERAL;
    result->scope_at = scope;
    result->type = type_from_token(token);
    result->flags = AST_FLAG_EXPRESSION;
    result->id = ast_new_id();

    result->expr_literal.flags = 0;
    result->expr_literal.storage_class = STORAGE_CLASS_REGISTER;
    result->expr_literal.token = token;

    Light_Literal_Type type = 0;
    switch(token->type) {
        case TOKEN_LITERAL_BIN_INT:     type = LITERAL_BIN_INT; break;
        case TOKEN_LITERAL_BOOL_FALSE:
        case TOKEN_LITERAL_BOOL_TRUE:   type = LITERAL_BOOL; break;
        case TOKEN_LITERAL_CHAR:        type = LITERAL_CHAR; break;
        case TOKEN_LITERAL_DEC_INT:     type = LITERAL_DEC_SINT; break;
        case TOKEN_LITERAL_DEC_UINT:    type = LITERAL_DEC_UINT; break;
        case TOKEN_LITERAL_FLOAT:       type = LITERAL_FLOAT; break;
        case TOKEN_LITERAL_HEX_INT:     type = LITERAL_HEX_INT; break;
        case TOKEN_KEYWORD_NULL:        type = LITERAL_POINTER; break;
        default: type = 0; break;
    }
    result->expr_literal.type = type;

    if(result->type) {
        switch(result->type->kind) {
            case TYPE_KIND_POINTER: break; // value is 0
            case TYPE_KIND_PRIMITIVE:{
                eval_literal_primitive(result);
            }break;
            default: assert(0); break;
        }
    }    

    return result;
}

Light_Ast* 
ast_new_decl_typedef(Light_Scope* scope, Light_Type* type, Light_Token* name) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_TYPEDEF;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_typedef.name = name;
    result->decl_typedef.type_referenced = type;

    return result;
}

Light_Ast* 
ast_new_decl_variable(Light_Scope* scope, Light_Token* name, Light_Type* type, Light_Ast* expr, Light_Storage_Class storage, u32 flags) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_VARIABLE;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_variable.name = name;
    result->decl_variable.flags = flags;
    result->decl_variable.storage_class = storage;
    result->decl_variable.type = type;
    result->decl_variable.assignment = expr;

    return result;
}

Light_Ast* 
ast_new_decl_constant(Light_Scope* scope, Light_Token* name, Light_Type* type, Light_Ast* expr, u32 flags) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_CONSTANT;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_constant.name = name;
    result->decl_constant.flags = flags;
    result->decl_constant.type_info = type;
    result->decl_constant.value = expr;

    return result;
}

Light_Ast* 
ast_new_decl_procedure(
    Light_Scope* scope, Light_Token* name, Light_Ast* body, Light_Type* return_type, 
    Light_Scope* args_scope, Light_Ast_Decl_Variable** args, s32 args_count, u32 flags) 
{
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_PROCEDURE;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_proc.name = name;
    result->decl_proc.argument_count = args_count;
    result->decl_proc.arguments = args;
    result->decl_proc.body = body;
    result->decl_proc.arguments_scope = args_scope;
    result->decl_proc.flags = flags;
    result->decl_proc.return_type = return_type;
    result->decl_proc.proc_type = 0;

    return result;
}

Light_Ast* 
ast_new_comm_assignment(Light_Scope* scope, Light_Ast* lvalue, Light_Ast* rvalue, Light_Token* op_token) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_ASSIGNMENT;
    result->scope_at = scope;
    result->type = 0;
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_assignment.lvalue = lvalue;
    result->comm_assignment.rvalue = rvalue;
    result->comm_assignment.op_token = op_token;

    return result;
}

Light_Ast* 
ast_new_comm_block(Light_Scope* scope, Light_Ast** commands, s32 command_count, Light_Scope* block_scope) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_BLOCK;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_block.block_scope = block_scope;
    result->comm_block.command_count = command_count;
    result->comm_block.commands = commands;
    result->comm_block.defer_stack = 0;

    return result;
}

Light_Ast* 
ast_new_comm_if(Light_Scope* scope, Light_Ast* condition, Light_Ast* if_true, Light_Ast* if_false, Light_Token* if_token) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_IF;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_if.condition = condition;
    result->comm_if.body_true = if_true;
    result->comm_if.body_false = if_false;
    result->comm_if.if_token = if_token;

    return result;
}

Light_Ast* 
ast_new_comm_while(Light_Scope* scope, Light_Ast* condition, Light_Ast* body, Light_Token* while_token) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_WHILE;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_while.condition = condition;
    result->comm_while.body = body;
    result->comm_while.while_token = while_token;

    return result;
}


Light_Ast* 
ast_new_comm_for(Light_Scope* scope, Light_Scope* for_scope, Light_Ast* condition, Light_Ast* body, 
    Light_Ast** prologue, Light_Ast** epilogue, Light_Token* for_token) 
{
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_FOR;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_for.body = body;
    result->comm_for.condition = condition;
    result->comm_for.epilogue = epilogue;
    result->comm_for.prologue = prologue;
    result->comm_for.for_scope = for_scope;
    result->comm_for.for_token = for_token;

    return result;
}

Light_Ast* 
ast_new_comm_break(Light_Scope* scope, Light_Token* break_keyword, Light_Ast* level) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_BREAK;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_break.token_break = break_keyword;
    result->comm_break.level = level;

    return result;
}

Light_Ast* 
ast_new_comm_continue(Light_Scope* scope, Light_Token* continue_keyword, Light_Ast* level) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_CONTINUE;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_continue.token_continue = continue_keyword;
    result->comm_continue.level = level;

    return result;
}

Light_Ast* 
ast_new_comm_return(Light_Scope* scope, Light_Ast* expr, Light_Token* return_token) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_COMMAND_RETURN;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_COMMAND;
    result->id = ast_new_id();

    result->comm_return.expression = expr;
    result->comm_return.token_return = return_token;

    return result;
}



// Print
const char* ColorReset   = "\x1B[0m";
const char* ColorRed     = "\x1B[31m";
const char* ColorGreen   = "\x1B[32m";
const char* ColorYellow  = "\x1B[33m";
const char* ColorBlue    = "\x1B[34m";
const char* ColorMagenta = "\x1B[35m";
const char* ColorCyan    = "\x1B[36m";
const char* ColorWhite   = "\x1B[37m";

static s32
print_indent_level(FILE* out, s32 level) {
    s32 length = 0;
    // TODO(psv): more efficient
    for(s32 i = 0; i < level; ++i) {
        length += fprintf(out, "   ");
    }

    return length;
}
#define fprintf(F, ...) fprintf(F, __VA_ARGS__); fflush(F)
#define TOKEN_STR(T) (T)->length, (T)->data

static FILE* ast_file_from_flags(u32 flags) {
    FILE* out = 0;
    if(flags & LIGHT_AST_PRINT_STDOUT)
        out = stdout;
    else if(flags & LIGHT_AST_PRINT_STDERR)
        out = stderr;
    else
        out = 0;
    return out;
}

s32
ast_print_expr_literal(Light_Ast* expr, u32 flags, s32 indent_level) {
    assert(expr->kind == AST_EXPRESSION_LITERAL);
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    switch(expr->expr_literal.type) {
        case LITERAL_BIN_INT:
        case LITERAL_DEC_SINT:
        case LITERAL_DEC_UINT:
        case LITERAL_HEX_INT:
        case LITERAL_BOOL:
        case LITERAL_CHAR:
        case LITERAL_FLOAT:{
            length += fprintf(out, "%.*s", TOKEN_STR(expr->expr_literal.token));
        }break;
        case LITERAL_POINTER: length += fprintf(out, "null"); break;
        case LITERAL_ARRAY:{
            length += fprintf(out, "array {");
            for(u64 i = 0; i < array_length(expr->expr_literal.struct_exprs); ++i) {
                if(i != 0) {
                    length += fprintf(out, ", ");
                }
                length += ast_print_expression(expr->expr_literal.array_exprs[i], flags, indent_level);
            }
            length += fprintf(out, "}");
        }break;
        case LITERAL_STRUCT:{
            length += fprintf(out, "struct {");
            for(u64 i = 0; i < array_length(expr->expr_literal.struct_exprs); ++i) {
                if(i != 0) {
                    length += fprintf(out, ", ");
                }
                length += ast_print_expression(expr->expr_literal.struct_exprs[i], flags, indent_level);
            }
            length += fprintf(out, "}");
        }break;
        default: length += fprintf(out, "<invalid literal>"); break;
    }
    return length;
}

s32
ast_print_expr_binary(Light_Ast* expr, u32 flags, s32 indent_level) {
    assert(expr->kind == AST_EXPRESSION_BINARY);
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    length += ast_print_expression(expr->expr_binary.left, flags, indent_level);
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:    length += fprintf(out, " + "); break;
        case OP_BINARY_MINUS:   length += fprintf(out, " - "); break;
        case OP_BINARY_MULT:    length += fprintf(out, " * "); break;
        case OP_BINARY_DIV:     length += fprintf(out, " / "); break;
        case OP_BINARY_MOD:     length += fprintf(out, " & "); break;
        case OP_BINARY_AND:     length += fprintf(out, " %% "); break;
        case OP_BINARY_OR:      length += fprintf(out, " | "); break;
        case OP_BINARY_XOR:     length += fprintf(out, " ^ "); break;
        case OP_BINARY_SHL:     length += fprintf(out, " << "); break;
        case OP_BINARY_SHR:     length += fprintf(out, " >> "); break;

        case OP_BINARY_LT:          length += fprintf(out, " < "); break;
        case OP_BINARY_GT:          length += fprintf(out, " > "); break;
        case OP_BINARY_LE:          length += fprintf(out, " <= "); break;
        case OP_BINARY_GE:          length += fprintf(out, " >= "); break;
        case OP_BINARY_EQUAL:       length += fprintf(out, " == "); break;
        case OP_BINARY_NOT_EQUAL:   length += fprintf(out, " != "); break;

        case OP_BINARY_LOGIC_AND:   length += fprintf(out, "&&"); break;
        case OP_BINARY_LOGIC_OR:    length += fprintf(out, "||"); break;

        case OP_BINARY_VECTOR_ACCESS: length += fprintf(out, "["); break;
        default: length += fprintf(out, "<invalid binary expr>"); break;
    }
    length += ast_print_expression(expr->expr_binary.right, flags, indent_level);
    if(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS) {
        length += fprintf(out, "]");
    }

    return length;
}

s32
ast_print_expr_proc_call(Light_Ast* expr, u32 flags, s32 indent_level) {
    assert(expr->kind == AST_EXPRESSION_PROCEDURE_CALL);
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    length += ast_print_expression(expr->expr_proc_call.caller_expr, flags, indent_level);
    length += fprintf(out, "(");
    for(s32 i = 0; i < expr->expr_proc_call.arg_count; ++i) {
        length += ast_print_expression(expr->expr_proc_call.args[i], flags, indent_level);
    }
    length += fprintf(out, ")");

    return length;
}

s32
ast_print_expr_unary(Light_Ast* expr, u32 flags, s32 indent_level) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    switch(expr->expr_unary.op) {
        case OP_UNARY_ADDRESSOF:     length += fprintf(out, "&"); length += ast_print_expression(expr->expr_unary.operand, flags, indent_level); break;
        case OP_UNARY_BITWISE_NOT:   length += fprintf(out, "~"); length += ast_print_expression(expr->expr_unary.operand, flags, indent_level); break;
        case OP_UNARY_DEREFERENCE:   length += fprintf(out, "*"); length += ast_print_expression(expr->expr_unary.operand, flags, indent_level); break;
        case OP_UNARY_LOGIC_NOT:     length += fprintf(out, "!"); length += ast_print_expression(expr->expr_unary.operand, flags, indent_level); break;
        case OP_UNARY_MINUS:         length += fprintf(out, "-"); length += ast_print_expression(expr->expr_unary.operand, flags, indent_level); break;
        case OP_UNARY_PLUS:          length += fprintf(out, "+"); length += ast_print_expression(expr->expr_unary.operand, flags, indent_level); break;
        case OP_UNARY_CAST: {
            length += fprintf(out, "[");
            length += ast_print_type(expr->expr_unary.type_to_cast, flags, indent_level);
            length += fprintf(out, "]");
            length += ast_print_expression(expr->expr_unary.operand, flags, indent_level);
        }break;
        default: length += fprintf(out, "<invalid unary expr>"); break;
    }

    return length;
}

s32
ast_print_expression(Light_Ast* expr, u32 flags, s32 indent_level) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    //length += fprintf(out, "(");
    if(flags & LIGHT_AST_PRINT_EXPR_TYPES) {
        length += fprintf(out, "<");
        length += ast_print_type(expr->type, flags, indent_level);
        length += fprintf(out, ">");
    }

    switch(expr->kind) {
        case AST_EXPRESSION_BINARY:
            length += ast_print_expr_binary(expr, flags, indent_level);
            break;
        case AST_EXPRESSION_UNARY:
            length += ast_print_expr_unary(expr, flags, indent_level);
            break;
        case AST_EXPRESSION_DIRECTIVE:
            break;
        case AST_EXPRESSION_LITERAL:
            length += ast_print_expr_literal(expr, flags, indent_level);
            break;
        case AST_EXPRESSION_PROCEDURE_CALL:
            length += ast_print_expr_proc_call(expr, flags, indent_level);
            break;
        case AST_EXPRESSION_VARIABLE:
            length += fprintf(out, "%.*s", TOKEN_STR(expr->expr_variable.name));
            break;
        case AST_EXPRESSION_DOT:
            length += ast_print_expression(expr->expr_dot.left, flags, indent_level);
            length += fprintf(out, ".%.*s", TOKEN_STR(expr->expr_dot.identifier));
            break;
        default: length += fprintf(out, "<invalid expression>"); break;
    }

    //length += fprintf(out, ")");

    return length;
}

s32
ast_print_node(Light_Ast* node, u32 flags, s32 indent_level) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    switch(node->kind) {
        case AST_DECL_CONSTANT: {
            length += fprintf(out, "%.*s :", TOKEN_STR(node->decl_constant.name));
            if(node->decl_constant.type_info) {
                length += ast_print_type(node->decl_constant.type_info, flags, indent_level);
            }
            length += fprintf(out, ":");
            length += ast_print_expression(node->decl_constant.value, flags, indent_level);
        }break;
        case AST_DECL_TYPEDEF:{
            length += fprintf(out, "%.*s -> ", TOKEN_STR(node->decl_typedef.name));
            length += ast_print_type(node->decl_typedef.type_referenced, flags, indent_level);
            Light_Type* type = node->decl_typedef.type_referenced;
            if(type && type->kind != TYPE_KIND_ENUM && type->kind != TYPE_KIND_STRUCT && type->kind != TYPE_KIND_UNION) {
                length += fprintf(out, ";");
            }
        }break;
        case AST_DECL_VARIABLE:{
            length += fprintf(out, "%.*s :", TOKEN_STR(node->decl_variable.name));
            if(node->decl_variable.type) {
                length += ast_print_type(node->decl_variable.type, flags, indent_level);
            }
            if(node->decl_variable.assignment) {
                length += fprintf(out, " = ");
                length += ast_print_expression(node->decl_variable.assignment, flags, indent_level);
            }
        }break;
        case AST_DECL_PROCEDURE:{
            length += fprintf(out, "%.*s :: (", TOKEN_STR(node->decl_proc.name));
            for(s32 i = 0; i < node->decl_proc.argument_count; ++i) {
                length += ast_print_node((Light_Ast*)node->decl_proc.arguments[i], flags, indent_level);
            }
            length += fprintf(out, ") -> ");
            length += ast_print_type(node->decl_proc.return_type, flags, indent_level);
            if(node->decl_proc.body) {
                length += ast_print_node(node->decl_proc.body, flags, indent_level);
            } else {
                length += fprintf(out, ";");
            }
        }break;
        case AST_COMMAND_ASSIGNMENT:{
            if(node->comm_assignment.lvalue) {
                length += ast_print_expression(node->comm_assignment.lvalue, flags, indent_level);
                length += fprintf(out, " = ");
            }
            length += ast_print_expression(node->comm_assignment.rvalue, flags, indent_level);
            length += fprintf(out, ";");
        }break;
        case AST_COMMAND_BLOCK:{
            length += fprintf(out, "{\n");
            for(s32 i = 0; i < node->comm_block.command_count; ++i) {
                print_indent_level(out, indent_level + 1);
                length += ast_print_node(node->comm_block.commands[i], flags, indent_level + 1);
                length += fprintf(out, "\n");
            }
            print_indent_level(out, indent_level);
            length += fprintf(out, "}");
        }break;
        case AST_COMMAND_BREAK:{
            length += fprintf(out, "break");
            if(node->comm_break.level) {
                length += fprintf(out, " ");
                length += ast_print_expression(node->comm_break.level, flags, indent_level);
            }
            length += fprintf(out, ";");
        }break;
        case AST_COMMAND_CONTINUE:{
            length += fprintf(out, "continue");
            if(node->comm_continue.level) {
                length += fprintf(out, " ");
                length += ast_print_expression(node->comm_continue.level, flags, indent_level);
            }
            length += fprintf(out, ";");
        }break;
        case AST_COMMAND_FOR:{
            length += fprintf(out, "for ");
            if(node->comm_for.prologue){
                for(s32 i = 0; i < array_length(node->comm_for.prologue); ++i) {
                    if(i != 0) length += fprintf(out, ", ");
                    length += ast_print_node(node->comm_for.prologue[i], flags, indent_level);
                }
            }
            length += fprintf(out, ";");
            if(node->comm_for.condition)
                length += ast_print_expression(node->comm_for.condition, flags, indent_level);
            length += fprintf(out, ";");
            if(node->comm_for.epilogue) {
                for(s32 i = 0; i < array_length(node->comm_for.epilogue); ++i) {
                    if(i != 0) length += fprintf(out, ", ");
                    length += ast_print_node(node->comm_for.epilogue[i], flags, indent_level);
                }
            }
            length += ast_print_node(node->comm_for.body, flags, indent_level);
        }break;
        case AST_COMMAND_IF:{
            length += fprintf(out, "if ");
            length += ast_print_expression(node->comm_if.condition, flags, indent_level);
            length += ast_print_node(node->comm_if.body_true, flags, indent_level);
            if(node->comm_if.body_false) {
                length += fprintf(out, " else ");
                length += ast_print_node(node->comm_if.body_false, flags, indent_level);
            }
        }break;
        case AST_COMMAND_RETURN:{
            length += fprintf(out, "return");
            if(node->comm_return.expression) {
                length += fprintf(out, " ");
                length += ast_print_expression(node->comm_return.expression, flags, indent_level);
            }
            length += fprintf(out, ";");
        }break;
        case AST_COMMAND_WHILE:{
            length += fprintf(out, "while ");
            length += ast_print_expression(node->comm_while.condition, flags, indent_level);
            length += ast_print_node(node->comm_while.body, flags, indent_level);
        }break;
        default: length += fprintf(out, "<invalid node>"); break;
    }

    return length;
}


s32 
ast_print_type(Light_Type* type, u32 flags, s32 indent_level) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    if(!type) {
        return 0;
    }

    const char* color = 0;
    if(type->flags & TYPE_FLAG_INTERNALIZED) {
        fprintf(out, "%s", ColorGreen);
        color = ColorGreen;
    } else {
        fprintf(out, "%s", ColorRed);
        color = ColorRed;
    }

    if(!type) return 0;
    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE:{
            switch(type->primitive) {
                case TYPE_PRIMITIVE_VOID:   length += fprintf(out, "void"); break;
                case TYPE_PRIMITIVE_S8:     length += fprintf(out, "s8"); break;
                case TYPE_PRIMITIVE_S16:    length += fprintf(out, "s16"); break;
                case TYPE_PRIMITIVE_S32:    length += fprintf(out, "s32"); break;
                case TYPE_PRIMITIVE_S64:    length += fprintf(out, "s64"); break;
                case TYPE_PRIMITIVE_U8:     length += fprintf(out, "u8"); break;
                case TYPE_PRIMITIVE_U16:    length += fprintf(out, "u16"); break;
                case TYPE_PRIMITIVE_U32:    length += fprintf(out, "u32"); break;
                case TYPE_PRIMITIVE_U64:    length += fprintf(out, "u64"); break;
                case TYPE_PRIMITIVE_R32:    length += fprintf(out, "r32"); break;
                case TYPE_PRIMITIVE_R64:    length += fprintf(out, "r64"); break;
                case TYPE_PRIMITIVE_BOOL:   length += fprintf(out, "bool"); break;
                default: length += fprintf(out, "<invalid primitive type>"); break;
            }
        } break;
        case TYPE_KIND_POINTER:{
            length += fprintf(out, "^");
            length += ast_print_type(type->pointer_to, flags, indent_level);
        } break;
        case TYPE_KIND_ALIAS:{
            length += fprintf(out, "%.*s", type->alias.name->length, type->alias.name->data);
        } break;
        case TYPE_KIND_ARRAY:{
            length += fprintf(out, "[%llu]", type->array_info.dimension);
            length += ast_print_type(type->array_info.array_of, flags, indent_level);
        } break;
        case TYPE_KIND_FUNCTION:{
            length += fprintf(out, "(");
            for(s32 i = 0; i < type->function.arguments_count; ++i) {
                if(i != 0) length += fprintf(out, ", ");
                length += ast_print_type(type->function.arguments_type[i], flags, indent_level);
            }
            length += fprintf(out, "%s) -> ", color);
            length += ast_print_type(type->function.return_type, flags, indent_level);
        } break;
        case TYPE_KIND_STRUCT:{
            length += fprintf(out, "struct { ");
            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                Light_Ast* field = type->struct_info.fields[i];
                length += ast_print_node(field, flags, indent_level);
                length += fprintf(out, "%s; ", color);
            }
            length += fprintf(out, "%s }", color);
        } break;
        case TYPE_KIND_UNION: {
            length += fprintf(out, "union { ");
            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                Light_Ast* field = type->union_info.fields[i];
                length += ast_print_node(field, flags, indent_level);
                length += fprintf(out, "%s; ", color);
            }
            length += fprintf(out, "%s }", color);
        } break;
        case TYPE_KIND_ENUM: {
            length += fprintf(out, "enum ");
            length += ast_print_type(type->enumerator.type_hint, flags, indent_level);
            length += fprintf(out, "{ ");
            for(s32 i = 0; i < type->enumerator.field_count; ++i) {
                if(i != 0) length += fprintf(out, "%s, ", color);
                length += fprintf(out, "%.*s", type->enumerator.fields_names[i]->length, type->enumerator.fields_names[i]->data);
            }
            length += fprintf(out, "%s }", color);
        } break;
        case TYPE_KIND_NONE:
        default: length += fprintf(out, "<unknown type>"); break;
    }

    fprintf(out, "%s", ColorReset);
    return length;
}

s32
ast_print(Light_Ast** ast, u32 flags, s32 indent_level) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    if(!ast) return length;

    for(u64 i = 0; i < array_length(ast); ++i) {
        length += ast_print_node(ast[i], flags, indent_level);
        switch(ast[i]->kind) {
            case AST_DECL_CONSTANT:     length += fprintf(out, ";"); break;
            case AST_DECL_VARIABLE:     length += fprintf(out, ";"); break;
            case AST_DECL_PROCEDURE:
            case AST_DECL_TYPEDEF:
            break;
            default: break;
        }
        length += fprintf(out, "\n");
    }
    length += fprintf(out, "\n");
    return length;
}