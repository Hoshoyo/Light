#include "type_infer.h"
#include "decl_check.h"
#include "ast.h"

Type_Error report_type_mismatch(Ast* node, Type_Instance* t1, Type_Instance* t2) {
	report_error_location(node);
	Type_Error err = report_type_error(TYPE_ERROR_FATAL, "type mismatch '");
	DEBUG_print_type(stderr, t1, true);
	fprintf(stderr, "' vs '");
	DEBUG_print_type(stderr, t2, true);
	fprintf(stderr, "'\n");
	return err;
}
Type_Error report_type_not_defined_for_binop(Type_Instance* left, Type_Instance* right, Ast* expr) {
	Type_Error error = TYPE_OK;
	error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operation '");
	DEBUG_print_type(stderr, left, true);
	fprintf(stderr, "' %s '", binop_op_to_string(expr->expr_binary.op));
	DEBUG_print_type(stderr, right, true);
	fprintf(stderr, "' is undefined\n");

	return error;
}

Type_Error type_update_weak(Ast* expr, Type_Instance* strong) { return 0; }

/* --------------------------------------------------------------
   -------------------- Type Inference --------------------------
   -------------------------------------------------------------- */

inline bool type_weak(Type_Instance* type) {
	return (type->flags & TYPE_FLAG_WEAK);
}
inline bool type_strong(Type_Instance* type) {
	return (type->flags & TYPE_FLAG_STRONG);
}

Type_Instance* infer_from_expression(Ast* expr, Type_Error* error, u32 flags) {
	// if it is raw data, get the type directly
	if(expr->node_type == AST_DATA) {
		assert(expr->data_global.data_type->flags & TYPE_FLAG_INTERNALIZED);
		return expr->data_global.data_type;
	}

	assert(expr->flags & AST_FLAG_IS_EXPRESSION);

	Type_Instance* type_infered = 0;

	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:				type_infered = infer_from_binary_expression(expr, error, flags); break;
		case AST_EXPRESSION_LITERAL:			type_infered = infer_from_literal_expression(expr, error, flags); break;
		case AST_EXPRESSION_PROCEDURE_CALL:		type_infered = infer_from_procedure_call(expr, error, flags); break;
		case AST_EXPRESSION_UNARY:				type_infered = infer_from_unary_expression(expr, error, flags); break;
		case AST_EXPRESSION_VARIABLE:			type_infered = infer_from_variable_expression(expr, error, flags); break;
	}
	return type_infered;
}

Type_Instance* infer_from_binary_expression(Ast* expr, Type_Error* error, u32 flags)  {
	assert(expr->node_type == AST_EXPRESSION_BINARY);

	expr->expr_binary.left->type_return = infer_from_expression(expr->expr_binary.left, error, flags);
	if(expr->expr_binary.op != OP_BINARY_DOT){
		expr->expr_binary.right->type_return = infer_from_expression(expr->expr_binary.right, error, flags);
		// @IMPORTANT
		// TODO(psv): more cases could be lvalue
		// arr[0] -> array dereference
		expr->flags |= AST_FLAG_LVALUE;
	}
	if(*error & TYPE_ERROR_FATAL) return 0;
	
	expr->type_return = type_check_expr(0, expr, error);

	return expr->type_return;
}

Type_Instance* infer_from_unary_expression(Ast* expr, Type_Error* error, u32 flags) {
	assert(expr->node_type == AST_EXPRESSION_UNARY);
	Type_Instance* infered = infer_from_expression(expr->expr_unary.operand, error, flags);
	if (!infered) return 0;
	
	switch (expr->expr_unary.op) {
		// @INFER ADDRESSOF
		case OP_UNARY_ADDRESSOF:{
			if (expr->expr_unary.operand->flags & AST_FLAG_LVALUE) {
				// that means right side is strong because it is lvalue
				assert(type_strong(infered));
				Type_Instance* ptrtype = type_new_temporary();
				ptrtype->kind = KIND_POINTER;
				ptrtype->type_size_bits = type_pointer_size_bits();
				ptrtype->flags = TYPE_FLAG_SIZE_RESOLVED;
				ptrtype->pointer_to = infered;
				expr->type_return = internalize_type(&ptrtype, expr->scope, true);
				return expr->type_return;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary address of operator requires lvalue\n");
			}
		}break;

		// @INFER DEREFERENCE
		case OP_UNARY_DEREFERENCE: {
			if (infered->kind == KIND_POINTER || infered->kind == KIND_ARRAY) {
				assert(type_strong(infered));
				expr->flags |= AST_FLAG_LVALUE;
				expr->type_return = infered->pointer_to;
				return infered->pointer_to;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "operator dereference requires pointer or array type\n");
			}
		}break;

		// @INFER BITWISE NOT
		case OP_UNARY_BITWISE_NOT: {
			if (type_primitive_int(infered)) {
				expr->type_return = infered;
				return infered;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operator '~' requires integer type\n");
			}
		}break;

		// @INFER CAST
		case OP_UNARY_CAST: {
			Type_Instance* cast_to = resolve_type(expr->scope, expr->expr_unary.type_to_cast, true);
			expr->expr_unary.type_to_cast = cast_to;
			if(!cast_to) {
				*error |= TYPE_ERROR_FATAL;
				return 0;
			}
			if ((type_primitive_numeric(infered) || infered->kind == KIND_POINTER)) {
				// @TODO: check more types here and also check cast_to, cannot be any type.
				type_propagate(0, expr->expr_unary.operand);
				assert(type_strong(cast_to));
				expr->type_return = cast_to;
				return cast_to;
			} else if (infered->kind == KIND_ARRAY && cast_to->kind == KIND_POINTER) {
				assert(type_strong(infered) && type_strong(cast_to));
				expr->type_return = cast_to;
				return cast_to;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "cannot cast from type '");
				DEBUG_print_type(stderr, infered, true);
				fprintf(stderr, "' to '");
				DEBUG_print_type(stderr, cast_to, true);
				fprintf(stderr, "\n");
			}
		}break;

		// @INFER LOGIC NOT
		case OP_UNARY_LOGIC_NOT: {
			if (type_primitive_bool(infered)) {
				expr->type_return = infered;
				return infered;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operator '!' requires boolean type\n");
			}
		} break;

		// @INFER MINUS
		case OP_UNARY_MINUS: {
			if (type_primitive_numeric(infered)) {
				expr->type_return = infered;
				return infered;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operator '-' requires numeric type\n");
			}
		}break;

		// @INFER PLUS
		case OP_UNARY_PLUS: {
			if (type_primitive_numeric(infered)) {
				expr->type_return = infered;
				return infered;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operator '+' requires numeric type\n");
			}
		}break;
	}

	return 0;
}

