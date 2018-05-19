#include "c_backend.h"
#include "decl_check.h"
#include <stdarg.h>

static s64 temp_variable;

static void report_fatal_error(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	//assert(0);
	exit(-1);
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
	return num_written;
}

void C_Code_Generator::defer_flush() {
	memcpy(buffer + ptr, defer_buffer, defer_ptr);
	ptr += defer_ptr;
	defer_ptr = 0;
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
                sprintf(p, "%s__typet_%d", prefix, id++);
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
            emit_type(type->pointer_to, name);
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
			sprint("__func_type_%p* ", type);
			if (name) {
				sprint("%.*s", TOKEN_STR(name));
			}

			/*
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
			*/
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
            //emit_type(decl->decl_constant.type_info);
            //sprint(" %.*s", TOKEN_STR(decl->decl_constant.name));
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

void C_Code_Generator::emit_array_assignment_from_base(s64 offset_bytes, Ast* expr){
    // base is a char*
    if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY){
            size_t nexpr = 0;
            if(expr->expr_literal.array_exprs){
                nexpr = array_get_length(expr->expr_literal.array_exprs);
                for(size_t i = 0; i < nexpr; ++i) {
                    emit_array_assignment_from_base(offset_bytes, expr->expr_literal.array_exprs[i]);
                }
            }
    } else if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT) {
        sprint("__struct_base = __t_base;\n");
        sprint("{\n");
        sprint("char* __t_base = __struct_base;\n");
        emit_struct_assignment_from_base(0, expr);
        sprint("}\n");
        sprint("__t_base += %lld;\n", (expr->type_return->type_size_bits / 8) + offset_bytes);
    } else {
        sprint("*(");
        emit_type(expr->type_return);
        sprint("*)__t_base = ");
        emit_expression(expr);
        sprint(";\n");
        sprint("__t_base += %lld;\n", (expr->type_return->type_size_bits / 8) + offset_bytes);
    }
}

void C_Code_Generator::emit_array_assignment(Ast* decl) {
    Type_Instance* indexed_type = decl->decl_variable.variable_type;
    Ast* expr = decl->decl_variable.assignment;

    assert(indexed_type->kind == KIND_ARRAY);
    assert(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY);

    sprint("{\n");
    sprint("char* __t_base = (char*)%.*s;\n", TOKEN_STR(decl->decl_variable.name));
    sprint("char* __struct_base = __t_base;\n");
    emit_array_assignment_from_base(0, expr);
    sprint("}\n");
}

void C_Code_Generator::emit_array_assignment_to_temp(Ast* expr) {
	Type_Instance* indexed_type = expr->type_return;
	assert(indexed_type->kind == KIND_ARRAY);
	assert(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY);

	sprint("{\n");
	sprint("char* __t_base = (char*)__temp_v_%lld;\n", temp_variable);
	sprint("char* __struct_base = __t_base;\n");
	emit_array_assignment_from_base(0, expr);
	sprint("}\n");
}

void C_Code_Generator::emit_struct_assignment_to_temp(Ast* expr) {
	Type_Instance* struct_type = expr->type_return;

	assert(struct_type->kind == KIND_STRUCT);
	if (expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT) {
		sprint("{\n");

		sprint("char* __t_base = (char*)&(__temp_v_%lld);\n", temp_variable);

		// array copies will use this
		sprint("char* __array_base = __t_base;\n");
		emit_struct_assignment_from_base(0, expr);

		sprint("}\n");
	} else {
		sprint("%.*s = __temp_v_%lld", temp_variable);
		emit_expression(expr);
		sprint(";\n");
	}
}

void C_Code_Generator::emit_struct_assignment_from_base(s64 offset_bytes, Ast* expr) {
    // base is a char*
    if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT){
            size_t nexpr = 0;
            Type_Instance* stype = expr->type_return;
            if(expr->expr_literal.struct_exprs){
                nexpr = array_get_length(expr->expr_literal.struct_exprs);
                for(size_t i = 0; i < nexpr; ++i) {
                    s64 localoffset_bytes = stype->struct_desc.offset_bits[i] / 8;
                    emit_struct_assignment_from_base(offset_bytes + localoffset_bytes, expr->expr_literal.array_exprs[i]);
                }
            }
    } else if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_ARRAY) {
        sprint("{\n");
        sprint("char* __t_base = __array_base;\n");
        emit_array_assignment_from_base(offset_bytes, expr);
        sprint("}\n");
    } else {
        sprint("*(");
        emit_type(expr->type_return);
        sprint("*)((char*)__t_base + %lld) = ", offset_bytes);
        emit_expression(expr);
        sprint(";\n");
    }
}

