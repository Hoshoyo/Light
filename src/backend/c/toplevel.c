#include "toplevel.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../../symbol_table.h"
#include "light_array.h"
#include "../../utils/catstring.h"

static void emit_typed_declaration(catstring* buffer, Light_Type* type, Light_Token* name, u32 flags);
static void emit_declaration(catstring* buffer, Light_Ast* node, u32 flags);
static void emit_expression(catstring* literal_decls, catstring* buffer, Light_Ast* node);

enum {
    REFLECT_TYPE_USER_TYPE_INFO_POINTER = 0,
    REFLECT_TYPE_COUNT,
};
static Light_Type* reflect_types[REFLECT_TYPE_COUNT] = {0};

static Light_Ast*
decl_from_name(Light_Scope* scope, Light_Token* name) {
    Light_Symbol s = {0};
    s.token = name;

    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;

    if(!symbol_table) return 0;

    s32 index = 0;
    if(symbol_table_entry_exist(symbol_table, s, &index, 0)) {
        return symbol_table->entries[index].data.decl;
    }
    return 0;
}

static Light_Token
token_from_name(char buffer[256], const char* name, int id) {
    int len = sprintf(buffer, "%s_%d", name, id);
    Light_Token result = {0};
    result.data = (u8*)buffer;
    result.length = len;
    result.type = TOKEN_IDENTIFIER;
    return result;
}

// Change level to parent scopes
static Light_Scope* 
loop_scope_level_change(Light_Scope* from, s64 level) {
    Light_Scope* result = from;
    while(result) {
        if(result->flags & SCOPE_LOOP) {
            level--;
            if(level == 0) break;
        }

        result = result->parent;
    }

    return result;
}

static void
emit_type_end(catstring* buffer, Light_Type* type) {
    switch(type->kind) {
        case TYPE_KIND_ARRAY:{
            if (type->array_info.dimension == 0) {
                catsprint(buffer, ")[]");
            } else {
                catsprint(buffer, ")[%u]", type->array_info.dimension);
            }
            emit_type_end(buffer, type->array_info.array_of);
        } break;
        case TYPE_KIND_FUNCTION:{
            catsprint(buffer, ")(");
            for(int i = 0; i < type->function.arguments_count; ++i) {
                if(i > 0) catsprint(buffer, ", ");

                if(type->function.arguments_names) {
                    Light_Token arg_name = {0};
                    arg_name.type = TOKEN_IDENTIFIER;
                    arg_name.data = (u8*)(type->function.arguments_names[i]);
                    arg_name.length = type->function.arguments_names_length[i];
                    emit_typed_declaration(buffer, type->function.arguments_type[i], &arg_name, 0);
                } else {
                    emit_typed_declaration(buffer, type->function.arguments_type[i], 0, 0);
                }
            }
            catsprint(buffer, ")");
            emit_type_end(buffer, type->function.return_type);
        } break;
        case TYPE_KIND_POINTER:{
            emit_type_end(buffer, type->pointer_to);
        } break;

        case TYPE_KIND_ALIAS:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_PRIMITIVE:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
            break;

        case TYPE_KIND_NONE:
        default: assert(0); break;
    }
}

enum {
    EMIT_FLAG_TREAT_AS_FUNCTION_TYPE = (1 << 0),
    EMIT_DECLARATION_DEFAULT_VALUE   = (1 << 1),
    EMIT_FLAG_ARRAY_AS_POINTER       = (1 << 2),
};

static void
emit_type_start(catstring* buffer, Light_Type* type, u32 flags) {
    switch(type->kind) {
        case TYPE_KIND_ALIAS:{
            Light_Type* root_type = type_alias_root(type);
            switch(root_type->kind) {
                case TYPE_KIND_STRUCT:
                    catsprint(buffer, "struct "); break;
                case TYPE_KIND_UNION:
                    catsprint(buffer, "union "); break;
                default: break;
            }
            catsprint_token(buffer, type->alias.name);
        } break;
        case TYPE_KIND_ARRAY:{
            emit_type_start(buffer, type->array_info.array_of, flags);
            catsprint(buffer, "(");
        } break;
        case TYPE_KIND_FUNCTION:{
            emit_type_start(buffer, type->function.return_type, 0);

            if (flags & EMIT_FLAG_TREAT_AS_FUNCTION_TYPE) {
                catsprint(buffer, "(");
            } else {
                catsprint(buffer, "(*");
            }
        } break;
        case TYPE_KIND_POINTER:{
            if(type->pointer_to->kind == TYPE_KIND_STRUCT)
                catsprint(buffer, "struct ");
            if(type->pointer_to->kind == TYPE_KIND_UNION)
                catsprint(buffer, "union ");
            emit_type_start(buffer, type->pointer_to, flags);
            catsprint(buffer, "*");
        } break;
        case TYPE_KIND_PRIMITIVE:{
            switch(type->primitive){
                case TYPE_PRIMITIVE_BOOL:
                    catsprint(buffer, "bool"); break;
                case TYPE_PRIMITIVE_R32:
                    catsprint(buffer, "r32"); break;
                case TYPE_PRIMITIVE_R64:
                    catsprint(buffer, "r64"); break;
                case TYPE_PRIMITIVE_S8:
                    catsprint(buffer, "s8"); break;
                case TYPE_PRIMITIVE_S16:
                    catsprint(buffer, "s16"); break;
                case TYPE_PRIMITIVE_S32:
                    catsprint(buffer, "s32"); break;
                case TYPE_PRIMITIVE_S64:
                    catsprint(buffer, "s64"); break;
                case TYPE_PRIMITIVE_U8:
                    catsprint(buffer, "u8"); break;
                case TYPE_PRIMITIVE_U16:
                    catsprint(buffer, "u16"); break;
                case TYPE_PRIMITIVE_U32:
                    catsprint(buffer, "u32"); break;
                case TYPE_PRIMITIVE_U64:
                    catsprint(buffer, "u64"); break;
                case TYPE_PRIMITIVE_VOID:
                    catsprint(buffer, "void"); break;
                default: assert(0); break;
            }
        } break;
        case TYPE_KIND_STRUCT:{
            catsprint(buffer, " { ");
            for(int i = 0; i < type->struct_info.fields_count; ++i) {
                Light_Ast* field = type->struct_info.fields[i];
                emit_typed_declaration(buffer, field->decl_variable.type, field->decl_variable.name, 0);
                catsprint(buffer, "; ");
            }
            catsprint(buffer, "}");
        } break;
        case TYPE_KIND_UNION: {
            catsprint(buffer, " { ");
            for(int i = 0; i < type->union_info.fields_count; ++i) {
                Light_Ast* field = type->union_info.fields[i];
                emit_typed_declaration(buffer, field->decl_variable.type, field->decl_variable.name, 0);
                catsprint(buffer, "; ");
            }
            catsprint(buffer, "}");
        } break;
        case TYPE_KIND_ENUM: break;
        default: assert(0); break;
    }
}