Type_Instance* infer_from_literal_expression(Ast* expr, Type_Error* error, u32 flags) {
	Type_Instance* result = type_new_temporary();
	result->flags = TYPE_FLAG_WEAK;

	switch (expr->expr_literal.type) {
		case LITERAL_UINT:{
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_U32;
		} break;
		case LITERAL_BIN_INT:
		case LITERAL_HEX_INT: {
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_U64;
		}break;
		case LITERAL_SINT: {
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_S64;
		}break;
		case LITERAL_BOOL: {
			result = type_primitive_get(TYPE_PRIMITIVE_BOOL);
		}break;
		case LITERAL_FLOAT: {
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_R32;
		}break;
		case LITERAL_ARRAY: {
			result->kind = KIND_ARRAY;

			size_t nexpr = 0;
			if(expr->expr_literal.array_exprs){
				nexpr = array_get_length(expr->expr_literal.array_exprs);
			}

			for(size_t i = 0; i < nexpr; ++i){
				Type_Instance* type = infer_from_expression(expr->expr_literal.array_exprs[i], error, flags);
				expr->expr_literal.array_exprs[i]->type_return = type;
				if(type_strong(type)){
					expr->expr_literal.array_strong_type = type;
				}
			}
			
			result->array_desc.dimension_evaluated = true;
			result->array_desc.dimension = (u64)nexpr;
			
			if(expr->expr_literal.array_strong_type){
				for(size_t i = 0; i < nexpr; ++i){
					type_propagate(expr->expr_literal.array_strong_type, expr->expr_literal.array_exprs[i]);
				}
				result->array_desc.array_of = expr->expr_literal.array_strong_type;
				result->flags |= TYPE_FLAG_SIZE_RESOLVED;
				result->type_size_bits = nexpr * expr->expr_literal.array_strong_type->type_size_bits;
				internalize_type(&result->array_desc.array_of, expr->scope, true);
			} else if(expr->expr_literal.array_exprs) {
				result->array_desc.array_of = expr->expr_literal.array_exprs[0]->type_return;
			} else {
				result->array_desc.array_of = 0;
			}

		}break;
		case LITERAL_STRUCT:{
			result->kind = KIND_STRUCT;
			result->struct_desc.name = expr->expr_literal.token;
			Ast* struct_decl = decl_from_name(expr->scope, result->struct_desc.name);
			if(!struct_decl){
				*error |= TYPE_QUEUED;
				return 0;
			}

			size_t nexpr = 0;
			if(expr->expr_literal.struct_exprs){
				nexpr = array_get_length(expr->expr_literal.struct_exprs);
				result->struct_desc.fields_types = array_create(Type_Instance*, nexpr);
			}

			for(size_t i = 0; i < nexpr; ++i){
				Type_Instance* type = infer_from_expression(expr->expr_literal.struct_exprs[i], error, flags);
				expr->expr_literal.struct_exprs[i]->type_return = type;
				array_push(result->struct_desc.fields_types, &type);
			}
			expr->type_return = result;
			type_propagate(0, expr);
			if(!struct_decl || !expr->type_return){
				*error |= TYPE_ERROR_FATAL;
				return 0;
			}
			result = type_check_expr(struct_decl->decl_struct.type_info, expr, error);
		}break;
		default: {
			assert_msg(0, "tried to infer type of undefined literal");
		}break;
	}
	expr->type_return = result;
	return result;
}

