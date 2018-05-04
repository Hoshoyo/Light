#include "c_backend.h"
#include "decl_check.h"
#include <stdarg.h>

static void report_fatal_error(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	//assert(0);
	exit(-1);
}
//#define DEBUG 1
int C_Code_Generator::sprint(char* msg, ...) {
	va_list args;
	va_start(args, msg);
#if DEBUG
	int num_written = vfprintf(stdout, msg, args);
	fflush(stdout);
#else
	int num_written = vsprintf(buffer + ptr, msg, args);
	ptr += num_written;
#endif
	va_end(args);
	return num_written;
}

bool type_regsize(Type_Instance* type) {
	return (type->kind == KIND_POINTER || type->kind == KIND_PRIMITIVE);
}

s64 C_Code_Generator::alloc_loop_id() {
	loop_id++;
	return loop_id - 1;
}

void C_Code_Generator::emit_typedef(Type_Instance* type, Token* name, char* prefix) {
    static int id = 0;

    switch(type->kind){
        case KIND_POINTER:
        case KIND_PRIMITIVE:{
            sprint("typedef ");
            emit_type(type);
            if(prefix) sprint(" %s", prefix);
            if(name) sprint(" %.*s", TOKEN_STR(name));
            sprint(";\n");
        } break;

        case KIND_FUNCTION:{
            if(type->function_desc.return_type->kind == KIND_FUNCTION){
                char p[64] = {0};
                int rettype_id = id;
                sprintf(p, "__typet_%d", prefix, id++);
                emit_typedef(type->function_desc.return_type, 0, p);
                sprint("typedef __typet_%d ", rettype_id);
            } else {
                sprint("typedef ");
                emit_type(type->function_desc.return_type);
            }

            sprint(" ");
            assert(prefix || name);
            if(prefix) {
                sprint("%s", prefix);
            }
            if(name){
                sprint("%.*s", TOKEN_STR(name));
            }
            sprint("(");
            for(s32 i = 0; i < type->function_desc.num_arguments; ++i) {
                if(i != 0) sprint(", ");
                emit_type(type->function_desc.arguments_type[i]);
            }
            sprint(");\n");

        }break;
        case KIND_ARRAY:
        case KIND_STRUCT:
            break;
    }
}

void C_Code_Generator::emit_type(Type_Instance* type, Token* name){
    //assert_msg(type->flags & TYPE_FLAG_INTERNALIZED, "tried to emit a type that is not internalized");
    switch(type->kind){
        case KIND_PRIMITIVE:{
            switch(type->primitive){
                case TYPE_PRIMITIVE_BOOL:
                    sprint("bool"); break;
                case TYPE_PRIMITIVE_R32:
                    sprint("r32"); break;
                case TYPE_PRIMITIVE_R64:
                    sprint("r64"); break;
                case TYPE_PRIMITIVE_S8:
                    sprint("s8"); break;
                case TYPE_PRIMITIVE_S16:
                    sprint("s16"); break;
                case TYPE_PRIMITIVE_S32:
                    sprint("s32"); break;
                case TYPE_PRIMITIVE_S64:
                    sprint("s64"); break;
                case TYPE_PRIMITIVE_U8:
                    sprint("u8"); break;
                case TYPE_PRIMITIVE_U16:
                    sprint("u16"); break;
                case TYPE_PRIMITIVE_U32:
                    sprint("u32"); break;
                case TYPE_PRIMITIVE_U64:
                    sprint("u64"); break;
                case TYPE_PRIMITIVE_VOID:
                    sprint("void"); break;
            }
        }break;
        case KIND_POINTER: {
            emit_type(type->pointer_to);
            sprint("*");
        }break;
        case KIND_ARRAY:{
            if(name){
                Type_Instance* auxtype = type;
                sprint("char %.*s[%lld]", TOKEN_STR(name), type->type_size_bits /8);
            } else {
                emit_type(type->array_desc.array_of);
                sprint("*");
            }
        }break;
        case KIND_FUNCTION:{
            emit_type(type->function_desc.return_type);
            if(name){
                sprint("(*%.*s)(", TOKEN_STR(name));
            } else {
                sprint("(*)(");
            }
            for(s32 i = 0; i < type->function_desc.num_arguments; ++i) {
                if(i != 0) sprint(", ");
                emit_type(type->function_desc.arguments_type[i]);
            }
            sprint(")");
        }break;
        case KIND_STRUCT: {
            // @HACK
            // @HACK
            // @HACK
            // patch internalized type in type checking or something
            if(!(type->flags & TYPE_FLAG_INTERNALIZED)) {
                sprint("struct ");
            }
            if(name){
                sprint("%.*s", TOKEN_STR(name));
            } else {
                sprint("%.*s", TOKEN_STR(type->struct_desc.name));
            }
        }break;
    }
}