static void
emit_typed_procedure_declaration(catstring* buffer, Light_Ast* decl) {
    assert(decl->kind == AST_DECL_PROCEDURE);

    Light_Type* type = decl->decl_proc.proc_type;
    // start
    emit_type_start(buffer, type, EMIT_FLAG_TREAT_AS_FUNCTION_TYPE);
    catsprint(buffer, " ");
    
    // name
    if(decl->decl_proc.name && decl->decl_proc.flags & DECL_PROC_FLAG_MAIN) {
        catsprint(buffer, "__light_main");
    } else {
        catsprint_token(buffer, decl->decl_proc.name);
    }

    bool variadic = decl->decl_proc.flags & DECL_PROC_FLAG_VARIADIC;
    bool external = (decl->decl_proc.flags & DECL_PROC_FLAG_EXTERN);

    // end (arguments)
    catsprint(buffer, ")(");
    for(int i = 0; i < decl->decl_proc.argument_count - ((variadic && external) ? 1 : 0); ++i) {
        if(i > 0) catsprint(buffer, ", ");
        
        emit_typed_declaration(buffer, decl->decl_proc.arguments[i]->decl_variable.type, 
            decl->decl_proc.arguments[i]->decl_variable.name, 0);
    }
    if(variadic && external) {
        catsprint(buffer, ", ...");
    }
    catsprint(buffer, ")");
    emit_type_end(buffer, type->function.return_type);
}

static void
emit_typed_declaration(catstring* buffer, Light_Type* type, Light_Token* name, u32 flags) {
    switch(type->kind) {
        case TYPE_KIND_ALIAS: {
            catsprint_token(buffer, type->alias.name);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
        } break;

        case TYPE_KIND_POINTER:
        case TYPE_KIND_FUNCTION: {
            emit_type_start(buffer, type, flags);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
            emit_type_end(buffer, type);
        } break;
        case TYPE_KIND_ARRAY:{
            Light_Type arrptr = *type;
            if(flags & EMIT_FLAG_ARRAY_AS_POINTER) {
                arrptr.kind = TYPE_KIND_POINTER;
                arrptr.pointer_to = type->array_info.array_of;
            }
            emit_type_start(buffer, &arrptr, flags);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
            emit_type_end(buffer, &arrptr);
        } break;

        case TYPE_KIND_PRIMITIVE:
        {
            emit_type_start(buffer, type, flags);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
        } break;
        case TYPE_KIND_UNION:
        {
            catsprint(buffer, "union ");
            if(name)
                catsprint_token(buffer, name);
            emit_type_start(buffer, type, flags);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
        } break;
        case TYPE_KIND_STRUCT:
        {
            catsprint(buffer, "struct ");
            if(name)
                catsprint_token(buffer, name);
            emit_type_start(buffer, type, flags);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
        } break;

        case TYPE_KIND_ENUM:
            break;
        case TYPE_KIND_NONE:
        default: assert(0); break;
    }
}

static void
emit_binop(catstring* buffer, Light_Operator_Binary binop) {
    switch(binop) {
        case OP_BINARY_PLUS:    catsprint(buffer, " + "); break;
        case OP_BINARY_MINUS:   catsprint(buffer, " - "); break;
        case OP_BINARY_MULT:    catsprint(buffer, " * "); break;
        case OP_BINARY_DIV:     catsprint(buffer, " / "); break;
        case OP_BINARY_MOD:     catsprint(buffer, " %% "); break;
        case OP_BINARY_AND:     catsprint(buffer, " & "); break;
        case OP_BINARY_OR:      catsprint(buffer, " | "); break;
        case OP_BINARY_XOR:     catsprint(buffer, " ^ "); break;
        case OP_BINARY_SHL:     catsprint(buffer, " << "); break;
        case OP_BINARY_SHR:     catsprint(buffer, " >> "); break;

        case OP_BINARY_LT:          catsprint(buffer, " < "); break;
        case OP_BINARY_GT:          catsprint(buffer, " > "); break;
        case OP_BINARY_LE:          catsprint(buffer, " <= "); break;
        case OP_BINARY_GE:          catsprint(buffer, " >= "); break;
        case OP_BINARY_EQUAL:       catsprint(buffer, " == "); break;
        case OP_BINARY_NOT_EQUAL:   catsprint(buffer, " != "); break;

        case OP_BINARY_LOGIC_AND:   catsprint(buffer, " && "); break;
        case OP_BINARY_LOGIC_OR:    catsprint(buffer, " || "); break;

        case OP_BINARY_VECTOR_ACCESS: // TODO(psv):
        default: break;
    }
}