void C_Code_Generator::emit_struct_assignment(Ast* decl) {
    Type_Instance* struct_type = decl->decl_variable.variable_type;
    Ast* expr = decl->decl_variable.assignment;

    assert(struct_type->kind == KIND_STRUCT);
    if(expr->node_type == AST_EXPRESSION_LITERAL && expr->expr_literal.type == LITERAL_STRUCT) {
        sprint("{\n");

        sprint("char* __t_base = (char*)&(%.*s);\n", TOKEN_STR(decl->decl_variable.name));

        // array copies will use this
        sprint("char* __array_base = __t_base;\n");
        emit_struct_assignment_from_base(0, expr);

        sprint("}\n");
    } else {
        sprint("%.*s = ", TOKEN_STR(decl->decl_variable.name));
        emit_expression(decl->decl_variable.assignment);
        sprint(";\n");
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
                        Ast* expr = cm->decl_variable.assignment;
                        if(!expr){
                            sprint(" = ", TOKEN_STR(cm->decl_variable.name));
                            emit_default_value(cm->decl_variable.variable_type);
                        }
                        sprint(";\n");
                        if(expr){
                            if(expr->type_return->kind == KIND_ARRAY){
                                emit_array_assignment(cm);
                            } else if(expr->type_return->kind == KIND_STRUCT) {
                                emit_struct_assignment(cm);
                            } else {
								deferring = true;
                                sprint("%.*s = ", TOKEN_STR(cm->decl_variable.name));
                                emit_expression(cm->decl_variable.assignment);
                                sprint(";");
								deferring = false;
								defer_flush();
                            }
                        }
					} else if (cm->node_type == AST_DECL_CONSTANT) {
						//deferring = true;
						//sprint("const ");
						//emit_decl(cm);
						//sprint(" = ");
						//emit_expression(cm->decl_constant.value);
						//sprint(";");
						//deferring = false;
						//defer_flush();
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

			deferring = true;
            sprint("while(");
            emit_expression(comm->comm_for.condition);
            sprint(")");
			deferring = false;
			defer_flush();

            emit_command(comm->comm_for.body);
			sprint("\nloop_%lld:;\n", id);
        }break;
		case AST_COMMAND_IF:{

			deferring = true;
            sprint("if(");
            emit_expression(comm->comm_if.condition);
            sprint(")");
			deferring = false;
			defer_flush();

            emit_command(comm->comm_if.body_true);
            if(comm->comm_if.body_false){
                sprint(" else ");
                emit_command(comm->comm_if.body_false);
            }
        }break;
		case AST_COMMAND_RETURN:{
			deferring = true;
            sprint("return ");
            if(comm->comm_return.expression){
                emit_expression(comm->comm_return.expression);
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
					emit_expression(comm->comm_var_assign.lvalue);
					sprint(" = ");
				}
				emit_expression(comm->comm_var_assign.rvalue);
				sprint(";");
				deferring = false;
				defer_flush();
			}
			// bigger than regsize
			else {
                if(rval->node_type == AST_EXPRESSION_VARIABLE && rval->type_return->kind == KIND_STRUCT){
                    if (comm->comm_var_assign.lvalue) {
                        emit_expression(comm->comm_var_assign.lvalue);
                        sprint(" = ");
                    }
                    emit_expression(comm->comm_var_assign.rvalue);
                    sprint(";");
                } else if(rval->node_type == AST_EXPRESSION_LITERAL && rval->expr_literal.type == LITERAL_STRUCT) {
                    sprint("{\n");
                    sprint("char* __t_base = (char*)&(");
                    emit_expression(comm->comm_var_assign.lvalue);
                    sprint(");\n");
                    emit_struct_assignment_from_base(0, rval);
                    sprint("}\n");
                } else if(rval->node_type == AST_EXPRESSION_LITERAL && rval->expr_literal.type == LITERAL_ARRAY) {
                    sprint("{\n");
                    sprint("char* __t_base = (char*)(");
                    emit_expression(comm->comm_var_assign.lvalue);
                    sprint(");\n");
                    emit_array_assignment_from_base(0, rval);
                    sprint("}\n");
                } else {
				    assert_msg(0, "assignment of type not recognized");
                }
			}
        } break;
	}
}

void C_Code_Generator::emit_temp_assignment(Ast* expr) {
	deferring = false;
	if (expr->type_return->kind == KIND_ARRAY) {
		sprint("char ");
	} else {
		emit_type(expr->type_return);
	}
	sprint(" __temp_v_%lld", temp_variable);
    if (expr->type_return->kind == KIND_ARRAY) {
        sprint("[%lld]", expr->type_return->type_size_bits / 8);
    }
    sprint(";\n");

	emit_expression(expr);
	deferring = true;
	sprint("__temp_v_%lld", temp_variable);
	temp_variable += 1;
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
			sprint("((");
            emit_expression(expr->expr_binary.left);
			sprint(")");
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
				// @TODO check if this causes a bug anywhere?
				// why is this here ?

				//if (indexed_type->pointer_to->kind != KIND_POINTER) {
					sprint("*");
				//}
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
			// TODO(psv): array bounds check
            emit_expression(expr->expr_binary.right);
			sprint("))");
        }break;
    }
}