void C_Code_Generator::emit_default_value(Type_Instance* type) {
	switch (type->kind) {
		case KIND_PRIMITIVE:{
			switch (type->primitive) {
				case TYPE_PRIMITIVE_BOOL:
					sprint("false");
					break;
				case TYPE_PRIMITIVE_R32:
					sprint("0.0f");
					break;
				case TYPE_PRIMITIVE_R64:
					sprint("0.0");
					break;
				case TYPE_PRIMITIVE_S8:
				case TYPE_PRIMITIVE_S16:
				case TYPE_PRIMITIVE_S32:
				case TYPE_PRIMITIVE_S64:
				case TYPE_PRIMITIVE_U8:
				case TYPE_PRIMITIVE_U16:
				case TYPE_PRIMITIVE_U32:
				case TYPE_PRIMITIVE_U64:
					sprint("0");
					break;
				case TYPE_PRIMITIVE_VOID:
					assert(0);
					break;
			}
		}break;
		case KIND_POINTER:
		case KIND_FUNCTION:
			sprint("0");
			break;
		case KIND_STRUCT:
		case KIND_ARRAY:
			sprint("{0}");
			break;
	}
}

void C_Code_Generator::emit_decl(Ast* decl, bool forward) {
    assert(decl->flags & AST_FLAG_IS_DECLARATION);
    switch(decl->node_type) {
        case AST_DECL_PROCEDURE: {
            if(forward && decl->decl_procedure.type_return->kind == KIND_FUNCTION){
                emit_typedef(decl->decl_procedure.type_return, decl->decl_procedure.name, "__ret_");
            } 
            if(decl->decl_procedure.type_return->kind == KIND_FUNCTION){
                sprint("__ret_%.*s*", TOKEN_STR(decl->decl_procedure.name));
            } else {
                emit_type(decl->decl_procedure.type_return);
            }

			if (decl->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) {
				sprint(" %.*s(", TOKEN_STR(decl->decl_procedure.name));
			} else {
				if (decl->decl_procedure.flags & DECL_PROC_FLAG_MAIN) {
					sprint(" __%.*s(", TOKEN_STR(decl->decl_procedure.name));
				} else {
					sprint(" %.*s(", TOKEN_STR(decl->decl_procedure.name));
				}
			}
            for(s32 i = 0; i < decl->decl_procedure.arguments_count; ++i){
                if(i != 0) sprint(", ");

                Ast* arg = decl->decl_procedure.arguments[i];
                assert(arg->node_type == AST_DECL_VARIABLE);
                emit_decl(arg);
            }
            sprint(")");
        }break;
        case AST_DECL_VARIABLE:{
            if(decl->decl_variable.variable_type->kind == KIND_FUNCTION){
                emit_type(decl->decl_variable.variable_type, decl->decl_variable.name);
            } else {
                if(decl->decl_variable.variable_type->kind == KIND_ARRAY){
                    emit_type(decl->decl_variable.variable_type, decl->decl_variable.name);
                } else {
                    emit_type(decl->decl_variable.variable_type);
                    sprint(" %.*s", TOKEN_STR(decl->decl_variable.name));
                }
            }
        }break;
        case AST_DECL_CONSTANT:{
            emit_type(decl->decl_constant.type_info);
            sprint(" %.*s", TOKEN_STR(decl->decl_constant.name));
        }break;
        case AST_DECL_STRUCT:{
            sprint("typedef struct %.*s{", TOKEN_STR(decl->decl_struct.name));
            size_t nfields = decl->decl_struct.fields_count;
            for(size_t i = 0; i < nfields; ++i){
                emit_decl(decl->decl_struct.fields[i]);
				sprint(";");
            }
            sprint("} %.*s", TOKEN_STR(decl->decl_struct.name));
        }break;
        case AST_DECL_UNION: {
            assert_msg(0, "union C codegen not yet implemented");
        }break;
        case AST_DECL_ENUM:{
            assert_msg(0, "enum C codegen not yet implemented");
        }break;
    }
}