static void
emit_primitive_literal(catstring* buffer, Light_Ast_Expr_Literal_Primitive lit, Light_Type* type) {
    type = type_alias_root(type);
    switch(lit.type) {
        case LITERAL_BIN_INT:
        case LITERAL_HEX_INT:
        case LITERAL_DEC_SINT:
        case LITERAL_DEC_UINT:{
            switch(type->primitive) {
                case TYPE_PRIMITIVE_S8:
                    catsprint(buffer, "%l", (s64)lit.value_s8); break;
                case TYPE_PRIMITIVE_S16:
                    catsprint(buffer, "%l", (s64)lit.value_s16); break;
                case TYPE_PRIMITIVE_S32:
                    catsprint(buffer, "%l", (s64)lit.value_s32); break;
                case TYPE_PRIMITIVE_S64:
                    catsprint(buffer, "%l", lit.value_s64); break;
                case TYPE_PRIMITIVE_U8:
                    catsprint(buffer, "%x", (u64)lit.value_u8); break;
                case TYPE_PRIMITIVE_U16:
                    catsprint(buffer, "%x", (u64)lit.value_u16); break;
                case TYPE_PRIMITIVE_U32:
                    catsprint(buffer, "%x", (u64)lit.value_u32); break;
                case TYPE_PRIMITIVE_U64:
                    catsprint(buffer, "%x", lit.value_u64); break;
                default: break;
            }
        } break;
        case LITERAL_BOOL:{
            if (lit.value_bool) {
                catsprint(buffer, "true");
            } else {
                catsprint(buffer, "false");
            }
        } break;
        case LITERAL_CHAR:{
            catsprint(buffer, "%x", lit.value_u32);
        } break;
        case LITERAL_FLOAT:{
            if(type->primitive == TYPE_PRIMITIVE_R32)
                catsprint(buffer, "%f", lit.value_r32);
            else if(type->primitive == TYPE_PRIMITIVE_R64)
                catsprint(buffer, "%f", lit.value_r64);
        } break;
        case LITERAL_POINTER:
            catsprint(buffer, "0"); break;
        default: break;
    }
}

static void
emit_expression_binary(catstring* literal_decls, catstring* buffer, Light_Ast* node) {
    assert(node->kind == AST_EXPRESSION_BINARY);

    catsprint(buffer, "(");
    if(node->expr_binary.op == OP_BINARY_VECTOR_ACCESS) {
        emit_expression(literal_decls, buffer, node->expr_binary.left);
        catsprint(buffer, "[");
        emit_expression(literal_decls, buffer, node->expr_binary.right);
        catsprint(buffer, "]");
    } else {
        
        //if(node->expr_binary.left->type )
        if(node->expr_binary.left->type->kind == TYPE_KIND_POINTER && node->expr_binary.left->type->pointer_to == type_primitive_get(TYPE_PRIMITIVE_VOID))
        {
            catsprint(buffer, "((char*)(");
            emit_expression(literal_decls, buffer, node->expr_binary.left);
            catsprint(buffer, "))");
        }
        else
            emit_expression(literal_decls, buffer, node->expr_binary.left);
        
        emit_binop(buffer, node->expr_binary.op);
        emit_expression(literal_decls, buffer, node->expr_binary.right);
    }
    catsprint(buffer, ")");
}

static void
emit_expression_unary(catstring* literal_decls, catstring* buffer, Light_Ast* node) {
    assert(node->kind == AST_EXPRESSION_UNARY);

    switch(node->expr_unary.op) {
        case OP_UNARY_LOGIC_NOT:
            catsprint(buffer, "(!");
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            catsprint(buffer, ")");
            break;
        case OP_UNARY_MINUS:
            catsprint(buffer, "(-");
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            catsprint(buffer, ")");
            break;
        case OP_UNARY_BITWISE_NOT:
            catsprint(buffer, "(~");
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            catsprint(buffer, ")");
            break;
        case OP_UNARY_PLUS:
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            break;
        case OP_UNARY_ADDRESSOF:
            catsprint(buffer, "(&");
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            catsprint(buffer, ")");
            break;
        case OP_UNARY_DEREFERENCE:
            catsprint(buffer, "(*");
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            catsprint(buffer, ")");
            break;
        case OP_UNARY_CAST:
            catsprint(buffer, "((");
            emit_typed_declaration(buffer, node->expr_unary.type_to_cast, 0, EMIT_FLAG_ARRAY_AS_POINTER);
            catsprint(buffer, ")");
            emit_expression(literal_decls, buffer, node->expr_unary.operand);
            catsprint(buffer, ")");
            break;

        default: assert(0); break;
    }
}

