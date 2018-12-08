#include "c_backend.h"
#include "decl_check.h"
#include <stdarg.h>

static s64 temp_variable;

static void report_fatal_error(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(-1);
}

bool print_debug_c = false;

int RuntimeBuffer::sprint(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	int num_written = 0;

	num_written = vsprintf(data + ptr, msg, args);
	ptr += num_written;

	va_end(args);

	if (print_debug_c) {
		va_list args;
		va_start(args, msg);
		int num_written = 0;

		num_written = vfprintf(stdout, msg, args);
		fflush(stdout);

		va_end(args);
	}
	return num_written;
}

int C_Code_Generator::sprint(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	int num_written = 0;

	if (!deferring) {
		num_written = vsprintf(buffer + ptr, msg, args);
		ptr += num_written;
	} else {
		num_written = vsprintf(defer_buffer + defer_ptr, msg, args);
		defer_ptr += num_written;
	}

	va_end(args);

	if(print_debug_c){
		if (!deferring) {
			va_list args;
			va_start(args, msg);
			int num_written = 0;

			num_written = vfprintf(stdout, msg, args);
			fflush(stdout);

			va_end(args);
		}
	}
	return num_written;
}

void C_Code_Generator::defer_flush() {
	if(print_debug_c) {
		fprintf(stdout, "%.*s", defer_ptr, defer_buffer);
		fflush(stdout);
	}

	memcpy(buffer + ptr, defer_buffer, defer_ptr);
	ptr += defer_ptr;
	defer_ptr = 0;
}

s64 C_Code_Generator::alloc_loop_id() {
	loop_id++;
	return loop_id - 1;
}

void C_Code_Generator::emit_typedef(Type_Instance** type_table, Type_Instance* type, Token* name, char* prefix) {
    static int id = 0;

    switch(type->kind){
        case KIND_POINTER:
        case KIND_PRIMITIVE:{
            sprint("typedef ");
            emit_type(type_table, type);
            if(prefix) sprint(" %s", prefix);
            if(name) sprint(" %.*s", TOKEN_STR(name));
            sprint(";\n");
        } break;

        case KIND_FUNCTION:{
            if(type->function_desc.return_type->kind == KIND_FUNCTION){
                char p[64] = {0};
                int rettype_id = id;
                sprintf(p, "%s__typet_%d", prefix, id++);
                emit_typedef(type_table, type->function_desc.return_type, 0, p);
                sprint("typedef __typet_%d ", rettype_id);
            } else {
                sprint("typedef ");
                emit_type(type_table, type->function_desc.return_type);
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
                emit_type(type_table, type->function_desc.arguments_type[i]);
            }
            sprint(");\n");

        }break;
        case KIND_ARRAY:
        case KIND_STRUCT:
            break;
    }
}

void C_Code_Generator::emit_type(Type_Instance** type_table, Type_Instance* type, Token* name){
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
            emit_type(type_table, type->pointer_to, name);
            sprint("*");
        }break;
        case KIND_ARRAY:{
            if(name){
                Type_Instance* auxtype = type;
                sprint("char %.*s[%lld]", TOKEN_STR(name), type->type_size_bits /8);
            } else {
                emit_type(type_table, type->array_desc.array_of);
                sprint("*");
            }
        }break;
        case KIND_FUNCTION:{
			sprint("__func_type_%p* ", type);
			if (name) {
				sprint("%.*s", TOKEN_STR(name));
			}
        }break;
		case KIND_UNION: {
			assert_msg(type->flags & TYPE_FLAG_INTERNALIZED, "tried to emit uninternalized type\n");
            // TODO(psv): does this always work in C?
            sprint("union ");
            if(name){
                sprint("%.*s", TOKEN_STR(name));
            } else {
                sprint("%.*s", TOKEN_STR(type->union_desc.name));
            }
		}break;
        case KIND_STRUCT: {
            assert_msg(type->flags & TYPE_FLAG_INTERNALIZED, "tried to emit uninternalized type\n");
            // TODO(psv): does this always work in C?
			sprint("struct ");
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
					assert_msg(0, "unexpected void expression in code generation");
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
		case KIND_UNION:
			sprint("{0}");
			break;
		default: assert_msg(0, "invalid default value"); break;
	}
}

void C_Code_Generator::emit_decl(Type_Instance** type_table, Ast* decl, bool forward) {
    assert(decl->flags & AST_FLAG_IS_DECLARATION);
    switch(decl->node_type) {
        case AST_DECL_PROCEDURE: {
            if(forward && decl->decl_procedure.type_return->kind == KIND_FUNCTION){
                emit_typedef(type_table, decl->decl_procedure.type_return, decl->decl_procedure.name, "__ret_");
            } 
            if(decl->decl_procedure.type_return->kind == KIND_FUNCTION){
                sprint("__ret_%.*s*", TOKEN_STR(decl->decl_procedure.name));
            } else {
                emit_type(type_table, decl->decl_procedure.type_return);
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
                emit_decl(type_table, arg);
            }
            sprint(")");
        }break;
        case AST_DECL_VARIABLE:{
            if(decl->decl_variable.variable_type->kind == KIND_FUNCTION){
                emit_type(type_table, decl->decl_variable.variable_type, decl->decl_variable.name);
            } else {
                if(decl->decl_variable.variable_type->kind == KIND_ARRAY){
                    emit_type(type_table, decl->decl_variable.variable_type, decl->decl_variable.name);
                } else {
                    emit_type(type_table, decl->decl_variable.variable_type);
                    sprint(" %.*s", TOKEN_STR(decl->decl_variable.name));
                }
            }
        }break;
        case AST_DECL_CONSTANT:{
            //emit_type(decl->decl_constant.type_info);
            //sprint(" %.*s", TOKEN_STR(decl->decl_constant.name));
        }break;
        case AST_DECL_STRUCT:{
			sprint("typedef struct %.*s{", TOKEN_STR(decl->decl_struct.name));
            size_t nfields = decl->decl_struct.fields_count;
            for(size_t i = 0; i < nfields; ++i){
                emit_decl(type_table, decl->decl_struct.fields[i]);
				sprint(";");
            }
            sprint("} %.*s", TOKEN_STR(decl->decl_struct.name));
        }break;
        case AST_DECL_UNION: {
			sprint("typedef union %.*s{", TOKEN_STR(decl->decl_union.name));
            size_t nfields = decl->decl_union.fields_count;
            for(size_t i = 0; i < nfields; ++i){
                emit_decl(type_table, decl->decl_union.fields[i]);
				sprint(";");
            }
            sprint("} %.*s", TOKEN_STR(decl->decl_union.name));
        }break;
        case AST_DECL_ENUM:{
            // implemented as CONSTANT for now
        }break;
    }
}