void C_Code_Generator::emit_expression(Ast* expr){
    assert(expr->flags & AST_FLAG_IS_EXPRESSION || expr->node_type == AST_DATA);

    switch(expr->node_type){
        case AST_EXPRESSION_BINARY:{
            emit_expression_binary(expr);
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
				case LITERAL_ARRAY: {
					if (deferring) {
						emit_temp_assignment(expr);
					} else {
						emit_array_assignment_to_temp(expr);
					}
				}break;
				case LITERAL_STRUCT: {
					if (deferring) {
						emit_temp_assignment(expr);
					} else {
						emit_struct_assignment_to_temp(expr);
					}
				} break;
            }
        }break;
        case AST_EXPRESSION_PROCEDURE_CALL:{
            sprint("(");
            emit_expression(expr->expr_proc_call.caller);
            sprint(")(");
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
            Ast* decl = decl_from_name(expr->scope, expr->expr_variable.name);
            if(decl->node_type == AST_DECL_CONSTANT){
                emit_expression(decl->decl_constant.value);
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

void C_Code_Generator::emit_data_decl(Ast* decl) {
    if(decl->data_global.type == GLOBAL_STRING) {
        sprint("char* __string_data_%d = \"", decl->data_global.id);
        sprint("%.*s\";\n", decl->data_global.length_bytes, decl->data_global.data);
    } else {
        assert_msg(0, "trying to generate code for undefined data node");
    }
}

void C_Code_Generator::emit_proc(Ast* decl) {
	if (decl->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) return;

	emit_decl(decl);
	
	// body
	emit_command(decl->decl_procedure.body);
	sprint("\n\n");
}

void C_Code_Generator::emit_function_typedef(Type_Instance* type) {
	assert(type->kind == KIND_FUNCTION);
	assert(type->flags & TYPE_FLAG_INTERNALIZED);
	sprint("typedef ");

	Type_Instance* rettype = type->function_desc.return_type;
	if (rettype->kind == KIND_FUNCTION) {
		sprint("__func_type_%p*", rettype);
	} else {
		emit_type(rettype);
	}
	sprint(" __func_type_%p(", type);

	size_t nargs = type->function_desc.num_arguments;

	for (size_t i = 0; i < nargs; ++i) {
		if (i != 0) sprint(", ");
		emit_type(type->function_desc.arguments_type[i]);
	}

	sprint(");\n");
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

    // forward declarations of types
    Ast** type_decl_arr = type_decl_array_get();
    for(size_t i = 0; i < array_get_length(type_decl_arr); ++i){
        Ast* decl = type_decl_arr[i];
        if(decl->node_type == AST_DECL_STRUCT) {
            sprint("typedef struct %.*s %.*s;\n", TOKEN_STR(decl->decl_struct.name), TOKEN_STR(decl->decl_struct.name));
        }
    }

    // emit all declarations of types
    for (size_t i = 0; i < array_get_length(type_table); ++i) {
		Type_Instance* type = type_table[i];
		if (type->kind == KIND_FUNCTION) {
			emit_function_typedef(type);
		}
	}
    
    // emit structs and proc declarations
    for(size_t i = 0; i < array_get_length(type_decl_arr); ++i){
        Ast* decl = type_decl_arr[i];
        emit_decl(decl);
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
			emit_decl(decl, true);
            //if(decl->node_type == AST_DECL_CONSTANT){
            //    sprint(" = ");
            //    emit_expression(decl->decl_constant.value);
            //}
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
			if (decl->decl_variable.assignment) {
				// emit expression
                switch(decl->decl_variable.assignment->type_return->kind){
                    case KIND_ARRAY:{
                        emit_array_assignment(decl);
                    }break;
                    case KIND_STRUCT:{
                        emit_struct_assignment(decl);
                    }break;
                    case KIND_FUNCTION:
                    case KIND_POINTER:
                    case KIND_PRIMITIVE:{
						deferring = true;
			            sprint("\t%.*s = ", TOKEN_STR(decl->decl_variable.name));
				        emit_expression(decl->decl_variable.assignment);
						deferring = false;
						defer_flush();
                    }break;
                }
			} else {
                /*
                sprint("\t%.*s = ", TOKEN_STR(decl->decl_variable.name));
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
				}*/
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

    Timer timer;

    double start = timer.GetTime();
    int err = code_generator.c_generate_top_level(toplevel, type_table);
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
	sprintf(cmdbuffer, "gcc -c -g %.*s -o %.*s.obj", out_obj.length, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	int len = sprintf(cmdbuffer, "ld %.*s.obj -e__entry -nostdlib -o %.*s.exe -L%.*s..\\..\\lib",
		fname_len, out_obj.data, fname_len, out_obj.data, comp_path.length, comp_path.data);
#elif defined(__linux__)
    sprintf(cmdbuffer, "gcc -c %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
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