Type_Instance* infer_from_procedure_call(Ast* expr, Type_Error* error, u32 flags) {
	Type_Instance* proc_type = 0;
	
	Type_Instance* caller_type = infer_from_expression(expr->expr_proc_call.caller, error, flags);
	if(!caller_type){
		assert(*error != 0);
		return 0;
	}

	if(caller_type->kind != KIND_FUNCTION){
		*error |= report_type_error(TYPE_ERROR_FATAL, expr, "expression is not a procedure\n");
	} else {
		assert_msg(type_strong(caller_type), "weak functional type in type inference pass");
	}

	proc_type = caller_type;
	assert(proc_type->flags & TYPE_FLAG_INTERNALIZED);

	size_t nargs = expr->expr_proc_call.args_count;
	if(nargs < proc_type->function_desc.num_arguments) {
		*error |= report_type_error(TYPE_ERROR_FATAL, expr, "not enough arguments for procedure call, expected '%d' got '%d'\n", 
			proc_type->function_desc.num_arguments, nargs);
		return 0;
	} else if(nargs > proc_type->function_desc.num_arguments) {
		*error |= report_type_error(TYPE_ERROR_FATAL, expr, "too many arguments for procedure call, expected '%d' got '%d'\n",
			proc_type->function_desc.num_arguments, nargs);
		return 0;
	}

	for (size_t i = 0; i < nargs; ++i) {
		Type_Instance* type = infer_from_expression(expr->expr_proc_call.args[i], error, true);
		if (*error & TYPE_ERROR_FATAL) continue;
		expr->expr_proc_call.args[i]->type_return = type;
		
		if(type_weak(type)){
			type_propagate(proc_type->function_desc.arguments_type[i], expr->expr_proc_call.args[i]);
		}
		expr->expr_proc_call.args[i]->type_return = type_check_expr(proc_type->function_desc.arguments_type[i], expr->expr_proc_call.args[i], error);
	}

	expr->type_return = proc_type->function_desc.return_type;
	return expr->type_return;
}

Type_Instance* infer_from_variable_expression(Ast* expr, Type_Error* error, u32 flags) {
	assert(expr->node_type == AST_EXPRESSION_VARIABLE);

	Ast* decl = decl_from_name(expr->scope, expr->expr_variable.name);
	
	if (!decl) {
		if(flags & TYPE_INFER_REPORT_UNDECLARED) {
			*error |= TYPE_ERROR_FATAL;
			*error |= report_undeclared(expr->expr_variable.name);
		} else {
			*error |= TYPE_QUEUED;
		}
		return 0;
	}
	
	if(decl->type_return->kind == KIND_STRUCT){
		*error |= report_type_error(TYPE_ERROR_FATAL, expr, "cannot use structure name '%.*s' as expression\n", 
			TOKEN_STR(expr->expr_variable.name));
		return 0;
	}
	expr->expr_variable.decl = decl;
	Type_Instance* type = 0;

	if (decl->node_type != AST_DECL_VARIABLE && decl->node_type != AST_DECL_CONSTANT) {
		assert_msg(decl->node_type == AST_DECL_PROCEDURE, "invalid variable node");
		type = decl->decl_procedure.type_procedure;

		if(!type) {
			*error |= TYPE_ERROR_FATAL;
			return 0;
		}
		expr->type_return = type;
	} else if(decl->node_type == AST_DECL_VARIABLE){
		type = decl->decl_variable.variable_type;

		// type of variable tried to resolve already, but could not
		if(!type) {
			*error |= TYPE_ERROR_FATAL;
			return 0;
		}

		assert(type_strong(type));
		expr->flags |= AST_FLAG_LVALUE;
		expr->type_return = type;
	} else if(decl->node_type == AST_DECL_CONSTANT) {
		type = decl->decl_constant.type_info;
		assert(type_strong(type));
		expr->type_return = type;
	}
	return type;
}