void C_Code_Generator::emit_array_assignment_from_base(Type_Instance** type_table, s64 offset_bytes, Ast* expr){
    // base is a char*
    if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY){
            size_t nexpr = 0;
            if(expr->expr_literal.array_exprs){
                nexpr = array_get_length(expr->expr_literal.array_exprs);
                for(size_t i = 0; i < nexpr; ++i) {
                    emit_array_assignment_from_base(type_table, offset_bytes, expr->expr_literal.array_exprs[i]);
                }
            }
    } else if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT) {
        sprint("__struct_base = __t_base;\n");
        sprint("{\n");
        sprint("char* __t_base = __struct_base;\n");
        emit_struct_assignment_from_base(type_table, 0, expr);
        sprint("}\n");
        sprint("__t_base += %lld;\n", (expr->type_return->type_size_bits / 8) + offset_bytes);
    } else {
        sprint("*(");
        emit_type(type_table, expr->type_return);
        sprint("*)__t_base = ");
        emit_expression(type_table, expr);
        sprint(";\n");
        sprint("__t_base += %lld;\n", (expr->type_return->type_size_bits / 8) + offset_bytes);
    }
}

void C_Code_Generator::emit_array_assignment(Type_Instance** type_table, Ast* decl) {
    Type_Instance* indexed_type = decl->decl_variable.variable_type;
    Ast* expr = decl->decl_variable.assignment;

    assert(indexed_type->kind == KIND_ARRAY);
	if (expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY) {
		sprint("{\n");
		sprint("char* __t_base = (char*)%.*s;\n", TOKEN_STR(decl->decl_variable.name));
		sprint("char* __struct_base = __t_base;\n");
		emit_array_assignment_from_base(type_table, 0, expr);
		sprint("}\n");
	} else {
		sprint("{\nchar* __arr_res = (char*)(");
		emit_expression(type_table, decl->decl_variable.assignment);
		sprint(");\n");
		sprint("__memory_copy(%.*s, __arr_res, %lld);\n", TOKEN_STR(decl->decl_variable.name), decl->decl_variable.variable_type->type_size_bits / 8);
		sprint("}\n");
	}
}

void C_Code_Generator::emit_array_assignment_to_temp(Type_Instance** type_table, Ast* expr) {
	Type_Instance* indexed_type = expr->type_return;
	assert(indexed_type->kind == KIND_ARRAY);
	assert(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY);

	sprint("{\n");
	sprint("char* __t_base = (char*)__temp_v_%lld;\n", temp_variable);
	sprint("char* __struct_base = __t_base;\n");
	emit_array_assignment_from_base(type_table, 0, expr);
	sprint("}\n");
}

void C_Code_Generator::emit_struct_assignment_to_temp(Type_Instance** type_table, Ast* expr) {
	Type_Instance* struct_type = expr->type_return;

	assert(struct_type->kind == KIND_STRUCT);
	if (expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT) {
		sprint("{\n");

		sprint("char* __t_base = (char*)&(__temp_v_%lld);\n", temp_variable);

		// array copies will use this
		sprint("char* __array_base = __t_base;\n");
		emit_struct_assignment_from_base(type_table, 0, expr);

		sprint("}\n");
	} else {
		sprint("%.*s = __temp_v_%lld", temp_variable);
		emit_expression(type_table, expr);
		sprint(";\n");
	}
}

void C_Code_Generator::emit_struct_assignment_from_base(Type_Instance** type_table, s64 offset_bytes, Ast* expr) {
    // base is a char*
    if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT){
            size_t nexpr = 0;
            Type_Instance* stype = expr->type_return;
            if(expr->expr_literal.struct_exprs){
                nexpr = array_get_length(expr->expr_literal.struct_exprs);
                for(size_t i = 0; i < nexpr; ++i) {
                    s64 localoffset_bytes = stype->struct_desc.offset_bits[i] / 8;
                    emit_struct_assignment_from_base(type_table, offset_bytes + localoffset_bytes, expr->expr_literal.array_exprs[i]);
                }
            }
    } else if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY) {
        sprint("{\n");
        sprint("char* __t_base = __array_base;\n");
        emit_array_assignment_from_base(type_table, offset_bytes, expr);
        sprint("}\n");
    } else {
        sprint("*(");
        emit_type(type_table, expr->type_return);
        sprint("*)((char*)__t_base + %lld) = ", offset_bytes);
        emit_expression(type_table, expr);
        sprint(";\n");
    }
}

void C_Code_Generator::emit_struct_assignment(Type_Instance** type_table, Ast* decl) {
    Type_Instance* struct_type = decl->decl_variable.variable_type;
    Ast* expr = decl->decl_variable.assignment;

    assert(struct_type->kind == KIND_STRUCT);
    if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT) {
        sprint("{\n");

        sprint("char* __t_base = (char*)&(%.*s);\n", TOKEN_STR(decl->decl_variable.name));

        // array copies will use this
        sprint("char* __array_base = __t_base;\n");
		// struct copies will use this
		sprint("char* __struct_base = __t_base;\n");
        emit_struct_assignment_from_base(type_table, 0, expr);

        sprint("}\n");
    } else {
		deferring = true;
        sprint("%.*s = ", TOKEN_STR(decl->decl_variable.name));
        emit_expression(type_table, decl->decl_variable.assignment);
        sprint(";\n");
		defer_flush();
    }
}

static Scope* get_scope_loop(Scope* scope, u64 level) {
	while (scope) {
		if (scope->flags & SCOPE_LOOP) --level;
		if (level <= 0) return scope;
		if (scope->flags & SCOPE_PROCEDURE_BODY) return false;
		if (scope->flags & SCOPE_ENUM) return false;
		if (scope->flags & SCOPE_STRUCTURE) return false;
		scope = scope->parent;
	}
	return 0;
}

