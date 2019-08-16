#include "backend_c.h"
#include "../include/light_array.h"
#include "type_table.h"
#include <assert.h>
#include <stdio.h>

// --------------------------------
// ---------- Catstring -----------
// --------------------------------

#define MAX(A, B) ((A > B) ? (A) : (B))

typedef struct {
    int   length;
    int   capacity;
    char* data;
} catstring;

static void
buffer_grow(catstring* s) {
    if(s->capacity < 32) {
        s->data = realloc(s->data, s->capacity + 32);
        s->capacity = s->capacity + 32;
    } else {
        s->data = realloc(s->data, s->capacity * 2);
        s->capacity = s->capacity * 2;
    }
}

static void
buffer_grow_by(catstring* s, int amount) {
    int a = (s->capacity + amount) * 2;
    s->data = realloc(s->data, a);
    s->capacity = a;
}

//static int 
//catsprint_u64(catstring* buffer, u64 num) {
//    if((buffer->length + 32) >= buffer->capacity) {
//        buffer_grow_by(buffer, 32);
//    }
//    int c = sprintf(buffer->data, "%lx", num);
//    buffer->length += c;
//
//    return c;
//}

int
catsprint_token(catstring* buffer, Light_Token* t) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, MAX(32, t->length));
        buffer->capacity = MAX(32, t->length);
    }

    int n = 0;

    if((buffer->length + t->length) >= buffer->capacity) {
        buffer_grow_by(buffer, t->length);
    }
    memcpy(buffer->data + buffer->length, t->data, t->length);
    buffer->length += t->length;

    return t->length;
}

int 
catsprint(catstring* buffer, char* str) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    int n = 0;
    for(char* at = str;; ++at, ++n) {
        // push to stream
        if(buffer->length >= buffer->capacity) {
            buffer_grow(buffer);
        }

        buffer->data[buffer->length] = *at;
        if(!(*at)) break;
        buffer->length++;
    }

    return n;
}

catstring 
catstring_copy(catstring* s) {
    catstring result = {0};

    result.data = calloc(1, s->capacity);
    result.capacity = s->capacity;
    result.length = s->length;
    memcpy(result.data, s->data, s->length);

    return result;
}

int
catstring_to_file(const char* filename, catstring s) {
    FILE* out = fopen(filename, "wb");
    if(!out) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        return -1;
    }

    if(fwrite(s.data, s.length, 1, out) != 1) {
        fclose(out);
        fprintf(stderr, "Could not write to file %s\n", filename);
        return -1;
    }

    fclose(out);
    return 0;
}

// Generate

void emit_declaration(catstring* s, Light_Ast* node);
bool emit_type_declaration(catstring* buffer, Light_Type* type, Light_Token* name);

static void 
emit_procedure_part1(catstring* buffer, Light_Type* type) {
    if(type->kind == TYPE_KIND_FUNCTION) {
        emit_procedure_part1(buffer, type->function.return_type);
    } else {
        emit_type_declaration(buffer, type, 0);
    }
    catsprint(buffer, "(*");
}

static void
emit_procedure_part1_non_ptr(catstring* buffer, Light_Type* type) {
    if(type->kind == TYPE_KIND_FUNCTION) {
        emit_procedure_part1(buffer, type->function.return_type);
    } else {
        emit_type_declaration(buffer, type, 0);
    }
    catsprint(buffer, " ");
}

static bool 
emit_procedure_part2(catstring* buffer, Light_Type* type, Light_Token* name) {
    bool emitted_name = false;
    if(type->kind == TYPE_KIND_FUNCTION) {
        emitted_name = emit_procedure_part2(buffer, type->function.return_type, name);
        catsprint(buffer, ")(");
        for(int i = 0; i < type->function.arguments_count; ++i) {
            if (i > 0) catsprint(buffer, ", ");
            emit_type_declaration(buffer, type->function.arguments_type[i], name);
        }
        catsprint(buffer, ")");
    } else {
        if (name) {
            catsprint_token(buffer, name);
            emitted_name = true;
        }
    }
    return emitted_name;
}