Type_Error type_propagate(Type_Instance* strong, Ast* expr) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION || expr->node_type == AST_DATA);

	Type_Error error_code = TYPE_OK;

	if(!expr->type_return) {
		// @TODO see what to do here
		// @IMPORTANT
		// This type can be null?
		return error_code;
	}
	if (expr->type_return->flags & TYPE_FLAG_STRONG)
		return error_code;

	switch(expr->node_type){
		// @PROPAGATE LITERAL
		case AST_EXPRESSION_LITERAL: {
			if (strong) {
				Type_Error error = TYPE_OK;
				if(type_primitive_int(strong) && type_primitive_int(expr->type_return)) {
					expr->type_return = strong;
				} else if(type_primitive_float(strong) && type_primitive_float(expr->type_return)){
					expr->type_return = strong;
				} else if(type_primitive_bool(strong) && type_primitive_bool(expr->type_return)){
					assert(expr->type_return == strong);
					expr->type_return = strong;
				} else if(strong->kind == KIND_ARRAY && expr->expr_literal.type == LITERAL_ARRAY){
					size_t nexpr = 0;
					if(expr->expr_literal.array_exprs){
						nexpr = array_get_length(expr->expr_literal.array_exprs);
					}
					for(size_t i = 0; i < nexpr; ++i){
						type_propagate(strong->array_desc.array_of, expr->expr_literal.array_exprs[i]);
					}
					expr->type_return->array_desc.array_of = strong->array_desc.array_of;
					internalize_type(&expr->type_return, expr->scope, true);
				} else if(strong->kind == KIND_STRUCT && expr->expr_literal.type == LITERAL_STRUCT) {
					size_t nexpr = 0;
					if(expr->expr_literal.struct_exprs) {
						nexpr = array_get_length(expr->expr_literal.struct_exprs);
					}
					size_t strong_dim = 0;
					if(strong->struct_desc.fields_types){
						strong_dim = array_get_length(strong->struct_desc.fields_types);
					}
					for(size_t i = 0; i < MIN(nexpr,strong_dim); ++i){
						type_propagate(strong->struct_desc.fields_types[i], expr->expr_literal.struct_exprs[i]);
						expr->type_return->struct_desc.fields_types[i] = expr->expr_literal.struct_exprs[i]->type_return;
					}
					expr->type_return->flags |= TYPE_FLAG_INTERNALIZED;
					internalize_type(&expr->type_return, expr->scope, true);
				}
			} else {
				if(expr->expr_literal.type == LITERAL_ARRAY){
					size_t nexpr = array_get_length(expr->expr_literal.array_exprs);
					for(size_t i = 0; i < nexpr; ++i){
						type_propagate(0, expr->expr_literal.array_exprs[i]);
					}
					expr->type_return->array_desc.array_of = expr->expr_literal.array_exprs[0]->type_return;
					expr->type_return = resolve_type(expr->scope, expr->type_return, false);
				} else if(expr->expr_literal.type == LITERAL_STRUCT) {
					size_t nexpr = 0;
					if(expr->expr_literal.struct_exprs) {
						nexpr = array_get_length(expr->expr_literal.struct_exprs);
					}
					strong = expr->type_return = resolve_type(expr->scope, expr->type_return, true);
					if(!strong) {
						return error_code;
					}
					size_t strong_dim = 0;
					if(strong->kind == KIND_STRUCT) {
						strong_dim = strong->struct_desc.fields_count;
					}

					for(size_t i = 0; i < MIN(nexpr, strong_dim); ++i){
						type_propagate(strong->struct_desc.fields_types[i], expr->expr_literal.struct_exprs[i]);
						expr->type_return->struct_desc.fields_types[i] = expr->expr_literal.struct_exprs[i]->type_return;
					}
				}else {
					expr->type_return = resolve_type(expr->scope, expr->type_return, false);
					expr->type_return = internalize_type(&expr->type_return, expr->scope, true);
				}
			}
		}break;

		// @PROPAGATE BINARY
		case AST_EXPRESSION_BINARY:{
			switch (expr->expr_binary.op) {
				case OP_BINARY_PLUS:
				case OP_BINARY_MINUS: {
					// pointer can never be weak
					assert(expr->expr_binary.left->type_return->kind != KIND_POINTER);
				} //fallthrough
				case OP_BINARY_AND:
				case OP_BINARY_OR:
				case OP_BINARY_XOR:
				case OP_BINARY_MOD:
				case OP_BINARY_MULT:
				case OP_BINARY_DIV:
				case OP_BINARY_LOGIC_AND:
				case OP_BINARY_LOGIC_OR:
				case OP_BINARY_SHL:
				case OP_BINARY_SHR: {
					type_propagate(strong, expr->expr_binary.left);
					type_propagate(strong, expr->expr_binary.right);
					expr->type_return = expr->expr_binary.left->type_return;
				}break;

				case OP_BINARY_EQUAL:
				case OP_BINARY_NOT_EQUAL:
				case OP_BINARY_GE:
				case OP_BINARY_GT:
				case OP_BINARY_LE:
				case OP_BINARY_LT: {
					type_propagate(0, expr->expr_binary.left);
					type_propagate(0, expr->expr_binary.right);
					expr->type_return = type_primitive_get(TYPE_PRIMITIVE_BOOL);
				}

				case OP_BINARY_DOT:
					//assert(0);
					assert(type_strong(expr->expr_binary.left->type_return));
					assert(type_strong(expr->expr_binary.right->type_return));
					break;
				case OP_BINARY_VECTOR_ACCESS:
					//assert(type_strong(expr->expr_binary.left->type_return));
					if(!type_strong(expr->expr_binary.left->type_return)) {
						// type is an array literal
						assert(expr->expr_binary.left->node_type == AST_EXPRESSION_LITERAL && expr->expr_binary.left->expr_literal.type == LITERAL_ARRAY);
						type_propagate(strong, expr->expr_binary.left);
						expr->type_return = resolve_type(expr->scope, expr->type_return, true);
					}

					// index should be strong aswell as indexed
					assert(type_strong(expr->expr_binary.right->type_return));
					break;

			}
		}break;

		// @PROPAGATE UNARY
		case AST_EXPRESSION_UNARY: {
			switch (expr->expr_unary.op) {
				case OP_UNARY_ADDRESSOF:
				case OP_UNARY_DEREFERENCE:
				case OP_UNARY_CAST:
					// cast, addressof and dereference should already be strong
					assert(type_strong(expr->type_return));
					break;

				case OP_UNARY_BITWISE_NOT:
				case OP_UNARY_LOGIC_NOT:
				case OP_UNARY_MINUS:
				case OP_UNARY_PLUS: {
					type_propagate(strong, expr->expr_unary.operand);
					expr->type_return = expr->expr_unary.operand->type_return;
				}break;
			}
		}break;

		case AST_EXPRESSION_VARIABLE:
		case AST_EXPRESSION_PROCEDURE_CALL: {
			// should be strong already
			assert(type_strong(expr->type_return));
		}break;
	}

	return error_code;
}