void C_Code_Generator::emit_command(Type_Instance** type_table, Ast* comm, s64 deferred_index) {
	switch (comm->node_type) {
		case AST_COMMAND_BLOCK:{
			sprint("{\n");
			bool deferred = false;
			for (s32 i = 0; i < comm->comm_block.command_count; ++i) {
                Ast* cm = comm->comm_block.commands[i];
				if(deferred_index == i && comm->comm_block.block_scope->flags & SCOPE_LOOP) {
					sprint("loop_defer_%lld:\n", get_scope_loop(comm->comm_block.block_scope, 1));
					deferred = true;
				}
                
                if(cm->flags & AST_FLAG_IS_DECLARATION){
                    if(cm->node_type == AST_DECL_VARIABLE){
                        emit_decl(type_table, cm);
                        Ast* expr = cm->decl_variable.assignment;
                        if(!expr){
                            sprint(" = ", TOKEN_STR(cm->decl_variable.name));
                            emit_default_value(cm->decl_variable.variable_type);
                        }
                        sprint(";\n");
                        if(expr){
                            if(expr->type_return->kind == KIND_ARRAY){
                                emit_array_assignment(type_table, cm);
                            } else if(expr->type_return->kind == KIND_STRUCT) {
                                emit_struct_assignment(type_table, cm);
                            } else {
								deferring = true;
                                sprint("%.*s = ", TOKEN_STR(cm->decl_variable.name));
                                emit_expression(type_table, cm->decl_variable.assignment);
                                sprint(";");
								defer_flush();
								deferring = false;
                            }
                        }
					} else if (cm->node_type == AST_DECL_CONSTANT) {
						// a declaration of a constant does not generate any code
					} else {
                        assert_msg(0, "scoped declaration of procedure, enum and union not yet implemented");
                    }
                } else {
				    emit_command(type_table, cm);
                }
				sprint("\n");
			}
			if (!deferred && comm->comm_block.block_scope->flags & SCOPE_LOOP) {
				Scope* s =  get_scope_loop(comm->comm_block.block_scope, 1);
				if (s) {
					sprint("loop_defer_%llu:\n", s);
					sprint("continue;\n");
				}
			}
			sprint("}");
		}break;
		case AST_COMMAND_BREAK: {
			//sprint("break;");
			s64 level = comm->comm_break.level->expr_literal.value_u64;
			sprint("goto loop_%lld;", get_scope_loop(comm->scope, (u64)level));
		}break;
		case AST_COMMAND_CONTINUE: {
            sprint("goto loop_defer_%lld;", get_scope_loop(comm->scope, 1));
		}break;
		case AST_COMMAND_FOR:{
			s64 id = alloc_loop_id();
			comm->comm_for.id = id;

			deferring = true;
            sprint("while(");
            emit_expression(type_table, comm->comm_for.condition);
            sprint(")");
			deferring = false;
			defer_flush();
            emit_command(type_table, comm->comm_for.body, comm->comm_for.body->comm_block.command_count - comm->comm_for.deferred_commands);
			sprint("\nloop_%lld:;\n", get_scope_loop(comm->comm_for.body->comm_block.block_scope, 1));
        }break;
		case AST_COMMAND_IF:{

			deferring = true;
            sprint("if(");
            emit_expression(type_table, comm->comm_if.condition);
            sprint(")");
			deferring = false;
			defer_flush();
			sprint("{");
            emit_command(type_table, comm->comm_if.body_true);
			sprint("}");
            if(comm->comm_if.body_false){
                sprint(" else {");
                emit_command(type_table, comm->comm_if.body_false);
				sprint("}");
            }
        }break;
		case AST_COMMAND_RETURN:{
			deferring = true;
            sprint("return ");
            if(comm->comm_return.expression){
                emit_expression(type_table, comm->comm_return.expression);
            }
            sprint(";");
			deferring = false;
			defer_flush();
        }break;
		case AST_COMMAND_VARIABLE_ASSIGNMENT:{
			Ast* rval = comm->comm_var_assign.rvalue;
            
			// register size assignment
			if (type_regsize(rval->type_return) || rval->type_return->kind == KIND_FUNCTION) {
				deferring = true;
				if (comm->comm_var_assign.lvalue) {
					emit_expression(type_table, comm->comm_var_assign.lvalue);
					sprint(" = ");
				}
				emit_expression(type_table, comm->comm_var_assign.rvalue);
				sprint(";");
				deferring = false;
				defer_flush();
			}
			// bigger than regsize
			else {
				if(rval->node_type == AST_EXPRESSION_BINARY || rval->node_type == AST_EXPRESSION_PROCEDURE_CALL){
					deferring = true;
					if (comm->comm_var_assign.lvalue) {
                        emit_expression(type_table, comm->comm_var_assign.lvalue);
                        sprint(" = ");
                    }
                    emit_expression(type_table, comm->comm_var_assign.rvalue);
                    sprint(";");
					defer_flush();
					deferring = false;
				} else if(rval->node_type == AST_EXPRESSION_VARIABLE && 
					(rval->type_return->kind == KIND_STRUCT || rval->type_return->kind == KIND_UNION)){
                    if (comm->comm_var_assign.lvalue) {
                        emit_expression(type_table, comm->comm_var_assign.lvalue);
                        sprint(" = ");
                    }
                    emit_expression(type_table, comm->comm_var_assign.rvalue);
                    sprint(";");
                } else if(rval->node_type == AST_EXPRESSION_LITERAL && rval->expr_literal.type == LITERAL_STRUCT) {
                    sprint("{\n");
                    sprint("char* __t_base = (char*)&(");
                    emit_expression(type_table, comm->comm_var_assign.lvalue);
                    sprint(");\n");
                    emit_struct_assignment_from_base(type_table, 0, rval);
                    sprint("}\n");
                } else if(rval->node_type == AST_EXPRESSION_LITERAL && rval->expr_literal.type == LITERAL_ARRAY) {
                    sprint("{\n");
                    sprint("char* __t_base = (char*)(");
                    emit_expression(type_table, comm->comm_var_assign.lvalue);
                    sprint(");\n");
                    emit_array_assignment_from_base(type_table, 0, rval);
                    sprint("}\n");
				} else if (rval->type_return->kind == KIND_ARRAY) {
					// dst
					sprint("{\nchar* __arr_dst = (char*)(");
					emit_expression(type_table, comm->comm_var_assign.lvalue);
					sprint(");\n");

					// src
					sprint("char* __arr_src = (char*)(");
					emit_expression(type_table, rval);
					sprint(");\n");

					sprint("__memory_copy(__arr_dst, __arr_src, %llu);\n", rval->type_return->type_size_bits / 8);
					sprint("}\n");
				} else {
				    assert_msg(0, "assignment of type not recognized");
                }
			}
        } break;
	}
}