static void 
emit_expression(catstring* literal_decls, catstring* buffer, Light_Ast* node) {
    assert(node->flags & AST_FLAG_EXPRESSION);
    
    switch(node->kind) {
        case AST_EXPRESSION_BINARY:{
            emit_expression_binary(literal_decls, buffer, node);
        } break;
        case AST_EXPRESSION_PROCEDURE_CALL: {
            catsprint(buffer, "(");
            emit_expression(literal_decls, buffer, node->expr_proc_call.caller_expr);
            catsprint(buffer, ")(");
            for(int i = 0; i < node->expr_proc_call.arg_count; ++i) {
                if(i > 0) catsprint(buffer, ",");
                emit_expression(literal_decls, buffer, node->expr_proc_call.args[i]);
            }
            catsprint(buffer, ")");
        } break;
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            emit_primitive_literal(buffer, node->expr_literal_primitive, node->type);
        } break;
        case AST_EXPRESSION_VARIABLE: {
            if(node->expr_variable.decl->kind == AST_DECL_CONSTANT) {
                // Constants are not declared, so use the value directly
                emit_expression(literal_decls, buffer, node->expr_variable.decl->decl_constant.value);
            } else {
                // Is a variable, therefore it is declared
                if(node->expr_variable.decl->kind == AST_DECL_PROCEDURE && 
                    node->expr_variable.decl->decl_proc.flags & DECL_PROC_FLAG_MAIN)
                {
                    catsprint(buffer, "__light_main");
                } else {
                    catsprint_token(buffer, node->expr_variable.name);
                }
            }
        } break;
        case AST_EXPRESSION_DOT:{
            Light_Type* root_type = type_alias_root(node->expr_dot.left->type);
            if(root_type->kind == TYPE_KIND_ENUM) {
                Light_Ast* c = decl_from_name(root_type->enumerator.enum_scope, node->expr_dot.identifier);
                assert(c->kind == AST_DECL_CONSTANT);
                emit_expression(literal_decls, buffer, c->decl_constant.value);
            } else {
                if(node->expr_dot.left->type->kind == TYPE_KIND_POINTER) {
                    catsprint(buffer, "(");
                    emit_expression(literal_decls, buffer, node->expr_dot.left);
                    catsprint(buffer, ")->");
                } else {
                    catsprint(buffer, "(");
                    emit_expression(literal_decls, buffer, node->expr_dot.left);
                    catsprint(buffer, ").");
                }
                catsprint_token(buffer, node->expr_dot.identifier);
            }
        } break;
        case AST_EXPRESSION_LITERAL_ARRAY: {
            catstring after = {0};
            catstring epilogue = {0};
            catstring discard = {0};

            char b[256] = {0};
            Light_Token name = token_from_name(b, "_lit_array", node->id);
            emit_typed_declaration(&after, node->type, &name, 0);

            if(node->expr_literal_array.raw_data) {
                //catsprint(&after, " = \"%s*\";\n", node->expr_literal_array.data_length_bytes, node->expr_literal_array.data);
                catsprint(&after, " = \"");
                for(int i = 0; i < node->expr_literal_array.data_length_bytes; ++i) {
                    char b[] = "\\x00";
                    sprintf(b, "\\x%02x", (node->expr_literal_array.data[i]));
                    catsprint(&after, "%s", b);
                }
                catsprint(&after, "\";\n");
            } else {
                catsprint(&after, " = {");
                for(u64 i = 0; i < array_length(node->expr_literal_array.array_exprs); ++i) {
                    if(i > 0) catsprint(&after, ", ");
                    Light_Ast* expr = node->expr_literal_array.array_exprs[i];
                    if(expr->kind == AST_EXPRESSION_LITERAL_ARRAY) {
                        catsprint(&after, "{0}");
                        // TODO(psv): alignment and padding are important here
                        u64 size_element_bytes = node->type->array_info.array_of->size_bits / 8;
                        catsprint(&epilogue, "__memory_copy((u8*)_lit_array_%d + %l, _lit_array_%d, %l);\n", 
                            node->id, i * size_element_bytes, expr->id, expr->type->size_bits / 8);
                        // emit anyway, but discard the rvalue since were are memcopying
                        emit_expression(literal_decls, &discard, expr);
                    } else if (expr->kind == AST_EXPRESSION_VARIABLE && expr->type->kind == TYPE_KIND_ARRAY) {
                       catsprint(&after, "{0}");
                       u64 size_element_bytes = node->type->array_info.array_of->size_bits / 8;
                       catsprint(&epilogue, "__memory_copy((u8*)_lit_array_%d + %l, (u8*)(%s+), %l);\n", 
                            node->id, i * size_element_bytes, 
                            expr->expr_variable.name->length, 
                            expr->expr_variable.name->data, 
                            expr->type->size_bits / 8); 
                    } else {
                        emit_expression(literal_decls, &after, expr);
                    }
                }
                catsprint(&after, "};\n");
            }

            catstring_append(literal_decls, &after);
            catstring_append(literal_decls, &epilogue);

            // Dereference, since the array is already considered a pointer type
            catsprint(buffer, "_lit_array_%d", node->id);
        } break;
        case AST_EXPRESSION_LITERAL_STRUCT:{
            catstring after = {0};
            catstring arrays = {0};

            char b[256] = {0};
            Light_Token name = token_from_name(b, "_lit_struct", node->id);
            emit_typed_declaration(&after, node->type, &name, 0);
            catsprint(&after, " = {");

            for(u64 i = 0; i < array_length(node->expr_literal_struct.struct_exprs); ++i) {
                if(i > 0) catsprint(&after, ", ");
                Light_Ast* expr = node->expr_literal_struct.struct_exprs[i];
                if(expr->kind == AST_EXPRESSION_LITERAL_ARRAY) {
                    catsprint(&after, "{0}");
                    catsprint(&arrays, "__memory_copy(&_lit_struct_%d, ", node->id);
                    emit_expression(literal_decls, &arrays, expr);
                    catsprint(&arrays, ", %l);\n", expr->type->size_bits / 8);
                } else {
                    emit_expression(literal_decls, &after, expr);
                }
            }

            catstring_append(literal_decls, &after);

            catsprint(literal_decls, "};\n");

            catstring_append(literal_decls, &arrays);

            catsprint(buffer, "_lit_struct_%d", node->id);
        } break;
        case AST_EXPRESSION_UNARY:{
            emit_expression_unary(literal_decls, buffer, node);
        } break;
        case AST_EXPRESSION_COMPILER_GENERATED: {
            switch(node->expr_compiler_generated.kind) {
                case COMPILER_GENERATED_TYPE_VALUE_POINTER:
                    catsprint(buffer, "0");
                    break;
                case COMPILER_GENERATED_USER_TYPE_INFO_POINTER: {
                    Light_Type* user_info_ptr_type = reflect_types[REFLECT_TYPE_USER_TYPE_INFO_POINTER];
                    catsprint(buffer, "&__light_type_table[%u]", user_info_ptr_type->type_table_index);
                } break;
                case COMPILER_GENERATED_POINTER_TO_TYPE_INFO: {
                    catsprint(buffer, "&__light_type_table[%u]", node->expr_compiler_generated.type_value->type_table_index);
                } break;
                default: assert(0); break;
            }
        } break;
        default: assert(0); break;
    }
}