static Type_Instance* defer_check_against(Ast* expr, Type_Instance* check_against, Type_Instance* type, Type_Error* error) {
	if (!check_against)
		return type;
	if (check_against == type) {
		return type;
	} else {
		*error |= report_type_mismatch(expr, check_against, type);
	}
	return type;
}

Type_Instance* type_check_expr(Type_Instance* check_against, Ast* expr, Type_Error* error){
	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:{
			Type_Instance* lt = expr->expr_binary.left->type_return;
			Type_Instance* rt = expr->expr_binary.right->type_return;
			Operator_Binary binop = expr->expr_binary.op;
			switch (binop) {
				// @TYPECHECK PLUS
				case OP_BINARY_PLUS:{
					if (type_strong(lt) && type_strong(rt)) {
						// pointer arithmetic ^T(strong) + INT(strong) |-> ^T(strong)
						if (lt->kind == KIND_POINTER && type_primitive_int(rt)) {
							return defer_check_against(expr, check_against, lt, error);
						} 
						// NUMTYPE(strong) + NUMTYPE(strong) |-> NUMTYPE(strong)
						else if (type_primitive_numeric(lt) && type_primitive_numeric(rt)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, lt, error);
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '+' requires numeric type\n");
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// pointer arithmetic ^T(strong) + INT(weak) |-> ^T(strong) , propagate(INT(weak))
						if (lt->kind == KIND_POINTER && type_primitive_int(rt)) {
							rt = resolve_type(expr->scope, rt, false);
							assert(rt->flags & TYPE_FLAG_INTERNALIZED);
							type_propagate(rt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);
						} 
						// INT(strong) + INT(weak) |-> INT(strong) , propagete(INT(strong) -> INT(weak))
						// or
						// FLOAT(strong) + FLOAT(weak) |-> FLOAT(strong) , propagate(FLOAT(strong) -> FLOAT(weak))
						else if((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))){
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_strong(rt)) {
						// INT(weak) + INT(strong) |-> INT(strong) , propagate(INT(strong) -> INT(weak))
						// or
						// FLOAT(weak) + FLOAT(strong) |-> FLOAT(strong) , propagate(FLOAT(strong) -> FLOAT(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, lt, error);
						} else if (type_primitive_float(lt) && type_primitive_float(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				}break;

				// @TYPECHECK MINUS
				case OP_BINARY_MINUS:{
					if (type_strong(lt) && type_strong(rt)) {
						if (lt->kind == KIND_POINTER && rt->kind == KIND_POINTER) {
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							} else {
								return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_S64), error);
							}
						}
						// pointer arithmetic ^T(strong) - INT(strong) |-> ^T(strong)
						else if (lt->kind == KIND_POINTER && type_primitive_int(rt)) {
							return defer_check_against(expr, check_against, lt, error);
						}
						// NUMTYPE(strong) - NUMTYPE(strong) |-> NUMTYPE(strong)
						else if (type_primitive_numeric(lt) && type_primitive_numeric(rt)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, lt, error);
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '-' requires numeric type\n");
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// pointer arithmetic ^T(strong) - INT(weak) |-> ^T(strong) , propagate(INT(weak))
						if (lt->kind == KIND_POINTER && type_primitive_int(rt)) {
							rt = resolve_type(expr->scope, rt, false);
							assert(rt->flags & TYPE_FLAG_INTERNALIZED);
							type_propagate(rt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);;
						}
						// INT(strong) - INT(weak) |-> INT(strong) , propagete(INT(strong) -> INT(weak))
						// or
						// FLOAT(strong) - FLOAT(weak) |-> FLOAT(strong) , propagate(FLOAT(strong) -> FLOAT(weak))
						else if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_strong(rt)) {
						// INT(weak) - INT(strong) |-> INT(strong) , propagate(INT(strong) -> INT(weak))
						// or
						// FLOAT(weak) - FLOAT(strong) |-> FLOAT(strong) , propagate(FLOAT(strong) -> FLOAT(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, lt, error);
						} else if (type_primitive_float(lt) && type_primitive_float(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, lt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				} break;

				// @TYPECHECK MULT
				// @TYPECHECK DIV
				case OP_BINARY_DIV:
				case OP_BINARY_MULT: {
					if (type_strong(lt) && type_strong(rt)) {
						// NUMTYPE(strong) */ NUMTYPE(strong) |-> NUMTYPE(strong)
						if (type_primitive_numeric(lt) && type_primitive_numeric(rt)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, lt, error);
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires numeric type\n", binop_op_to_string(binop));
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// INT(strong) */ INT(weak) |-> INT(strong) , propagete(INT(strong) -> INT(weak))
						// or
						// FLOAT(strong) */ FLOAT(weak) |-> FLOAT(strong) , propagate(FLOAT(strong) -> FLOAT(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_strong(rt)) {
						// INT(weak) */ INT(strong) |-> INT(strong) , propagate(INT(strong) -> INT(weak))
						// or
						// FLOAT(weak) */ FLOAT(strong) |-> FLOAT(strong) , propagate(FLOAT(strong) -> FLOAT(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, lt, error);
						} else if (type_primitive_float(lt) && type_primitive_float(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				}break;

				// @TYPECHECK MOD
				case OP_BINARY_SHL:
				case OP_BINARY_SHR:
				case OP_BINARY_MOD: {
					if (type_strong(lt) && type_strong(rt)) {
						// INT(strong) % INT(strong) |-> INT(strong)
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, lt, error);
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires integer type\n", binop_op_to_string(binop));
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// INT(strong) % INT(weak) |-> INT(strong) , propagate(INT(strong) -> INT(weak))
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_strong(rt)) {
						// INT(weak) % INT(strong) |-> INT(strong) , propagate(INT(strong) -> INT(weak))
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							// TODO: can types here be different?
							return lt;
							//return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				}break;

				// @TYPECHECK AND
				// @TYPECHECK OR
				// @TYPECHECK XOR
				case OP_BINARY_AND:
				case OP_BINARY_OR:
				case OP_BINARY_XOR: {
					if (type_strong(lt) && type_strong(rt)) {
						// INT(strong) &|^ INT(strong) |-> INT(strong)
						// or
						// BOOL(strong) &|^ BOOL(strong) |-> BOOL(strong)
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || type_primitive_bool(lt) && type_primitive_bool(rt)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, lt, error);
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires numeric or boolean type\n", binop_op_to_string(binop));
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// INT(strong) &|^ INT(weak) |-> INT(strong) , propagete(INT(strong) -> INT(weak))
						// or
						// BOOL(strong) &|^ BOOL(weak) |-> BOOL(strong) , propagate(BOOL(strong) -> BOOL(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_bool(lt) && type_primitive_bool(rt))) {
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, lt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_strong(rt)) {
						// INT(weak) &|^ INT(strong) |-> INT(strong) , propagate(INT(strong) -> INT(weak))
						// or
						// BOOL(weak) &|^ BOOL(strong) |-> BOOL(strong) , propagate(BOOL(strong) -> BOOL(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						if (type_primitive_int(lt) && type_primitive_int(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, lt, error);
						} else if (type_primitive_bool(lt) && type_primitive_bool(rt)) {
							// TODO: can types here be different?
							return defer_check_against(expr, check_against, rt, error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				}break;
			
				// @TYPECHECK LOGIC_AND
				// @TYPECHECK LOGIC_OR
				case OP_BINARY_LOGIC_AND:
				case OP_BINARY_LOGIC_OR: {
					Type_Instance* lt = expr->expr_binary.left->type_return;
					Type_Instance* rt = expr->expr_binary.right->type_return;
					expr->type_return = type_primitive_get(TYPE_PRIMITIVE_BOOL);

					if (!(type_primitive_bool(lt) && type_primitive_bool(rt))) {
						*error |= report_type_not_defined_for_binop(lt, rt, expr);
					} else if (rt != lt) {
						*error |= report_type_mismatch(expr, lt, rt);
					}
					return defer_check_against(expr, check_against, expr->type_return, error);
				}break;

				// @TYPECHECK EQUAL
				// @TYPECHECK NOT_EQUAL
				case OP_BINARY_EQUAL:
				case OP_BINARY_NOT_EQUAL: {
					if (type_strong(lt) && type_strong(rt)) {
						// NUMTYPE(strong) == != NUMTYPE(strong) |-> BOOL(strong)
						// or
						// BOOL(strong) == != BOOL(strong) |-> BOOL(strong)
						// or
						// ^T(strong) == != ^T(strong) |-> BOOL(strong)
						if ((type_primitive_numeric(lt) && type_primitive_numeric(rt)) || type_primitive_bool(lt) && type_primitive_bool(rt) ||
							(lt->kind == KIND_POINTER && rt->kind == KIND_POINTER)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							// TODO: other comparisons
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' is not defined for this type\n", binop_op_to_string(binop));
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// pointer and bool types should not be weak
						assert(rt->kind != KIND_POINTER);
						// NUMTYPE(strong) == != NUMTYPE(weak) |-> BOOL(strong) , propagete(NUMTYPE(strong) -> NUMTYPE(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
					else if (type_weak(lt) && type_strong(rt)) {
						// pointer and bool types should not be weak
						assert(lt->kind != KIND_POINTER);
						// NUMTYPE(weak) == != NUMTYPE(strong) |-> BOOL(strong) , propagate(NUMTYPE(strong) -> NUMTYPE(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						// NUMTYP(weak) == != NUMTYPE(weak) |-> BOOL(strong) , 2 * propagate(0 -> NUMTYPE(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							// TODO: can types here be different?
							type_propagate(0, expr->expr_binary.left);
							type_propagate(0, expr->expr_binary.right);
							// this assumes the default is equal for both
							assert(expr->expr_binary.left->type_return == expr->expr_binary.right->type_return);
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else if (type_primitive_bool(lt) && type_primitive_bool(rt)) {
							// TODO: can types here be different?
							assert(lt == rt && type_strong(lt));
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				}break;

				// @TYPECHECK GE
				// @TYPECHECK GT
				// @TYPECHECK LE
				// @TYPECHECK LT
				case OP_BINARY_GE:
				case OP_BINARY_GT:
				case OP_BINARY_LE:
				case OP_BINARY_LT: {
					if (type_strong(lt) && type_strong(rt)) {
						// NUMTYPE(strong) > NUMTYPE(strong) |-> BOOL(strong)
						// or
						// ^T(strong) > ^T(strong) |-> BOOL(strong)
						if ((type_primitive_numeric(lt) && type_primitive_numeric(rt)) || (lt->kind == KIND_POINTER && rt->kind == KIND_POINTER)) {
							// TODO: type coercions
							if (lt != rt) {
								*error |= report_type_mismatch(expr, lt, rt);
							}
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							// TODO: other comparisons
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires numeric or pointer types\n", binop_op_to_string(binop));
						}
					} else if (type_strong(lt) && type_weak(rt)) {
						// pointer types should not be weak
						assert(rt->kind != KIND_POINTER);
						// NUMTYPE(strong) > NUMTYPE(weak) |-> BOOL(strong) , propagete(NUMTYPE(strong) -> NUMTYPE(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(lt, expr->expr_binary.right);
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_strong(rt)) {
						// pointer types should not be weak
						assert(lt->kind != KIND_POINTER);
						// NUMTYPE(weak) > NUMTYPE(strong) |-> BOOL(strong) , propagate(NUMTYPE(strong) -> NUMTYPE(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							type_propagate(rt, expr->expr_binary.left);
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					} else if (type_weak(lt) && type_weak(rt)) {
						// pointer types should not be weak
						assert(lt->kind != KIND_POINTER && rt->kind != KIND_POINTER);
						// NUMTYP(weak) > NUMTYPE(weak) |-> BOOL(strong) , 2 * propagate(0 -> NUMTYPE(weak))
						if ((type_primitive_int(lt) && type_primitive_int(rt)) || (type_primitive_float(lt) && type_primitive_float(rt))) {
							// TODO: can types here be different?
							type_propagate(0, expr->expr_binary.left);
							type_propagate(0, expr->expr_binary.right);
							// this assumes the default is equal for both
							assert(expr->expr_binary.left->type_return == expr->expr_binary.right->type_return);
							return defer_check_against(expr, check_against, type_primitive_get(TYPE_PRIMITIVE_BOOL), error);
						} else {
							if (type_hash(lt) != type_hash(rt))
								*error |= report_type_mismatch(expr, lt, rt);
							else
								*error |= report_type_not_defined_for_binop(lt, rt, expr);
						}
					}
				}break;

				case OP_BINARY_VECTOR_ACCESS: {
					Type_Instance* index_type = expr->expr_binary.right->type_return;
					Type_Instance* indexed_type = expr->expr_binary.left->type_return;
					if (type_primitive_int(index_type)) {
						if (type_weak(index_type)) {
							type_propagate(0, expr->expr_binary.right);
						}
						// operand should be an addressable pointer or array
						if (indexed_type->kind == KIND_POINTER) {
							assert(type_strong(indexed_type));
							return defer_check_against(expr, check_against, indexed_type->pointer_to, error);
						} else if (indexed_type->kind == KIND_ARRAY) {

							// @TODO(psv):
							// @IMPORTANT
							// this can be weak when an array literal is accessed
							//assert(type_strong(indexed_type));
							return defer_check_against(expr, check_against, indexed_type->array_desc.array_of, error);
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "vector accessing operator requires addressable value\n");
						}
					} else {
						*error |= report_type_error(TYPE_ERROR_FATAL, expr, "vector accessing operator requires integer index\n");
					}
				} break;
				case OP_BINARY_DOT:
					if(lt->kind != KIND_STRUCT && lt->kind != KIND_UNION){
						// @IMPLEMENT namespaces
						*error |= report_type_error(TYPE_ERROR_FATAL, expr, "cannot access field from a non structure type\n");
						return 0;
					}
					if(expr->expr_binary.right->node_type != AST_EXPRESSION_VARIABLE){
						*error |= report_type_error(TYPE_ERROR_FATAL, expr, "cannot access field of structure, name declaration required\n");
						return 0;
					}
					Ast* decl_struct = decl_from_name(expr->scope, lt->struct_desc.name);
					assert_msg(decl_struct, "how can we not get a struct that has a type defined?");
					
					Ast* decl = decl_from_name(decl_struct->decl_struct.struct_scope, expr->expr_binary.right->expr_variable.name);
					if(!decl){
						*error |= report_type_error(TYPE_ERROR_FATAL, expr, "'%.*s' is not a field of struct '%.*s'\n", 
							TOKEN_STR(expr->expr_binary.right->expr_variable.name), TOKEN_STR(decl_struct->decl_struct.name));
						return 0;
					} else {
						expr->expr_binary.right->scope = decl_struct->decl_struct.struct_scope;
					}
					assert(type_strong(decl->decl_variable.variable_type));

					expr->flags |= AST_FLAG_LVALUE;
					expr->type_return = decl->decl_variable.variable_type;
					expr->expr_binary.right->type_return = expr->type_return;
					return defer_check_against(expr, check_against, expr->type_return, error);
					break;
			}
		} break;
		case AST_EXPRESSION_UNARY: {
			switch(expr->expr_unary.op){
				case OP_UNARY_CAST:{
					assert(type_strong(expr->expr_unary.operand->type_return));
				}
				case OP_UNARY_BITWISE_NOT:
				case OP_UNARY_LOGIC_NOT:
				case OP_UNARY_DEREFERENCE:
				case OP_UNARY_MINUS:
				case OP_UNARY_PLUS:
				case OP_UNARY_ADDRESSOF:{
					assert(type_strong(expr->expr_unary.operand->type_return));
					return defer_check_against(expr, check_against, expr->type_return, error);
				}break;
				default: assert_msg(0, "internal error invalid unary expression"); break;
			}
		}break;
		case AST_EXPRESSION_LITERAL: {
			if(expr->expr_literal.type == LITERAL_ARRAY && check_against->kind == KIND_ARRAY) {
				if(expr->expr_literal.array_exprs){
					size_t check_against_dim = check_against->array_desc.dimension;
					size_t nexpr = array_get_length(expr->expr_literal.array_exprs);
					if(nexpr == check_against_dim){
						for(size_t i = 0; i < nexpr; ++i){
							Type_Instance* type = type_check_expr(check_against->array_desc.array_of, expr->expr_literal.array_exprs[i], error);
							if(*error) return 0;
						}
					}
				}
				expr->type_return = resolve_type(expr->scope, expr->type_return, false);
			} else if(expr->expr_literal.type == LITERAL_STRUCT && check_against->kind == KIND_STRUCT) {
				// @TODO set check_against here to 0
				// @BUG
				// using the struct name as variable i think this crashes, maybe?

				// Could be a union, if so, accept only 1 field
				if (check_against->flags & TYPE_FLAG_UNION) {
					if (expr->expr_literal.struct_exprs) {
						size_t nexpr = array_get_length(expr->expr_literal.struct_exprs);
						if (nexpr == 1) {
							
						} else {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "union literal can only have 1 value\n");
							return 0;
						}
					}
				} else {
					if (expr->expr_literal.struct_exprs) {
						size_t check_against_num_fields = 0;
						if (check_against->struct_desc.fields_types) {
							check_against_num_fields = array_get_length(check_against->struct_desc.fields_types);
						}
						size_t nexpr = array_get_length(expr->expr_literal.struct_exprs);
						if (nexpr == check_against_num_fields) {
							for (size_t i = 0; i < nexpr; ++i) {
								Ast* e = expr->expr_literal.struct_exprs[i];
								if (type_weak(e->type_return)) {
									type_propagate(0, e);
								}
								Type_Instance* type = type_check_expr(check_against->struct_desc.fields_types[i], e, error);
								if (*error) return 0;
							}
						} else if (nexpr > check_against_num_fields) {
							*error |= report_type_error(TYPE_ERROR_FATAL, expr, "too many fields for %.*s struct literal\n", TOKEN_STR(expr->expr_literal.token));
							return 0;
						} else if (nexpr < check_against_num_fields) {
							size_t nmissing = check_against_num_fields - nexpr;
							if (nmissing == 1) {
								*error |= report_type_error(TYPE_ERROR_FATAL, expr, "missing '%d' field in %.*s struct literal\n",
									nmissing, TOKEN_STR(expr->expr_literal.token));
							} else {
								*error |= report_type_error(TYPE_ERROR_FATAL, expr, "missing '%d' fields in %.*s struct literal\n",
									nmissing, TOKEN_STR(expr->expr_literal.token));
							}
							return 0;
						}
					}
				}
			} else {
				type_propagate(check_against, expr);
			}
			return defer_check_against(expr, check_against, expr->type_return, error);
		}break;
		case AST_EXPRESSION_PROCEDURE_CALL:
		case AST_EXPRESSION_VARIABLE:{
			assert(type_strong(expr->type_return));
			return defer_check_against(expr, check_against, expr->type_return, error);
		}
	}
	return 0;
}