void C_Code_Generator::emit_temp_assignment(Type_Instance** type_table, Ast* expr) {
	deferring = false;
	if (expr->type_return->kind == KIND_ARRAY) {
		sprint("char ");
	} else {
		emit_type(type_table, expr->type_return);
	}
	sprint(" __temp_v_%lld", temp_variable);
    if (expr->type_return->kind == KIND_ARRAY) {
        sprint("[%lld]", expr->type_return->type_size_bits / 8);
    }
    sprint(";\n");

	emit_expression(type_table, expr);
	deferring = true;
	sprint("__temp_v_%lld", temp_variable);
	temp_variable += 1;
}

void C_Code_Generator::emit_expression_binary(Type_Instance** type_table, Ast* expr){
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
        case OP_BINARY_NOT_EQUAL: {
			sprint("((");
            emit_expression(type_table, expr->expr_binary.left);
			sprint(")");
			sprint(binop_op_to_string(expr->expr_binary.op));
            emit_expression(type_table, expr->expr_binary.right);
			sprint(")");
        }break;

		case OP_BINARY_DOT: {
			if(expr->expr_binary.left->flags & AST_FLAG_ENUM_ACCESSOR) {
				emit_expression(type_table, expr->expr_binary.right);
			} else {
				sprint("((");
				emit_expression(type_table, expr->expr_binary.left);
				sprint(")");
				sprint(binop_op_to_string(expr->expr_binary.op));
				emit_expression(type_table, expr->expr_binary.right);
				sprint(")");
			}
		}break;

        case OP_BINARY_VECTOR_ACCESS:{
            Type_Instance* indexed_type = expr->expr_binary.left->type_return;

			if(expr->type_return->kind != KIND_ARRAY){
				sprint("*(");
				emit_type(type_table, indexed_type);
				sprint(")");
			}

			sprint("(");

			sprint("(char*)");
			emit_expression(type_table, expr->expr_binary.left);
			sprint(" + ");

			emit_expression(type_table, expr->expr_binary.right);
			sprint(" * ");

			if(indexed_type->kind == KIND_POINTER){
				sprint(" %lld ", indexed_type->pointer_to->type_size_bits / 8);
            } else {
				sprint(" %lld ", indexed_type->array_desc.array_of->type_size_bits / 8);
            }

			sprint(")");
        }break;
    }
}