bool
emit_type_declaration(catstring* buffer, Light_Type* type, Light_Token* name) {
    bool emitted_name = false;
    switch(type->kind)
    {
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
        }break;
        case TYPE_KIND_POINTER: {
            switch(type->pointer_to->kind) {
                case TYPE_KIND_ALIAS: {
                    Light_Type* ptrto_root = type_alias_root(type->pointer_to);
                    switch(ptrto_root->kind) {
                        case TYPE_KIND_ENUM: catsprint(buffer, "enum "); break;
                        case TYPE_KIND_STRUCT: catsprint(buffer, "struct "); break;
                        case TYPE_KIND_UNION: catsprint(buffer, "union "); break;
                        default: break;
                    }
                    emitted_name |= emit_type_declaration(buffer, type->pointer_to, name);
                    catsprint(buffer, "*");
                }break;
                case TYPE_KIND_FUNCTION: {
                    emit_procedure_part1(buffer, type->pointer_to->function.return_type);
                    catsprint(buffer, "*");
                    emitted_name |=emit_procedure_part2(buffer, type->pointer_to, name);
                } break;
                case TYPE_KIND_ARRAY: {

                } break;
                default: {
                    emit_type_declaration(buffer, type->pointer_to, 0);
                    catsprint(buffer, "*");
                } break;
            }

        }break;
        case TYPE_KIND_ARRAY:{
        }break;
        case TYPE_KIND_ENUM: {
        } break;
        case TYPE_KIND_FUNCTION:{
            emit_procedure_part1(buffer, type->function.return_type);
            emitted_name |=emit_procedure_part2(buffer, type, name);
        }break;
		case TYPE_KIND_UNION: {
            catsprint(buffer, "union ");
            catsprint_token(buffer, name);
            catsprint(buffer, " {");
            for(int i = 0; i < type->union_info.fields_count; ++i) {
                emit_declaration(buffer, type->union_info.fields[i]);
                catsprint(buffer, ";");
            }
            catsprint(buffer, "}");
		}break;
        case TYPE_KIND_STRUCT: {
			catsprint(buffer, "struct ");
            catsprint_token(buffer, name);
            catsprint(buffer, " {");
            for(int i = 0; i < type->struct_info.fields_count; ++i) {
                emit_declaration(buffer, type->struct_info.fields[i]);
                catsprint(buffer, "; ");
            }
            catsprint(buffer, "}");
        }break;
        case TYPE_KIND_ALIAS: {
            catsprint_token(buffer, type->alias.name);
        } break;
        default: break;
    }
    return emitted_name;
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
emit_expression(catstring* buffer, Light_Ast* node) {
    assert(node->flags & AST_FLAG_EXPRESSION);
    
    switch(node->kind) {
        case AST_EXPRESSION_BINARY:{
            catsprint(buffer, "(");
            emit_expression(buffer, node->expr_binary.left);
            emit_binop(buffer, node->expr_binary.op);
            emit_expression(buffer, node->expr_binary.right);
            catsprint(buffer, ")");
        } break;
        case AST_EXPRESSION_PROCEDURE_CALL: {
            catsprint(buffer, "(");
            emit_expression(buffer, node->expr_proc_call.caller_expr);
            catsprint(buffer, ")(");
            for(int i = 0; i < node->expr_proc_call.arg_count; ++i) {
                if(i > 0) catsprint(buffer, ",");
                emit_expression(buffer, node->expr_proc_call.args[i]);
            }
            catsprint(buffer, ")");
        } break;
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            catsprint(buffer, "3");
        } break;
        case AST_EXPRESSION_VARIABLE: {
            catsprint_token(buffer, node->expr_variable.name);
        } break;
        case AST_EXPRESSION_DOT:{

        } break;
        case AST_EXPRESSION_LITERAL_ARRAY:
        case AST_EXPRESSION_LITERAL_STRUCT:
        case AST_EXPRESSION_UNARY:
        default: break;
    }
}