static void
emit_variable_assignment(catstring* buffer, Light_Token* name, Light_Ast* expr) {
    catstring assignment = {0};

    Light_Type* root_type = type_alias_root(expr->type);
    switch(root_type->kind) {
        case TYPE_KIND_POINTER:
        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_PRIMITIVE:{
            catsprint_token(&assignment, name);
            catsprint(&assignment, " = ");
            emit_expression(buffer, &assignment, expr);
            catstring_append(buffer, &assignment);
        } break;
            
        case TYPE_KIND_STRUCT:{
            catsprint_token(&assignment, name);
            catsprint(&assignment, " = ");
            emit_expression(buffer, &assignment, expr);
            catstring_append(buffer, &assignment);
        } break;
        case TYPE_KIND_ARRAY: {
            catsprint(&assignment, "__memory_copy(");
            catsprint_token(&assignment, name);
            catsprint(&assignment, ", ");

            emit_expression(buffer, &assignment, expr);
            catstring_append(buffer, &assignment);

            catsprint(buffer, ", %l)", root_type->size_bits/8);
        } break;
        case TYPE_KIND_UNION:
            // TODO(psv): implement
        default: break;
    }
}

static void
emit_variable_assignment_top_level(catstring* buffer, catstring* top_level, Light_Token* name, Light_Ast* expr) {
    Light_Type* root_type = type_alias_root(expr->type);
    switch(root_type->kind) {
        case TYPE_KIND_POINTER:
        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_PRIMITIVE:{
            catsprint_token(buffer, name);
            catsprint(buffer, " = ");
            emit_expression(top_level, buffer, expr);
        } break;
            
        case TYPE_KIND_STRUCT:{
            catsprint_token(buffer, name);
            catsprint(buffer, " = ");
            emit_expression(top_level, buffer, expr);
        } break;
        case TYPE_KIND_ARRAY: {
            catsprint(buffer, "__memory_copy(");
            catsprint_token(buffer, name);
            catsprint(buffer, ", ");

            emit_expression(top_level, buffer, expr);
            catstring_append(top_level, buffer);

            catsprint(buffer, ", %l)", root_type->size_bits/8);
        } break;
        case TYPE_KIND_UNION:
            // TODO(psv): implement
        default: break;
    }
}

static void
emit_command(catstring* buffer, Light_Ast* node) {
    switch(node->kind){
        case AST_DECL_VARIABLE:{
            emit_declaration(buffer, node, EMIT_DECLARATION_DEFAULT_VALUE);
            if(node->decl_variable.assignment) {
                emit_variable_assignment(buffer, node->decl_variable.name, node->decl_variable.assignment);
                catsprint(buffer, ";\n");
            }
        } break;
        case AST_COMMAND_BLOCK:{
            catsprint(buffer, "{\n");

            for(int i = 0; i < node->comm_block.command_count; ++i) {
                Light_Ast* n = node->comm_block.commands[i];
                if(n->flags & AST_FLAG_COMMAND) {
                    emit_command(buffer, node->comm_block.commands[i]);
                } else if(n->kind == AST_DECL_VARIABLE) {
                    emit_declaration(buffer, node->comm_block.commands[i], EMIT_DECLARATION_DEFAULT_VALUE);

                    // When there is an assignment
                    if(n->decl_variable.assignment) {
                        emit_variable_assignment(buffer, 
                            n->decl_variable.name,
                            n->decl_variable.assignment);
                        catsprint(buffer, ";\n");
                    }
                } else if(n->kind == AST_DECL_PROCEDURE) {
                    emit_declaration(buffer, node->comm_block.commands[i], EMIT_FLAG_TREAT_AS_FUNCTION_TYPE);
                }
            }
            
            catsprint(buffer, "}\n");
        } break;
        
        case AST_COMMAND_ASSIGNMENT:{
            bool arraytype = type_alias_root(node->comm_assignment.rvalue->type)->kind == TYPE_KIND_ARRAY;

            catstring c = {0};
            if(arraytype) {
                assert(node->comm_assignment.lvalue);
                catsprint(&c, "__memory_copy(");
                emit_expression(buffer, &c, node->comm_assignment.lvalue);
                catsprint(&c, ", ");
                emit_expression(buffer, &c, node->comm_assignment.rvalue);
                catsprint(&c, ", %l)", node->comm_assignment.rvalue->type->size_bits / 8);
            } else {
                if(node->comm_assignment.lvalue) {
                    emit_expression(buffer, &c, node->comm_assignment.lvalue);
                    catsprint(&c, " = ");
                }
                emit_expression(buffer, &c, node->comm_assignment.rvalue);

            }
            catstring_append(buffer, &c);
            catsprint(buffer, ";\n");
        } break;
        
        case AST_COMMAND_IF:{
            bool body_true_is_block = node->comm_if.body_true && node->comm_if.body_true->kind == AST_COMMAND_BLOCK;
            bool body_false_is_block = node->comm_if.body_false && node->comm_if.body_false->kind == AST_COMMAND_BLOCK;

            // Conditional expression
            catstring c = { 0 };
            catsprint(&c, "if (");
            emit_expression(buffer, &c, node->comm_if.condition);
            catsprint(&c, ")");
            catstring_append(buffer, &c);

            // True body
            if (!body_true_is_block)
                catsprint(buffer, "{\n");
            emit_command(buffer, node->comm_if.body_true);
            if (!body_true_is_block)
                catsprint(buffer, "}");

            // False body
            if (node->comm_if.body_false) {
                catsprint(buffer, " else ");
                if (!body_false_is_block)
                    catsprint(buffer, "{\n");
                emit_command(buffer, node->comm_if.body_false);
                if (!body_false_is_block)
                    catsprint(buffer, "}");
            }
            catsprint(buffer, "\n");
        } break;

        case AST_COMMAND_WHILE:{
            // label to continue
            assert(node->comm_while.body->comm_block.block_scope);
            catsprint(buffer, "label_continue_%d:\n", (u64)node->comm_while.body->comm_block.block_scope->id);

            catstring c = { 0 };
            catsprint(&c, "while (");
            emit_expression(buffer, &c, node->comm_while.condition);
            catsprint(&c, ") {\n");
            catstring_append(buffer, &c);
            emit_command(buffer, node->comm_while.body);
            catsprint(buffer, "}\n");

            catsprint(buffer, "label_break_%d:;\n", (u64)node->comm_while.body->comm_block.block_scope->id);
        } break;
        
        case AST_COMMAND_RETURN:{
            catstring c = {0};
            catsprint(&c, "return ");
            if(node->comm_return.expression) {
                emit_expression(buffer, &c, node->comm_return.expression);
            }
            catstring_append(buffer, &c);
            catsprint(buffer, ";\n");
        } break;

        case AST_COMMAND_BREAK:{
            Light_Scope* scope = loop_scope_level_change(node->scope_at, node->comm_break.level_value);
            catsprint(buffer, "goto label_break_%d;\n", scope->id);
        } break;
        case AST_COMMAND_CONTINUE:{
            Light_Scope* scope = loop_scope_level_change(node->scope_at, node->comm_continue.level_value);
            catsprint(buffer, "goto label_continue_%d;\n", scope->id);
        } break;
        case AST_COMMAND_FOR:{            
            catsprint(buffer, "{\n");

            // first declarations (prologue)
            if(node->comm_for.prologue) {
                for(u64 i = 0; i < array_length(node->comm_for.prologue); ++i) {
                    emit_command(buffer, node->comm_for.prologue[i]);
                }
            }

            catstring c = { 0 };
            catsprint(&c, "while(");
            if(node->comm_for.condition) {
                emit_expression(buffer, &c, node->comm_for.condition);
            } else {
                catsprint(&c, "true");    
            }
            catsprint(&c, ")");
            catstring_append(buffer, &c);

            catsprint(buffer, "{");
            emit_command(buffer, node->comm_for.body);
            
            catsprint(buffer, "label_continue_%d:\n", node->comm_for.for_scope->id);

            // epilogue
            if(node->comm_for.epilogue) {
                for(u64 i = 0; i < array_length(node->comm_for.epilogue); ++i) {
                    emit_command(buffer, node->comm_for.epilogue[i]);
                }
            }

            catsprint(buffer, "}");
            catsprint(buffer, "}\n");

            catsprint(buffer, "label_break_%d:;\n", node->comm_for.for_scope->id);
        } break;
        default: break;
    }
}