void C_Code_Generator::emit_expression(Type_Instance** type_table, Ast* expr){
    assert(expr->flags & AST_FLAG_IS_EXPRESSION || expr->node_type == AST_DATA);

    switch(expr->node_type){
		case AST_EXPRESSION_DIRECTIVE: {
			if(expr->expr_directive.type == EXPR_DIRECTIVE_TYPEOF) {
				Type_Instance* type = expr->expr_directive.expr->type_return;
				//printf("type table index: %d\n", type->type_table_index);
				sprint("(__type_table + %d)", type->type_table_index * sizeof(User_Type_Info));
			} else {
				assert_msg(0, "unimplemented expression directive");
			}
		}break;
        case AST_EXPRESSION_BINARY:{
            emit_expression_binary(type_table, expr);
        }break;
        case AST_EXPRESSION_LITERAL:{
			//assert_msg(expr->type_return->kind == KIND_PRIMITIVE, "integer literal of type not primitive");
            switch(expr->expr_literal.type){
				case LITERAL_SINT:
                case LITERAL_BIN_INT:
                case LITERAL_HEX_INT:
                case LITERAL_UINT:{
					switch (expr->type_return->primitive) {
						case TYPE_PRIMITIVE_S8:
							sprint("0x%x", (s8)expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_S16:
							sprint("0x%x", (s16)expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_S32:
							sprint("0x%x", (s32)expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_S64:
							sprint("0x%llx", expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_U8:
							sprint("0x%x", (u8)expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_U16:
							sprint("0x%x", (u16)expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_U32:
							sprint("0x%x", (u32)expr->expr_literal.value_u64);
							break;
						case TYPE_PRIMITIVE_U64:
							sprint("0x%llx", expr->expr_literal.value_u64);
							break;
					}
                    
                }break;
                case LITERAL_FLOAT:{
					if (expr->type_return->primitive == TYPE_PRIMITIVE_R32) {
						sprint("%f", (r32)expr->expr_literal.value_r64);
					} else {
						sprint("%f", expr->expr_literal.value_r64);
					}
                }break;
                case LITERAL_BOOL:{
                    if(expr->expr_literal.value_bool){
						sprint("true");
                    } else {
						sprint("false");
                    }
                }break;
				case LITERAL_POINTER: {
					sprint("(void*)0");
				}break;
				case LITERAL_ARRAY: {
					if (deferring) {
						emit_temp_assignment(type_table, expr);
					} else {
						emit_array_assignment_to_temp(type_table, expr);
					}
				}break;
				case LITERAL_STRUCT: {
					if (deferring) {
						emit_temp_assignment(type_table, expr);
					} else {
						emit_struct_assignment_to_temp(type_table, expr);
					}
				} break;
            }
        }break;
        case AST_EXPRESSION_PROCEDURE_CALL:{
            sprint("(");
            emit_expression(type_table, expr->expr_proc_call.caller);
            sprint(")(");
            for(s32 i = 0; i < expr->expr_proc_call.args_count; ++i){
                if(i != 0) sprint(",");
                emit_expression(type_table, expr->expr_proc_call.args[i]);
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
					emit_type(type_table, expr->expr_unary.type_to_cast);
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
            emit_expression(type_table, expr->expr_unary.operand);
			sprint(")");
        }break;
        case AST_EXPRESSION_VARIABLE:{
            Ast* decl = decl_from_name(expr->scope, expr->expr_variable.name);
            if(decl->node_type == AST_DECL_CONSTANT){
                emit_expression(type_table, decl->decl_constant.value);
            } else {
                if(expr->expr_variable.name->value.data == compiler_tags[COMPILER_TAG_MAIN_PROC].data){
                    sprint("__");
                }
			    sprint("%.*s", TOKEN_STR(expr->expr_variable.name));
            }
        }break;
        case AST_DATA: {
            if(expr->data_global.type == GLOBAL_STRING) {
				sprint("__string_data_%d", expr->data_global.id);
            } else {
                assert_msg(0, "undefined data type");
            }
        }break;
    }
}

int C_Code_Generator::sprint_data_raw(Ast_Data* data) {
	u32 aux_len = ptr;
	for (s64 i = 0; i < data->length_bytes; ++i) {
		u8 d = data->data[i];
		buffer[aux_len++] = '\\';
		buffer[aux_len++] = 'x';
		buffer[aux_len++] = CHAR_HIGH_HEX(d);
		buffer[aux_len++] = CHAR_LOW_HEX(d);
	}

	int bytes_written = aux_len - ptr;
	ptr += bytes_written;
	return bytes_written;
}

int C_Code_Generator::sprint_data(Ast_Data* data) {
	u32 aux_len = ptr;
	s64 length_reduced = 0;
	for (s64 i = 0; i < data->length_bytes; ++i) {
		u8 d = data->data[i];
		switch (d) {
		case '"': {
			buffer[aux_len++] = '\\';
			buffer[aux_len++] = '"';
			length_reduced += 1;
		} break;
		case '\\': {
			buffer[aux_len++] = '\\';
			d = data->data[++i];
			length_reduced += 1;
			switch (d) {
			case 'a':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case 'v':
			case 'e':
			case '\\':
			case '\'':
			case '"':
			case '?':
				buffer[aux_len++] = d;
				break;
			case 'x': {
				length_reduced += 1;
				if (is_hex_digit(d) || is_number(d)) {
					buffer[aux_len++] = d;
					if (is_hex_digit(data->data[i + 1]) || is_number(data->data[i + 1])) {
						d = data->data[++i];
						length_reduced += 1;
						buffer[aux_len++] = d;
					}
				}
				else {
					assert_msg(0, "invalid escape sequence in code generation");
				}
			} break;
			default: {
				assert_msg(0, "invalid escape sequence in code generation");
			}break;
			}
		}break;

		default: {
			// ' ' to '~'
			if (d >= 0x20 && d <= 0x7e) {
				// normal ascii characters
				buffer[aux_len++] = d;
			}
			else {
				buffer[aux_len++] = '\\';
				buffer[aux_len++] = 'x';
				buffer[aux_len++] = CHAR_HIGH_HEX(d);
				buffer[aux_len++] = CHAR_LOW_HEX(d);
				buffer[aux_len++] = '"';
				buffer[aux_len++] = '"';
			}
		}break;
		}
	}
	data->length_bytes -= length_reduced;
	int bytes_written = aux_len - ptr;
	ptr += bytes_written;
	return bytes_written;
}

void C_Code_Generator::emit_data_decl(Ast* decl) {
    if(decl->data_global.type == GLOBAL_STRING) {
        sprint("char* __string_data_%d = \"", decl->data_global.id);
		sprint_data(&decl->data_global);
		sprint("\";\n");
        //sprint("%.*s\";\n", decl->data_global.length_bytes, decl->data_global.data);
    } else {
        assert_msg(0, "trying to generate code for undefined data node");
    }
}

void C_Code_Generator::emit_proc(Type_Instance** type_table, Ast* decl) {
	if (decl->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) return;

	emit_decl(type_table, decl);
	
	// body
	emit_command(type_table, decl->decl_procedure.body);
	sprint("\n\n");
}

void C_Code_Generator::emit_function_typedef(Type_Instance** type_table, Type_Instance* type) {
	assert(type->kind == KIND_FUNCTION);
	assert(type->flags & TYPE_FLAG_INTERNALIZED);
	sprint("typedef ");

	Type_Instance* rettype = type->function_desc.return_type;
	if (rettype->kind == KIND_FUNCTION) {
		sprint("__func_type_%p*", rettype);
	} else {
		emit_type(type_table, rettype);
	}
	sprint(" __func_type_%p(", type);

	size_t nargs = type->function_desc.num_arguments;

	for (size_t i = 0; i < nargs; ++i) {
		if (i != 0) sprint(", ");
		emit_type(type_table, type->function_desc.arguments_type[i]);
	}

	sprint(");\n");
}

#include <light_arena.h>

Type_Instance* fill_type_table_relative_pointer(Type_Instance** type_table, Type_Table_Copy* ttc) {
	// TODO(psv): replace arena with contiguous memory allocator
	// @IMPORTANT
	Light_Arena* arena = arena_create(65536 * 10);
	Light_Arena* strings_type = arena_create(65536 * 10);
	//u8* strings_type = array_create(u8, 65536);
	Type_Instance* type_table_copy = (Type_Instance*)calloc(array_get_length(type_table), sizeof(Type_Instance));

	u8* extra_space = 0;
	u8* extra_string_space = 0;
	u8* start_strings = (u8*)strings_type->ptr;
	u8* start = (u8*)arena->ptr;
	u8* offset = start;

	ttc->start_extra_mem = start;
	ttc->start_extra_strings = start_strings;

	for (size_t i = 0; i < array_get_length(type_table); ++i) {
		type_table[i]->type_table_index = (s32)i;
		type_table_copy[i] = *type_table[i];
	}

	for (size_t i = 0, s = 0; i < array_get_length(type_table); ++i) {
		Type_Instance* type = type_table[i];
		Type_Instance* type_copy = &type_table_copy[i];
		switch (type->kind) {
			case KIND_PRIMITIVE: break;
			case KIND_POINTER: {
				type_table_copy[i].pointer_to = (Type_Instance*)type_table_copy[i].pointer_to->type_table_index;
			} break;
			case KIND_STRUCT: {
				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(Type_Instance*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->struct_desc.fields_count; ++j) {
					((Type_Instance**)extra_space)[j] = (Type_Instance*)type->struct_desc.fields_types[j]->type_queue_index;
				}
				// TODO(psv): leaking here for now
				type_copy->struct_desc.fields_types = (Type_Instance**)offset;

				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(string));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->struct_desc.fields_count; ++j) {
					((string*)extra_space)[j] = (string)type->struct_desc.fields_names[j];
					s64 length = ((string*)extra_space)[j].length;
					extra_string_space = (u8*)arena_alloc(strings_type, length);
					memcpy(extra_string_space, ((string*)extra_space)[j].data, length);
					ttc->extra_strings_bytes += length;
					((string*)extra_space)[j].data = (u8*)(extra_string_space - start_strings);
				}
				type_copy->struct_desc.fields_names = (string*)offset;

				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(s64));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->struct_desc.fields_count; ++j) {
					((s64*)extra_space)[j] = (s64)type->struct_desc.offset_bits[j];
				}
				type_copy->struct_desc.offset_bits = (s64*)offset;
			} break;

			case KIND_UNION: {
				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(Type_Instance*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < array_get_length(type->union_desc.fields_types); ++j) {
					((Type_Instance**)extra_space)[j] = (Type_Instance*)type->union_desc.fields_types[j]->type_queue_index;
				}
				type_copy->union_desc.fields_types = (Type_Instance**)offset;

				extra_space = (u8*)arena_alloc(arena, type->union_desc.fields_count * sizeof(string));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->union_desc.fields_count; ++j) {
					((string*)extra_space)[j] = (string)type->union_desc.fields_names[j];
					s64 length = ((string*)extra_space)[j].length;
					extra_string_space = (u8*)arena_alloc(strings_type, length);
					memcpy(extra_string_space, ((string*)extra_space)[j].data, length);
					ttc->extra_strings_bytes += length;
					((string*)extra_space)[j].data = (u8*)(extra_string_space - start_strings);
				}
				type_copy->union_desc.fields_names = (string*)offset;
			} break;

			case KIND_ARRAY: {
				type_copy->array_desc.array_of = (Type_Instance*)type->array_desc.array_of->type_table_index;
			} break;
			case KIND_FUNCTION: {
				extra_space = (u8*)arena_alloc(arena, type->function_desc.num_arguments * sizeof(Type_Instance*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->function_desc.num_arguments; ++j) {
					((Type_Instance**)extra_space)[j] = (Type_Instance*)type->function_desc.arguments_type[j]->type_table_index;
				}
				type_copy->function_desc.arguments_type = (Type_Instance**)offset;

#if 0
				extra_space = (u8*)arena_alloc(arena, type->function_desc.num_arguments * sizeof(string*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->function_desc.num_arguments; ++j) {
					((string*)extra_space)[j] = (string)type->function_desc.arguments_names[j];
					s64 length = ((string*)extra_space)[j].length;
					extra_string_space = (u8*)arena_alloc(strings_type, length);
					memcpy(extra_string_space, ((string*)extra_space)[j].data, length);
					ttc->extra_strings_bytes += length;
					((string*)extra_space)[j].data = (u8*)(extra_string_space - start_strings);
				}
				type_copy->function_desc.arguments_names = (string*)offset;
#endif
			} break;
			default: break;
		}
	}
	ttc->extra_mem_bytes = (u8*)arena->ptr - start;

	return type_table_copy;
}

#if 0
void C_Code_Generator::emit_type_strings(Type_Table_Copy* ttc) {
#else
void C_Code_Generator::emit_type_strings(User_Type_Table* ttc) {
#endif
	sprint("u8 __type_strings[] = \"");

	Ast_Data d;
	d.data = ttc->start_extra_strings;
	d.length_bytes = ttc->extra_strings_bytes;
	sprint_data_raw(&d);

	sprint("\";\n\n");

	// ----

	sprint("u8 __type_extra[] = \"");

	Ast_Data d_te;
	d_te.data = ttc->start_extra_mem;
	d_te.length_bytes = ttc->extra_mem_bytes;
	sprint_data_raw(&d_te);

	sprint("\";\n\n");

	// ----

	sprint("u8 __type_table[] = \"");

	Ast_Data d_tt;
	d_tt.data = (u8*)ttc->type_table;
	d_tt.length_bytes = ttc->type_table_length * sizeof(User_Type_Info);
	sprint_data_raw(&d_tt);

	sprint("\";\n\n");
}

inline User_Type_Info type_instance_to_user(Type_Instance& t) {
	User_Type_Info result = {};
	result.kind = t.kind;
	result.type_size_bytes = t.type_size_bits / 8;

	switch(t.kind) {
		case KIND_PRIMITIVE:
			result.description.primitive = t.primitive;
			break;
		case KIND_POINTER:
			result.description.pointer_to = (User_Type_Info*)t.pointer_to;
			break;
		case KIND_ARRAY:
			result.description.array_desc.array_of = (User_Type_Info*)t.array_desc.array_of;
			result.description.array_desc.dimension = t.array_desc.dimension;
			break;
		case KIND_STRUCT:
			result.description.struct_desc.name = t.struct_desc.name->value;
			result.description.struct_desc.fields_count = t.struct_desc.fields_count;
			result.description.struct_desc.alignment = t.struct_desc.alignment;
			result.description.struct_desc.fields_types = (User_Type_Info**)t.struct_desc.fields_types;
			result.description.struct_desc.fields_names = t.struct_desc.fields_names;
			result.description.struct_desc.fields_offsets_bits = t.struct_desc.offset_bits;
			break;
		case KIND_UNION:
			result.description.union_desc.name = t.union_desc.name->value;
			result.description.union_desc.fields_count = t.union_desc.fields_count;
			result.description.union_desc.alignment = t.union_desc.alignment;
			result.description.union_desc.fields_types = (User_Type_Info**)t.union_desc.fields_types;
			result.description.union_desc.fields_names = t.union_desc.fields_names;
			break;
		case KIND_FUNCTION:
			result.description.function_desc.arguments_count = t.function_desc.num_arguments;
			result.description.function_desc.arguments_name = t.function_desc.arguments_names;
			result.description.function_desc.arguments_type = (User_Type_Info**)t.function_desc.arguments_type;
			result.description.function_desc.return_type = (User_Type_Info*)t.function_desc.return_type;
			break;
		default: assert_msg(0, "invalid type kind"); break;
	}

	return result;
}

User_Type_Info* fill_user_type_table(Type_Instance** type_table, User_Type_Table* utt, RuntimeBuffer* runtime_buffer) {
	// TODO(psv): replace arena with contiguous memory allocator
	// @IMPORTANT
	Light_Arena* arena = arena_create(65536 * 10);
	Light_Arena* strings_type = arena_create(65536 * 10);
	//u8* strings_type = array_create(u8, 65536);
	User_Type_Info* user_tt = (User_Type_Info*)calloc(array_get_length(type_table), sizeof(User_Type_Info));

	u8* extra_space = 0;
	u8* extra_string_space = 0;
	u8* start_strings = (u8*)strings_type->ptr;
	u8* start = (u8*)arena->ptr;
	u8* offset = start;

	utt->start_extra_mem = start;
	utt->start_extra_strings = start_strings;

	for (size_t i = 0; i < array_get_length(type_table); ++i) {
		type_table[i]->type_table_index = (s32)i;
		user_tt[i] = type_instance_to_user(*type_table[i]);
	}

	for (size_t i = 0, s = 0; i < array_get_length(type_table); ++i) {
		Type_Instance* type = type_table[i];
		User_Type_Info* type_copy = &user_tt[i];
		switch (type->kind) {
			case KIND_PRIMITIVE: break;
			case KIND_POINTER: {
				user_tt[i].description.pointer_to = (User_Type_Info*)((Type_Instance*)user_tt[i].description.pointer_to)->type_table_index;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_table + %lld;\n", 
					(u8*)&user_tt[i].description.pointer_to - (u8*)user_tt, (s64)user_tt[i].description.pointer_to * sizeof(User_Type_Info));
			} break;
			case KIND_STRUCT: {
				extra_string_space = (u8*)arena_alloc(strings_type, type->struct_desc.name->value.length);
				memcpy(extra_string_space, type->struct_desc.name->value.data, type->struct_desc.name->value.length);
				utt->extra_strings_bytes += type->struct_desc.name->value.length;
				user_tt[i].description.struct_desc.name.data = (u8*)(extra_string_space - start_strings);
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_strings + %lld;\n", 
					(u8*)&user_tt[i].description.struct_desc.name.data - (u8*)user_tt, (s64)(u8*)(extra_string_space - start_strings));

				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(User_Type_Info*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->struct_desc.fields_count; ++j) {
					s64 type_queue_index = ((Type_Instance*)type->struct_desc.fields_types[j])->type_queue_index;
					((User_Type_Info**)extra_space)[j] = (User_Type_Info*)type_queue_index;
					runtime_buffer->sprint("*(u8**)(__type_extra + %lld) = __type_table + %lld;\n", 
						(u8*)&((User_Type_Info**)extra_space)[j] - start, type_queue_index * sizeof(User_Type_Info));
				}
				// TODO(psv): leaking here for now
				type_copy->description.struct_desc.fields_types = (User_Type_Info**)offset;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_extra + %lld;\n",
					(u8*)&user_tt[i].description.struct_desc.fields_types - (u8*)user_tt, (s64)offset);

				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(string));
				offset = (u8*)(extra_space - start);
				
				for (size_t j = 0; j < type->struct_desc.fields_count; ++j) {
					((string*)extra_space)[j] = (string)type->struct_desc.fields_names[j];
					s64 length = ((string*)extra_space)[j].length;
					extra_string_space = (u8*)arena_alloc(strings_type, length);
					memcpy(extra_string_space, ((string*)extra_space)[j].data, length);
					utt->extra_strings_bytes += length;
					((string*)extra_space)[j].data = (u8*)(extra_string_space - start_strings);

					runtime_buffer->sprint("*(u8**)(__type_extra + %lld) = __type_strings + %lld;\n",
						(s64)&((string*)offset + j)->data, (s64)(extra_string_space - start_strings));
				}

				type_copy->description.struct_desc.fields_names = (string*)offset;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_extra + %lld;\n",
					(u8*)&user_tt[i].description.struct_desc.fields_names - (u8*)user_tt, offset);

				extra_space = (u8*)arena_alloc(arena, type->struct_desc.fields_count * sizeof(s64));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->struct_desc.fields_count; ++j) {
					((s64*)extra_space)[j] = (s64)type->struct_desc.offset_bits[j];
				}
				type_copy->description.struct_desc.fields_offsets_bits = (s64*)offset;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_extra + %lld;\n",
					(u8*)&user_tt[i].description.struct_desc.fields_offsets_bits - (u8*)user_tt, offset);
			} break;

			case KIND_UNION: {
				extra_string_space = (u8*)arena_alloc(strings_type, type->union_desc.name->value.length);
				memcpy(extra_string_space, type->union_desc.name->value.data, type->union_desc.name->value.length);
				utt->extra_strings_bytes += type->union_desc.name->value.length;
				user_tt[i].description.union_desc.name.data = (u8*)(extra_string_space - start_strings);
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_strings + %lld;\n", 
					(u8*)&user_tt[i].description.union_desc.name.data - (u8*)user_tt, (s64)(u8*)(extra_string_space - start_strings));

				// Fill in types for the fields in the union
				extra_space = (u8*)arena_alloc(arena, type->union_desc.fields_count * sizeof(User_Type_Info*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->union_desc.fields_count; ++j) {
					s64 type_queue_index = ((Type_Instance*)type->union_desc.fields_types[j])->type_queue_index;
					((User_Type_Info**)extra_space)[j] = (User_Type_Info*)type_queue_index;
					runtime_buffer->sprint("*(u8**)(__type_extra + %lld) = __type_table + %lld;\n", 
						(u8*)&((User_Type_Info**)extra_space)[j] - start, type_queue_index * sizeof(User_Type_Info));
				}
				// TODO(psv): leaking here for now
				type_copy->description.union_desc.fields_types = (User_Type_Info**)offset;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_extra + %lld;\n",
					(u8*)&user_tt[i].description.union_desc.fields_types - (u8*)user_tt, (s64)offset);

				// Fill fields names for the union
				extra_space = (u8*)arena_alloc(arena, type->union_desc.fields_count * sizeof(string));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->union_desc.fields_count; ++j) {
					((string*)extra_space)[j] = (string)type->union_desc.fields_names[j];
					s64 length = ((string*)extra_space)[j].length;
					extra_string_space = (u8*)arena_alloc(strings_type, length);
					memcpy(extra_string_space, ((string*)extra_space)[j].data, length);
					utt->extra_strings_bytes += length;
					((string*)extra_space)[j].data = (u8*)(extra_string_space - start_strings);

					runtime_buffer->sprint("*(u8**)(__type_extra + %lld) = __type_strings + %lld;\n",
						(s64)&((string*)offset + j)->data, (s64)(extra_string_space - start_strings));
				}
				type_copy->description.union_desc.fields_names = (string*)offset;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_extra + %lld;\n",
					(u8*)&user_tt[i].description.union_desc.fields_names - (u8*)user_tt, offset);
			} break;

			case KIND_ARRAY: {
				type_copy->description.array_desc.array_of = (User_Type_Info*)((Type_Instance*)type->array_desc.array_of)->type_table_index;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_table + %lld;\n", 
					(u8*)&user_tt[i].description.array_desc.array_of - (u8*)user_tt, (s64)user_tt[i].description.array_desc.array_of * sizeof(User_Type_Info));
			} break;
			case KIND_FUNCTION: {
				// Arguments types
				extra_space = (u8*)arena_alloc(arena, type->function_desc.num_arguments * sizeof(User_Type_Info*));
				offset = (u8*)(extra_space - start);
				for (size_t j = 0; j < type->function_desc.num_arguments; ++j) {
					s64 type_table_index = ((Type_Instance*)type->function_desc.arguments_type[j])->type_table_index;
					((User_Type_Info**)extra_space)[j] = (User_Type_Info*)type_table_index;
					runtime_buffer->sprint("*(u8**)(__type_extra + %lld) = __type_table + %lld;\n", 
						(u8*)&((User_Type_Info**)extra_space)[j] - start, type_table_index * sizeof(User_Type_Info));
				}
				type_copy->description.function_desc.arguments_type = (User_Type_Info**)offset;
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_extra + %lld;\n",
					(u8*)&user_tt[i].description.function_desc.arguments_type - (u8*)user_tt, offset);

				// Return Type
				type_copy->description.function_desc.return_type = (User_Type_Info*)((Type_Instance*)type->function_desc.return_type->type_table_index);
				runtime_buffer->sprint("*(u8**)(__type_table + %lld) = __type_table + %lld;\n",
					(u8*)&user_tt[i].description.function_desc.return_type - (u8*)user_tt, 
					type->function_desc.return_type->type_table_index * sizeof(User_Type_Info));
			} break;
			default: break;
		}
	}
	utt->extra_mem_bytes = (u8*)arena->ptr - start;

	return user_tt;
}