void C_Code_Generator::emit_command(Ast* comm) {
	switch (comm->node_type) {
		case AST_COMMAND_BLOCK:{
			sprint("{\n");
			for (s32 i = 0; i < comm->comm_block.command_count; ++i) {
                Ast* cm = comm->comm_block.commands[i];
                
                if(cm->flags & AST_FLAG_IS_DECLARATION){
                    if(cm->node_type == AST_DECL_VARIABLE){
                        emit_decl(cm);
                        sprint(" = ");
                        if(cm->decl_variable.assignment){
                            emit_expression(cm->decl_variable.assignment);
						} else {
							emit_default_value(cm->decl_variable.variable_type);
						}
						sprint(";");
					} else if (cm->node_type == AST_DECL_CONSTANT) {
						sprint("const ");
						emit_decl(cm);
						sprint(" = ");
						emit_expression(cm->decl_constant.value);
						sprint(";");
					} else {
                        assert_msg(0, "scoped declaration of procedure, enum and union not yet implemented");
                    }
                } else {
				    emit_command(cm);
                }
				sprint("\n");
			}
			sprint("}");
		}break;
		case AST_COMMAND_BREAK: {
			//sprint("break;");
			s64 level = comm->comm_break.level->expr_literal.value_u64;
			sprint("goto loop_%lld;", loop_id - level);
		}break;
		case AST_COMMAND_CONTINUE: {
            sprint("continue;");
		}break;
		case AST_COMMAND_FOR:{
			s64 id = alloc_loop_id();
			comm->comm_for.id = id;
            sprint("while(");
            emit_expression(comm->comm_for.condition);
            sprint(")");
            emit_command(comm->comm_for.body);
			sprint("\nloop_%lld:;\n", id);
        }break;
		case AST_COMMAND_IF:{
            sprint("if(");
            emit_expression(comm->comm_if.condition);
            sprint(")");
            emit_command(comm->comm_if.body_true);
            if(comm->comm_if.body_false){
                sprint(" else ");
                emit_command(comm->comm_if.body_false);
            }
        }break;
		case AST_COMMAND_RETURN:{
            sprint("return ");
            if(comm->comm_return.expression){
                emit_expression(comm->comm_return.expression);
            }
            sprint(";");
        }break;
		case AST_COMMAND_VARIABLE_ASSIGNMENT:{
			Ast* rval = comm->comm_var_assign.rvalue;
            
			// register size assignment
			if (type_regsize(rval->type_return) || rval->type_return->kind == KIND_STRUCT || rval->type_return->kind == KIND_FUNCTION) {
				if (comm->comm_var_assign.lvalue) {
					emit_expression(comm->comm_var_assign.lvalue);
					sprint(" = ");
				}
				emit_expression(comm->comm_var_assign.rvalue);
				sprint(";");
			}
			// bigger than regsize
			else {
				assert_msg(0, "assignment of type not recognized");
			}
        } break;
	}
}