static void 
emit_command(catstring* buffer, Light_Ast* node) {
    if(!(node->flags & AST_FLAG_COMMAND)) {
        return;
    }

    switch(node->kind) {
        case AST_COMMAND_BLOCK: {
            catsprint(buffer, "{\n");
            for(int i = 0; i < node->comm_block.command_count; ++i) {
                Light_Ast* n = node->comm_block.commands[i];
                if(n->flags & AST_FLAG_COMMAND) {
                    emit_command(buffer, node->comm_block.commands[i]);
                } else if(n->kind == AST_DECL_VARIABLE) {
                    emit_declaration(buffer, node->comm_block.commands[i]);
                    catsprint(buffer, ";\n");
                }
            }
            catsprint(buffer, "}\n");
        } break;
        case AST_COMMAND_ASSIGNMENT:{
            if(node->comm_assignment.lvalue) {
                emit_expression(buffer, node->comm_assignment.lvalue);
                catsprint(buffer, " = ");
            }
            emit_expression(buffer, node->comm_assignment.rvalue);
            catsprint(buffer, ";\n");
        } break;
        case AST_COMMAND_IF:{
            catsprint(buffer, "if (");
            emit_expression(buffer, node->comm_if.condition);
            catsprint(buffer, ") {\n");
            emit_command(buffer, node->comm_if.body_true);
            catsprint(buffer, "}");
            if (node->comm_if.body_false) {
                catsprint(buffer, " else {\n");
                emit_command(buffer, node->comm_if.body_false);
                catsprint(buffer, "}");
            }
        } break;
        case AST_COMMAND_FOR:{
        } break;
        case AST_COMMAND_WHILE:{
            
            catsprint(buffer, "while (");
            emit_expression(buffer, node->comm_while.condition);
            catsprint(buffer, ") {\n");
            emit_command(buffer, node->comm_while.body);
            catsprint(buffer, "}\n");
        } break;
        case AST_COMMAND_RETURN: {
            catsprint(buffer, "return ");
            if(node->comm_return.expression) {
                emit_expression(buffer, node->comm_return.expression);
            }
            catsprint(buffer, ";\n");
        } break;
        case AST_COMMAND_BREAK:
        case AST_COMMAND_CONTINUE:
            // TODO(psv):
        default: break;
    }
}

void
emit_declaration(catstring* buffer, Light_Ast* node) {
    switch(node->kind) {
        case AST_DECL_PROCEDURE: {
            emit_procedure_part1_non_ptr(buffer, node->decl_proc.return_type);

            catsprint_token(buffer, node->decl_proc.name);
            catsprint(buffer, "(");

            for(int i = 0; i < node->decl_proc.argument_count; ++i) {
                if(i > 0) catsprint(buffer, ", ");
                emit_declaration(buffer, node->decl_proc.arguments[i]);
            }

            catsprint(buffer, ")");

            emit_procedure_part2(buffer, node->decl_proc.return_type, 0);

            if(node->decl_proc.body) {
                emit_command(buffer, node->decl_proc.body);
            } else {
                catsprint(buffer, ";\n");
            }
        } break;
        case AST_DECL_VARIABLE: {
            if (!emit_type_declaration(buffer, node->decl_variable.type, node->decl_variable.name)) {
                catsprint(buffer, " ");
                catsprint_token(buffer, node->decl_variable.name);
            }
        } break;

        case AST_DECL_CONSTANT: // should not do anything, constant must be directly writted as an expression
        case AST_DECL_TYPEDEF:  // should not be handled here
        default: break;
    }
}

void
emit_forward_type_decl(catstring* buffer, Light_Type** types) {
    for(int i = 0; i < array_length(types); ++i){
        Light_Type* type = types[i];
        Light_Type* type_root = type_alias_root(type);

        if(type->kind == TYPE_KIND_ALIAS) {
            catsprint(buffer, "typedef ");
            emit_type_declaration(buffer, type_root, type->alias.name);
            catsprint(buffer, " ");
            catsprint_token(buffer, type->alias.name);
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

    for(int i = 0; i < array_length(ast); ++i) {
        emit_declaration(&code, ast[i]);
        catsprint(&code, "\n");
    }

    catstring_to_file("test/generated_out.c", code);
}