int C_Code_Generator::c_generate_top_level(Ast** toplevel, Type_Instance** type_table, RuntimeBuffer* runtime_buffer) {
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

	sprint("void __memory_copy(void* dest, void* src, u64 size) {\n");
	sprint("\tfor(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];\n");
	sprint("}\n");

#if 0
	Type_Table_Copy ttc = { 0 };
	Type_Instance* tt = fill_type_table_relative_pointer(type_table, &ttc);
	ttc.type_table = tt;
	ttc.type_table_length = array_get_length(type_table);
	emit_type_strings(&ttc);
#else
	User_Type_Table utt = { 0 };
	User_Type_Info* tt = fill_user_type_table(type_table, &utt, runtime_buffer);
	utt.type_table = tt;
	utt.type_table_length = array_get_length(type_table);
	emit_type_strings(&utt);
#endif

    // forward declarations of types
    Ast** type_decl_arr = type_decl_array_get();
    for(size_t i = 0; i < array_get_length(type_decl_arr); ++i){
        Ast* decl = type_decl_arr[i];
        if(decl->node_type == AST_DECL_STRUCT) {
			sprint("typedef struct %.*s %.*s;\n", TOKEN_STR(decl->decl_struct.name), TOKEN_STR(decl->decl_struct.name));
        }
		if(decl->node_type == AST_DECL_UNION) {
			sprint("typedef union %.*s %.*s;\n", TOKEN_STR(decl->decl_union.name), TOKEN_STR(decl->decl_union.name));
		}
    }

    // emit all declarations of types
    for (size_t i = 0; i < array_get_length(type_table); ++i) {
		Type_Instance* type = type_table[i];
		if (type->kind == KIND_FUNCTION) {
			emit_function_typedef(type_table, type);
		}
	}
    
    // emit structs and proc declarations
    for(size_t i = 0; i < array_get_length(type_decl_arr); ++i){
        Ast* decl = type_decl_arr[i];
        emit_decl(type_table, decl);
        sprint(";\n");
    }
	
	sprint("\n");

    size_t ndecls = array_get_length(toplevel);
	// emit all declarations forward procedures and strings
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_PROCEDURE ||
            decl->node_type == AST_DECL_VARIABLE) 
		{
			deferring = true;
			emit_decl(type_table, decl, true);
			sprint(";\n");
			deferring = false;
			defer_flush();
		} else if (decl->node_type == AST_DATA) {
            emit_data_decl(decl);
        }
	}
	sprint("\n");

	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_PROCEDURE) {
			emit_proc(type_table, decl);
		}
	}

	// Global variable initialization
