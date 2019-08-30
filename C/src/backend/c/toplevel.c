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
            catsprint(buffer, "[%u]", type->array_info.dimension);
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
};

static void
emit_type_start(catstring* buffer, Light_Type* type, u32 flags) {
    switch(type->kind) {
        case TYPE_KIND_ALIAS:{
            catsprint_token(buffer, type->alias.name);
        } break;
        case TYPE_KIND_ARRAY:{
            emit_type_start(buffer, type->array_info.array_of, flags);
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
    if(decl->decl_proc.name)
        catsprint_token(buffer, decl->decl_proc.name);

    // end (arguments)
    catsprint(buffer, ")(");
    for(int i = 0; i < decl->decl_proc.argument_count; ++i) {
        if(i > 0) catsprint(buffer, ", ");

        emit_typed_declaration(buffer, decl->decl_proc.arguments[i]->decl_variable.type, 
            decl->decl_proc.arguments[i]->decl_variable.name, 0);
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

        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_ARRAY:{
            emit_type_start(buffer, type, flags);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
            emit_type_end(buffer, type);
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
                    catsprint(buffer, "%u", (u64)lit.value_u8); break;
                case TYPE_PRIMITIVE_U16:
                    catsprint(buffer, "%u", (u64)lit.value_u16); break;
                case TYPE_PRIMITIVE_U32:
                    catsprint(buffer, "%u", (u64)lit.value_u32); break;
                case TYPE_PRIMITIVE_U64:
                    catsprint(buffer, "%u", lit.value_u64); break;
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
            emit_typed_declaration(buffer, node->expr_unary.type_to_cast, 0, 0);
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
            catsprint_token(buffer, node->expr_variable.name);
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

            char b[256] = {0};
            Light_Token name = token_from_name(b, "_lit_array", node->id);
            emit_typed_declaration(&after, node->type, &name, 0);

            if(node->expr_literal_array.raw_data) {
                //node->expr_literal_array.data
                catsprint(&after, " = %s+;\n", node->expr_literal_array.data_length_bytes, node->expr_literal_array.data);
            } else {
                catsprint(&after, " = {");
                for(u64 i = 0; i < array_length(node->expr_literal_array.array_exprs); ++i) {
                    if(i > 0) catsprint(&after, ", ");
                    Light_Ast* expr = node->expr_literal_array.array_exprs[i];
                    emit_expression(literal_decls, &after, expr);
                }
                catsprint(&after, "};\n");
            }

            catstring_append(literal_decls, &after);

            catsprint(buffer, "_lit_array_%d", node->id);
        } break;
        case AST_EXPRESSION_LITERAL_STRUCT:{
            catstring after = {0};

            char b[256] = {0};
            Light_Token name = token_from_name(b, "_lit_struct", node->id);
            emit_typed_declaration(&after, node->type, &name, 0);
            catsprint(&after, " = {");

            for(u64 i = 0; i < array_length(node->expr_literal_struct.struct_exprs); ++i) {
                if(i > 0) catsprint(&after, ", ");
                Light_Ast* expr = node->expr_literal_struct.struct_exprs[i];
                emit_expression(literal_decls, &after, expr);
            }

            catstring_append(literal_decls, &after);

            catsprint(literal_decls, "};\n");

            catsprint(buffer, "_lit_struct_%d", node->id);
        } break;
        case AST_EXPRESSION_UNARY:{
            emit_expression_unary(literal_decls, buffer, node);
        } break;
        default: break;
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
emit_command(catstring* buffer, Light_Ast* node) {
    switch(node->kind){
        case AST_DECL_VARIABLE:
            emit_declaration(buffer, node, EMIT_DECLARATION_DEFAULT_VALUE);
            break;
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
                }
            }
            
            catsprint(buffer, "}\n");
        } break;
        
        case AST_COMMAND_ASSIGNMENT:{
            catstring c = {0};
            if(node->comm_assignment.lvalue) {
                emit_expression(buffer, &c, node->comm_assignment.lvalue);
                catsprint(&c, " = ");
            }
            emit_expression(buffer, &c, node->comm_assignment.rvalue);

            catstring_append(buffer, &c);
            catsprint(buffer, ";\n");
        } break;
        
        case AST_COMMAND_IF:{
            bool body_true_is_block = node->comm_if.body_true && node->comm_if.body_true->kind == AST_COMMAND_BLOCK;
            bool body_false_is_block = node->comm_if.body_false && node->comm_if.body_false->kind == AST_COMMAND_BLOCK;

            // Conditional expression
            catsprint(buffer, "if (");
            // TODO(psv): literal declarations
            emit_expression(0, buffer, node->comm_if.condition);
            catsprint(buffer, ")");

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

            catsprint(buffer, "while (");
            // TODO(psv): literal declarations
            emit_expression(0, buffer, node->comm_while.condition);
            catsprint(buffer, ") {\n");
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
            catsprint(buffer, "label_continue_%d:\n", node->comm_for.for_scope->id);
            catsprint(buffer, "{\n");

            // first declarations (prologue)
            for(u64 i = 0; i < array_length(node->comm_for.prologue); ++i) {
                emit_command(buffer, node->comm_for.prologue[i]);
            }

            catsprint(buffer, "while(");
            // TODO(psv): literal_declarations
            emit_expression(0, buffer, node->comm_for.condition);
            catsprint(buffer, ")");

            catsprint(buffer, "{");
            emit_command(buffer, node->comm_for.body);
            
            // epilogue
            for(u64 i = 0; i < array_length(node->comm_for.epilogue); ++i) {
                emit_command(buffer, node->comm_for.epilogue[i]);
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

void 
backend_c_generate_top_level(Light_Ast** ast, Type_Table type_table) {
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

    // Emit type table
    //emit_type_table(&code, global_type_array);

    catstring decls = {0};
    catsprint(&decls, "\n// Forward Declarations\n\n");

    for(int i = 0; i < array_length(ast); ++i) {
        if(ast[i]->kind == AST_DECL_PROCEDURE) {
            emit_proc_forward_decl(&decls, ast[i]);
        }
    }

    catsprint(&decls, "\n// Declarations\n\n");

    for(int i = 0; i < array_length(ast); ++i) {
        emit_declaration(&decls, ast[i], 0);
        catsprint(&decls, "\n");
    }

    catstring_append(&code, &decls);

    catstring_to_file("test/generated_out.c", code);
}

void 
backend_c_compile_with_gcc(Light_Ast** ast, const char* filename) {
    char command_buffer[2048] = {0};
    sprintf(command_buffer, "gcc -g test/generated_out.c -o %.*s", (int)strlen(filename) - 3, filename);
    system(command_buffer);
}