static void
emit_default_value_for_type(catstring* buffer, Light_Type* type) {
    type = type_alias_root(type);
    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_ENUM:
            catsprint(buffer, "0"); break;
        case TYPE_KIND_ARRAY:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
            catsprint(buffer, "{0}"); break;
        default: catsprint(buffer, "0"); break;
    }
}

static void
emit_proc_forward_decl(catstring* buffer, Light_Ast* node) {
    emit_typed_procedure_declaration(buffer, node);
    catsprint(buffer, ";\n");
}

static void
emit_declaration(catstring* buffer, Light_Ast* node, u32 flags) {
    switch(node->kind) {
        case AST_DECL_PROCEDURE: {
            emit_typed_procedure_declaration(buffer, node);
            if(node->decl_proc.body) {
                emit_command(buffer, node->decl_proc.body);
            } else {
                catsprint(buffer, ";\n");
            }
        } break;
        case AST_DECL_VARIABLE: {
            emit_typed_declaration(buffer, node->decl_variable.type, node->decl_variable.name, 0);
            if(flags & EMIT_DECLARATION_DEFAULT_VALUE) {
                catsprint(buffer, " = ");
                emit_default_value_for_type(buffer, node->decl_variable.type);
            }
            catsprint(buffer, ";\n");
        } break;

        case AST_DECL_CONSTANT: // should not do anything, constant must be directly writted as an expression
        case AST_DECL_TYPEDEF:  // should not be handled here
        default: break;
    }
}

static void
emit_forward_type_decl(catstring* buffer, Light_Type** type_array) {
    for(int i = 0; i < array_length(type_array); ++i) {
        Light_Type* type = type_array[i];
        if(type->kind == TYPE_KIND_ALIAS) {
            Light_Type* root = type_alias_root(type);

            if(root->kind == TYPE_KIND_ENUM) {
                if(root->enumerator.type_hint) {
                    catsprint(buffer, "typedef ");
                    emit_typed_declaration(buffer, root->enumerator.type_hint, 0, 0);
                } else {
                    // TODO(psv): not always int
                    catsprint(buffer, "typedef int ");
                }
                catsprint_token(buffer, type->alias.name);
            } else {
                catsprint(buffer, "typedef ");
                emit_typed_declaration(buffer, root, type->alias.name, EMIT_FLAG_TREAT_AS_FUNCTION_TYPE);
            }

            catsprint(buffer, ";\n");
        }
    }
}