#if defined(_WIN32) || defined(_WIN64)    
	sprint("\nvoid __entry() {\n");
#elif defined(__linux__)
    sprint("\nint __entry() {\n");
#endif
	sprint(runtime_buffer->data);

	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_VARIABLE) {
			if (decl->decl_variable.assignment) {
				// emit expression
                switch(decl->decl_variable.assignment->type_return->kind){
                    case KIND_ARRAY:{
                        emit_array_assignment(type_table, decl);
                    }break;
                    case KIND_STRUCT:{
                        emit_struct_assignment(type_table, decl);
                    }break;
                    case KIND_FUNCTION:
                    case KIND_POINTER:
                    case KIND_PRIMITIVE:{
						deferring = true;
			            sprint("\t%.*s = ", TOKEN_STR(decl->decl_variable.name));
				        emit_expression(type_table, decl->decl_variable.assignment);
						deferring = false;
						defer_flush();
                    }break;
                }
			} else {
				// default value is assigned later
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

void c_generate(Ast** toplevel, Type_Instance** type_table, char* filename, char* compiler_path, string* libs_to_link) {
    size_t compiler_fullpath_length = 0;
    char* compiler_fullpath = ho_realpath(compiler_path, &compiler_fullpath_length);
    string comp_path = path_from_fullpath(string_make(compiler_fullpath));

    C_Code_Generator code_generator = {};
	code_generator.buffer = (char*)ho_bigalloc_rw(1 << 24);
	code_generator.defer_buffer = (char*)ho_bigalloc_rw(1 << 20);
	RuntimeBuffer runtime_buffer;
	runtime_buffer.data = (char*)ho_bigalloc_rw(1 << 20);
	runtime_buffer.ptr = 0;

    Timer timer;

    double start = timer.GetTime();
    int err = code_generator.c_generate_top_level(toplevel, type_table, &runtime_buffer);
    double end = timer.GetTime();
    printf("C generation elapsed: %fms\n", (end - start));

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
	sprintf(cmdbuffer, "gcc -w -c -g %.*s -o %.*s.obj", out_obj.length, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	int len = sprintf(cmdbuffer, "ld %.*s.obj -e__entry -nostdlib -o %.*s.exe -L%.*s..\\..\\lib -lKernel32",
		fname_len, out_obj.data, fname_len, out_obj.data, comp_path.length, comp_path.data);
#elif defined(__linux__)
    sprintf(cmdbuffer, "gcc -w -c %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	int len = sprintf(cmdbuffer, "ld %.*s.obj %.*s../../temp/c_entry.o -o %.*s -s -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc",// -lc -lX11 -lGL",
		fname_len, out_obj.data, comp_path.length, comp_path.data, fname_len, out_obj.data);
#endif
	size_t libs_length = 0;
	if (libs_to_link) {
		libs_length = array_get_length(libs_to_link);
	}
	for (size_t i = 0; i < libs_length; ++i) {
		len += sprintf(cmdbuffer + len, " -l%.*s", libs_to_link[i].length, libs_to_link[i].data);
	}
	system(cmdbuffer);
	ho_bigfree(code_generator.buffer, 1 << 20);
}