void C_Code_Generator::emit_expression_binary(Ast* expr){
    switch(expr->expr_binary.op){
        case OP_BINARY_PLUS:
        case OP_BINARY_MINUS:
        case OP_BINARY_AND:
        case OP_BINARY_OR:
        case OP_BINARY_SHL:
        case OP_BINARY_SHR:
        case OP_BINARY_XOR:
        case OP_BINARY_DIV:
        case OP_BINARY_MOD:
        case OP_BINARY_MULT:
        case OP_BINARY_EQUAL:
        case OP_BINARY_GE:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_LT:
        case OP_BINARY_LOGIC_AND:
        case OP_BINARY_LOGIC_OR:
        case OP_BINARY_NOT_EQUAL:
        case OP_BINARY_DOT:{
            sprint("(");
            emit_expression(expr->expr_binary.left);
            sprint(binop_op_to_string(expr->expr_binary.op));
            emit_expression(expr->expr_binary.right);
            sprint(")");
        }break;
        case OP_BINARY_VECTOR_ACCESS:{
            Type_Instance* indexed_type = expr->expr_binary.left->type_return;
			if (indexed_type->kind == KIND_ARRAY) {
				if(indexed_type->array_desc.array_of->kind != KIND_ARRAY)
					sprint("*");
			}
			if (indexed_type->kind == KIND_POINTER) {
				if (indexed_type->pointer_to->kind != KIND_POINTER) {
					sprint("*");
				}
			}

            sprint("(");
            emit_type(indexed_type);
            sprint(")");

            sprint("((");
            assert(indexed_type->kind == KIND_POINTER || indexed_type->kind == KIND_ARRAY);
            sprint("(char*)");
            emit_expression(expr->expr_binary.left);
            sprint(")");
            if(indexed_type->kind == KIND_POINTER){
                sprint("+ %lld * (", indexed_type->pointer_to->type_size_bits / 8);
            } else {
                sprint("+ %lld * (", indexed_type->array_desc.array_of->type_size_bits / 8);
            }
            emit_expression(expr->expr_binary.right);
            sprint("))");
        }break;
    }
}

void C_Code_Generator::emit_expression(Ast* expr){
    assert(expr->flags & AST_FLAG_IS_EXPRESSION);

    switch(expr->node_type){
        case AST_EXPRESSION_BINARY:{
            emit_expression_binary(expr);
        }break;
        case AST_EXPRESSION_LITERAL:{
            switch(expr->expr_literal.type){
                case LITERAL_BIN_INT:
                case LITERAL_HEX_INT:
                case LITERAL_UINT:{
                    sprint("0x%llx", expr->expr_literal.value_u64);
                }break;
                case LITERAL_SINT: {
                    sprint("%lld", expr->expr_literal.value_u64);
                }break;
                case LITERAL_FLOAT:{
                    sprint("%f", expr->expr_literal.value_r64);
                }break;
                case LITERAL_BOOL:{
                    if(expr->expr_literal.value_bool){
                        sprint("true");
                    } else {
                        sprint("false");
                    }
                }break;
                case LITERAL_ARRAY:
                case LITERAL_STRUCT:
                    assert_msg(0, "literal array and struct not yet implemented");
                    break;
            }
        }break;
        case AST_EXPRESSION_PROCEDURE_CALL:{
			if (expr->expr_proc_call.decl->flags & DECL_PROC_FLAG_MAIN) {
				sprint("__");
			}
            sprint("%.*s(", TOKEN_STR(expr->expr_proc_call.name));
            for(s32 i = 0; i < expr->expr_proc_call.args_count; ++i){
                if(i != 0) sprint(",");
                emit_expression(expr->expr_proc_call.args[i]);
            }
            sprint(")");
        }break;
        case AST_EXPRESSION_UNARY:{
            sprint("(");
            switch(expr->expr_unary.op){
                case OP_UNARY_ADDRESSOF:{
                    sprint("&");
                }break;
                case OP_UNARY_BITWISE_NOT:{
                    sprint("~");
                }break;
                case OP_UNARY_CAST:{
                    sprint("(");
                    emit_type(expr->expr_unary.type_to_cast);
                    sprint(")");
                }break;
                case OP_UNARY_DEREFERENCE:{
                    sprint("*");
                }break;
                case OP_UNARY_LOGIC_NOT:{
                    sprint("!");
                }break;
                case OP_UNARY_MINUS:{
                    sprint("-");
                }break;
                case OP_UNARY_PLUS: break;
            }
            emit_expression(expr->expr_unary.operand);
            sprint(")");
        }break;
        case AST_EXPRESSION_VARIABLE:{
            sprint("%.*s", TOKEN_STR(expr->expr_variable.name));
        }break;
    }
}

void C_Code_Generator::emit_proc(Ast* decl) {
	if (decl->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) return;

	emit_decl(decl);
	
	// body
	emit_command(decl->decl_procedure.body);
	sprint("\n\n");
}