static void
emit_type_table(catstring* buffer, Light_Type** type_table) {
    catstring table = {0};
    catstring arrays_before = {0};
    catstring arrays_after = {0};
    catstring loader = {0};

    catsprint(&loader, "\nvoid __light_load_type_table() {\n");

    //catsprint(&arrays, "string __light_type_array_names[] = {\n");
    catsprint(&table, "User_Type_Info __light_type_table[%l] = {\n", array_length(type_table));

    for(int i = 0; i < array_length(type_table); ++i) {
        Light_Type* type = type_table[i];
        type_table[i]->type_table_index = i;

        if(i > 0) {
            catsprint(&table, ",\n");
        }

        // kind, flags, type_size_bytes
        catsprint(&table, "{ %d, %d, %d, {", type->kind, type->flags, type->size_bits / 8);

        switch(type->kind) {
            case TYPE_KIND_PRIMITIVE:
                catsprint(&table, " .primitive = %d", type->primitive);
                break;
            case TYPE_KIND_POINTER:
                catsprint(&table, " .pointer_to = &__light_type_table[%u]", type->pointer_to->type_table_index);
                break;
            case TYPE_KIND_ARRAY:
                catsprint(&table, " .array_desc = { &__light_type_table[%u], %u }", type->array_info.array_of->type_table_index, type->array_info.dimension);
                break;
            case TYPE_KIND_ALIAS:
                catsprint(&table, " .alias_desc = { {0, %d, \"%s+\"}, &__light_type_table[%d] }", type->alias.name->length, 
                    type->alias.name->length, type->alias.name->data, type->alias.alias_to->type_table_index);
                break;
            case TYPE_KIND_ENUM:
                // TODO(psv):
                catsprint(&table, " .primitive = 0");
                break;
            case TYPE_KIND_FUNCTION:
                // TODO(psv): arguments_types, arguments_names
                if(type->function.arguments_count > 0) {
                    // forward declarations
                    catsprint(&arrays_before, "User_Type_Info* __function_args_types_%x[%d] = {0};\n", type, type->function.arguments_count);

                    if(type->function.arguments_names) {
                        catsprint(&arrays_after, "__type_names[] = {");
                        for(int a = 0; a < type->function.arguments_count; ++a) {
                            if(a > 0) catsprint(&arrays_after, ", ");
                            catsprint(&arrays_after, "\"%s+\"", 
                                type->function.arguments_names_length[a], 
                                type->function.arguments_names[a]);
                        }
                        catsprint(&arrays_after, "};\n");
                    }

                    for(int f = 0; f < type->function.arguments_count; ++f) {
                        Light_Type* arg_type = type->function.arguments_type[f];
                        catsprint(&loader, "__function_args_types_%x[%d] = &__light_type_table[%u];\n", type, f, arg_type->type_table_index);
                    }
                    catsprint(&loader, "\n");
                    catsprint(&table, " .function_desc = { &__light_type_table[%u], (User_Type_Info**)&__function_args_types_%x, 0, %d }", 
                        type->function.return_type->type_table_index,
                        type,
                        type->function.arguments_count);
                } else {
                    catsprint(&table, " .function_desc = { &__light_type_table[%u], 0, 0, %d }", 
                        type->function.return_type->type_table_index,
                        type->function.arguments_count);
                }
                
                break;
            case TYPE_KIND_STRUCT:{
                if(type->struct_info.fields_count > 0) {
                    // forward declaration
                    catsprint(&arrays_before, "User_Type_Info* __struct_field_types_%x[%d] = {0};\n", type, type->struct_info.fields_count);

                    // Emit array with the names of all fields before, since we don't depend
                    // on any information other than the name string and length.
                    catsprint(&arrays_before, "string __struct_field_names_%x[%d] = {", type, type->struct_info.fields_count);
                    for(int f = 0; f < type->struct_info.fields_count; ++f) {
                        Light_Ast* field = type->struct_info.fields[f];
                        assert(field->kind == AST_DECL_VARIABLE);
                        if(f > 0) catsprint(&arrays_before, ", ");
                        catsprint(&arrays_before, "{ 0, %d, \"%s+\" }", 
                            field->decl_variable.name->length, field->decl_variable.name->length, field->decl_variable.name->data);
                    }
                    catsprint(&arrays_before, "};\n");
                    
                    // Fields offsets
                    catsprint(&arrays_before, "s64 __struct_field_offsets_%x[%d] = {", type, type->struct_info.fields_count);
                    for(int f = 0; f < type->struct_info.fields_count; ++f) {
                        if(f > 0) catsprint(&arrays_before, ", ");
                        catsprint(&arrays_before, "%l", type->struct_info.offset_bits[f]);
                    }
                    catsprint(&arrays_before, "};\n");
                    

                    for(int f = 0; f < type->struct_info.fields_count; ++f) {
                        Light_Ast* field = type->struct_info.fields[f];
                        assert(field->kind == AST_DECL_VARIABLE);
                        catsprint(&loader, "__struct_field_types_%x[%d] = &__light_type_table[%d];\n", type, f, type->struct_info.fields[f]->decl_variable.type->type_table_index);
                    }
                    catsprint(&loader, "\n");

                    catsprint(&table, " .struct_desc = { (User_Type_Info**)&__struct_field_types_%x, __struct_field_names_%x, (s64*)__struct_field_offsets_%x, %d, %d }", 
                        type, type, type,
                        type->struct_info.fields_count, 
                        type->struct_info.alignment_bytes);
                } else {
                    catsprint(&table, " .struct_desc = { 0, 0, 0, %d, %d }", 
                        type->struct_info.fields_count, type->struct_info.alignment_bytes);
                }
            } break;
            case TYPE_KIND_UNION: {
                if(type->union_info.fields_count > 0) {
                    // forward declaration
                    catsprint(&arrays_before, "User_Type_Info* __union_field_types_%x[%d] = {0};\n", type, type->union_info.fields_count);

                    // Emit array with the names of all fields before, since we don't depend
                    // on any information other than the name string and length.
                    catsprint(&arrays_before, "string __union_field_names_%x[%d] = {", type, type->union_info.fields_count);
                    for(int f = 0; f < type->union_info.fields_count; ++f) {
                        Light_Ast* field = type->union_info.fields[f];
                        assert(field->kind == AST_DECL_VARIABLE);
                        if(f > 0) catsprint(&arrays_before, ", ");
                        catsprint(&arrays_before, "{ 0, %d, \"%s+\" }", 
                            field->decl_variable.name->length, field->decl_variable.name->length, field->decl_variable.name->data);
                    }
                    catsprint(&arrays_before, "};\n");

                    for(int f = 0; f < type->union_info.fields_count; ++f) {
                        Light_Ast* field = type->union_info.fields[f];
                        assert(field->kind == AST_DECL_VARIABLE);
                        catsprint(&loader, "__union_field_types_%x[%d] = &__light_type_table[%d];\n", type, f, type->union_info.fields[f]->decl_variable.type->type_table_index);
                    }
                    catsprint(&loader, "\n");

                    catsprint(&table, " .union_desc = { (User_Type_Info**)&__union_field_types_%x, __union_field_names_%x, %d, %d }", 
                        type, type,
                        type->union_info.fields_count, 
                        type->union_info.alignment_bytes);
                } else {
                    catsprint(&table, " .union_desc = { 0, 0, %d, %d }", 
                        type->union_info.fields_count, type->union_info.alignment_bytes);
                }
            } break;
            default: assert(0); break;
        }

        catsprint(&table, "}}");
    }

    catsprint(&table, "};\n");

    catsprint(&loader, "}\n");

    catstring_append(buffer, &arrays_before);
    catstring_append(buffer, &table);
    catstring_append(buffer, &arrays_after);
    catstring_append(buffer, &loader);
}