int C_Code_Generator::c_generate_top_level(Ast** toplevel, Type_Instance** type_table) {
    sprint("typedef char s8;\n");
    sprint("typedef short s16;\n");
    sprint("typedef int s32;\n");
    sprint("typedef long long s64;\n");
    sprint("typedef unsigned char u8;\n");
    sprint("typedef unsigned short u16;\n");
    sprint("typedef unsigned int u32;\n");
    sprint("typedef unsigned long long u64;\n");
    sprint("typedef u32 bool;\n");
    sprint("typedef float r32;\n");
    sprint("typedef double r64;\n");
	sprint("#define true 1\n");
	sprint("#define false 0\n");
    sprint("\n\n");

	sprint("// Forward declarations\n");
#if defined(_WIN32) || defined(_WIN64)
	sprint("u32 ExitProcess(u32 ret);\n");
#endif
    // emit all declarations of types
	Ast** type_decl_arr = type_decl_array_get();
    for(size_t i = 0; i < array_get_length(type_decl_arr); ++i){
        Ast* decl = type_decl_arr[i];
        emit_decl(decl);
        sprint(";\n");
    }
	sprint("\n");

    size_t ndecls = array_get_length(toplevel);
	// emit all declarations forward procedures
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_PROCEDURE ||
            decl->node_type == AST_DECL_CONSTANT || 
            decl->node_type == AST_DECL_VARIABLE) {
			emit_decl(decl, true);
            if(decl->node_type == AST_DECL_CONSTANT){
                sprint(" = ");
                emit_expression(decl->decl_constant.value);
            }
			sprint(";\n");
		}
	}
	sprint("\n");

	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_PROCEDURE) {
			emit_proc(decl);
		}
	}

	// Global variable initialization
#if defined(_WIN32) || defined(_WIN64)    
	sprint("\nvoid __entry() {\n");
#elif defined(__linux__)
    sprint("\nint __entry() {\n");
#endif
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_VARIABLE) {
			sprint("\t%.*s = ", TOKEN_STR(decl->decl_variable.name));
			if (decl->decl_variable.assignment) {
				// emit expression
				emit_expression(decl->decl_variable.assignment);
			} else {
				switch (decl->decl_variable.variable_type->kind) {
					case KIND_PRIMITIVE:
					case KIND_POINTER:
					case KIND_ARRAY:
					case KIND_FUNCTION:
						sprint("0");
						break;
					case KIND_STRUCT:
						sprint("{0}");
						break;
				}
			}
			sprint(";\n");
		}
	}

#if defined(_WIN32) || defined(_WIN64)
	sprint("\tExitProcess(__main());\n");
#elif defined(__linux__)
	sprint("\treturn __main();\n");
#endif
	sprint("}");

    return 0;
}

static size_t filename_length_strip_extension(char* f) {
	// Extract the base filename without the extension
	size_t baselen = strlen(f);
	size_t startlen = baselen;
	while (f[baselen] != '.') baselen--;
	if (baselen == 0) baselen = startlen;
	return baselen;
}

void c_generate(Ast** toplevel, Type_Instance** type_table, char* filename){
    C_Code_Generator code_generator = {};
	code_generator.buffer = (char*)ho_bigalloc_rw(1 << 20);

    int err = code_generator.c_generate_top_level(toplevel, type_table);

    size_t fname_len = filename_length_strip_extension(filename);
	string out_obj = string_new(filename, fname_len);
	string_append(&out_obj, ".c");
	
	if (code_generator.ptr == 0) return;
	HANDLE out = ho_createfile((const char*)out_obj.data, FILE_WRITE, CREATE_ALWAYS);
	if(out == INVALID_HANDLE_VALUE){
		report_fatal_error("Could not create file %s", out_obj.data);
	}
	ho_writefile(out, code_generator.ptr, (u8*)code_generator.buffer);
	ho_closefile(out);

	// Execute commands to compile .c
	char cmdbuffer[1024];
#if defined(_WIN32) || defined(_WIN64)
	sprintf(cmdbuffer, "gcc -c -g %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj -e__entry -nostdlib -o %.*s.exe lib/kernel32.lib lib/msvcrt.lib",
		fname_len, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
#elif defined(__linux__)
    sprintf(cmdbuffer, "gcc -c -g %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj temp/c_entry.o -o %.*s -s -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc",
		fname_len, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
#endif
	ho_bigfree(code_generator.buffer, 1 << 20);
}