void 
backend_c_generate_top_level(Light_Ast** ast, Type_Table type_table, Light_Scope* global_scope,
    const char* path, const char* filename, const char* compiler_path) 
{
    Light_Token* user_type_info_token = token_new_identifier_from_string("User_Type_Info", sizeof("User_Type_Info") - 1);
    Light_Ast* user_type_info_decl = decl_from_name(global_scope, user_type_info_token);
    Light_Type* user_type_info_type = user_type_info_decl->decl_typedef.type_referenced;
    reflect_types[REFLECT_TYPE_USER_TYPE_INFO_POINTER] = type_new_pointer(user_type_info_type);

    catstring code = {0};

    catsprint(&code, "typedef char s8;\n");
    catsprint(&code, "typedef short s16;\n");
    catsprint(&code, "typedef int s32;\n");
    catsprint(&code, "typedef long long s64;\n");
    catsprint(&code, "typedef unsigned char u8;\n");
    catsprint(&code, "typedef unsigned short u16;\n");
    catsprint(&code, "typedef unsigned int u32;\n");
    catsprint(&code, "typedef unsigned long long u64;\n");
    catsprint(&code, "typedef u32 bool;\n");
    catsprint(&code, "typedef float r32;\n");
    catsprint(&code, "typedef double r64;\n");
	catsprint(&code, "#define true 1\n");
	catsprint(&code, "#define false 0\n");
    catsprint(&code, "\n\n");

    catsprint(&code, "void __memory_copy(void* dest, void* src, u64 size) {\n");
	catsprint(&code, "\tfor(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];\n");
	catsprint(&code, "}\n");

    // Emit, in order, all type aliases
    emit_forward_type_decl(&code, global_type_array);

    catstring decls = {0};
    catsprint(&decls, "\n// Forward Declarations\n\n");

    for(int i = 0; i < array_length(ast); ++i) {
        if(ast[i]->kind == AST_DECL_PROCEDURE) {
            emit_proc_forward_decl(&decls, ast[i]);
        } else if(ast[i]->kind == AST_DECL_VARIABLE) {
            emit_declaration(&decls, ast[i], 0);
        }
    }

    // Emit type table
    catsprint(&decls, "\n// Type table\n\n");
    emit_type_table(&decls, global_type_array);

    // Emit top level initialization
    catstring init_function = {0};
    catstring init_function_before = {0};
    catsprint(&init_function, "void __light_initialize_top_level() {\n");

    catsprint(&decls, "\n// Declarations\n\n");
    for(int i = 0; i < array_length(ast); ++i) {
        Light_Ast* node = ast[i];
        emit_declaration(&decls, node, 0);
        if(node->kind == AST_DECL_VARIABLE && node->decl_variable.assignment) {            
            //emit_variable_assignment(&init_function, node->decl_variable.name, node->decl_variable.assignment);
            emit_variable_assignment_top_level(&init_function, &init_function_before, node->decl_variable.name, node->decl_variable.assignment);
            catsprint(&init_function, ";\n");
        }
    }
    catsprint(&init_function, "}\n");

    catstring_append(&code, &decls);
    catstring_append(&code, &init_function_before);
    catstring_append(&code, &init_function);

    catsprint(&code, "int main() { __light_load_type_table(); __light_initialize_top_level(); return __light_main(); }\n");

    catstring outfile = {0};
    catsprint(&outfile, "%s%s.c\0", path, filename);
    catstring_to_file(outfile.data, code);
}

void 
backend_c_compile_with_gcc(Light_Ast** ast, const char* filename, const char* working_directory) {
    char command_buffer[2048] = {0};
    #if defined(__linux__)
    sprintf(command_buffer, "gcc -g %s%s.c -o %s%s -lX11 -lGL -lm", 
        working_directory, filename, working_directory, filename);
    #elif defined(_WIN32) || defined(_WIN64)
    sprintf(command_buffer, "gcc -g %s%s.c -o %s%s.exe", 
        working_directory, filename, working_directory, filename);
    #endif
    system(command_buffer);
}

void 
backend_c_compile_with_cl(Light_Ast** ast, const char* filename, const char* working_directory) {
    char command_buffer[2048] = {0};
    #if defined(_WIN32) || defined(_WIN64)
    sprintf(command_buffer, "cl /nologo /Zi %s%s.c /Fe:%s%s.exe /link user32.lib kernel32.lib gdi32.lib opengl32.lib", 
        working_directory, filename, working_directory, filename);
    #endif
    system(command_buffer);
}