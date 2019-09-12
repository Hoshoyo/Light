#include "eval.h"
#include <assert.h>
#include "top_typecheck.h"
#include "type_infer.h"
#include "error.h"
#include "utils/utils.h"
#include <stdio.h>
#include <light_array.h>

#define TOKEN_STR(T) (T)->length, (T)->data

static s32
eval_error_location(Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", t->filepath);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", t->filepath, t->line + 1, t->column + 1);
    }
    return 0;
}

bool
eval_expr_is_constant(Light_Ast* expr, u32 flags, u32* error) {
	assert(expr->flags & AST_FLAG_EXPRESSION);
    assert(expr->type->flags & TYPE_FLAG_INTERNALIZED);

	switch(expr->kind) {
		case AST_EXPRESSION_BINARY:{
			return(eval_expr_is_constant(expr->expr_binary.left, flags, error) && 
				eval_expr_is_constant(expr->expr_binary.right, flags, error));
		}break;
		case AST_EXPRESSION_LITERAL_ARRAY: {
			Light_Ast** a = expr->expr_literal_array.array_exprs;
			u64 length = 0;
			if(a) {
				length = array_length(a);
			}
			bool res = true;
			for(u64 i = 0; i < length; ++i) {
				if(!eval_expr_is_constant(a[i], flags, error)){
					res = false;
				}
			}
			return res;
		} break;
		case AST_EXPRESSION_LITERAL_STRUCT:{
			Light_Ast** a = expr->expr_literal_struct.struct_exprs;
			u64 length = 0;
			if(a) {
				length = array_length(a);
			}
			bool res = true;
			for(u64 i = 0; i < length; ++i) {
				if(!eval_expr_is_constant(a[i], flags, error)){
					res = false;
				}
			}
			return res;
		} break;
		case AST_EXPRESSION_LITERAL_PRIMITIVE:{
			return true;
		}break;
		case AST_EXPRESSION_PROCEDURE_CALL:{
			if(flags & EVAL_ERROR_REPORT) {
				eval_error_location(expr->expr_proc_call.token);
                fprintf(stderr, "Type Error: Procedure call cannot be evaluated to a constant expression\n");
                *error |= TYPE_ERROR;
			}
			return false;
		}break;
		case AST_EXPRESSION_UNARY:{
			Light_Operator_Unary op = expr->expr_unary.op;
			switch(op){
				case OP_UNARY_DEREFERENCE: {
                    if(flags & EVAL_ERROR_REPORT) {
                        eval_error_location(expr->expr_unary.token_op);
                        fprintf(stderr, "Type Error: 'Dereference' operation cannot be evaluated to a constant expression\n");
                        *error |= TYPE_ERROR;
                    }
					return false;
                } break;
				case OP_UNARY_ADDRESSOF:{
                    if(flags & EVAL_ERROR_REPORT) {
                        eval_error_location(expr->expr_unary.token_op);
                        fprintf(stderr, "Type Error: 'Address of' operation cannot be evaluated to a constant expression\n");
                        *error |= TYPE_ERROR;
                    }
					return false;
				}break;
				case OP_UNARY_BITWISE_NOT:
				case OP_UNARY_CAST:
				case OP_UNARY_LOGIC_NOT:
				case OP_UNARY_MINUS:
				case OP_UNARY_PLUS:{
					return eval_expr_is_constant(expr->expr_unary.operand, flags, error);
				}break;
				default: assert(0); break;
			}
		}break;
		case AST_EXPRESSION_VARIABLE:{
			Light_Ast* decl = type_infer_decl_from_name(expr->scope_at, expr->expr_variable.name);
			if(!decl){
				if(flags & EVAL_ERROR_REPORT){
					eval_error_location(expr->expr_variable.name);
                    fprintf(stderr, "Type Error: Undeclared identifier '%.*s'\n", TOKEN_STR(expr->expr_variable.name));
                    *error |= TYPE_ERROR;
				}
				return false;
			} else {
				if(decl->kind == AST_DECL_CONSTANT || decl->kind == AST_DECL_PROCEDURE){
					return true;
				} else {
					if(flags & EVAL_ERROR_REPORT){
                        eval_error_location(expr->expr_variable.name);
                        fprintf(stderr, "Type Error: '%.*s' cannot be evaluated to a constant value\n", TOKEN_STR(expr->expr_variable.name));
                        *error |= TYPE_ERROR;
					}
					return false;
				}
			}
		} break;
        case AST_EXPRESSION_DOT: {
            switch(expr->type->kind) {
                case TYPE_KIND_ENUM:{
                    return true;
                } break;
                case TYPE_KIND_UNION:
                case TYPE_KIND_STRUCT:{
                    eval_error_location(expr->expr_dot.identifier);
                    fprintf(stderr, "Type Error: expression cannot be evaluated to a constant value\n");
                } break;
                default: assert(0); break;
            }
            return false;
        } break;
        default: assert(0); break;
	}
	return false;
}

s64
eval_expr_constant_int(Light_Ast* expr, u32* error) {	
    switch(expr->kind) {
        case AST_EXPRESSION_LITERAL_PRIMITIVE:{
			eval_literal_primitive(expr);
			return expr->expr_literal_primitive.value_s64;
		} break;
        case AST_EXPRESSION_BINARY:{
			s64 left = eval_expr_constant_int(expr->expr_binary.left, error);
			s64 right = eval_expr_constant_int(expr->expr_binary.right, error);
			if(*error & TYPE_ERROR) return 0;
			switch(expr->expr_binary.op) {
				case OP_BINARY_DIV:{
					if(right == 0){
						eval_error_location(expr->expr_binary.token_op);
						fprintf(stderr, "Type Error: Division by 0 while evaluating constant expression\n");
						*error |= TYPE_ERROR;
						return 0;
					} else {
						return left / right;
					}
				}break;
				case OP_BINARY_MOD:{
					if(right == 0){
						eval_error_location(expr->expr_binary.token_op);
						fprintf(stderr, "Type Error: Division by 0 while evaluating constant expression\n");
						*error |= TYPE_ERROR;
						return 0;
					} else {
						return left % right;
					}
				}break;
				case OP_BINARY_PLUS:
					return left + right;
				case OP_BINARY_MINUS:
					return left - right;
				case OP_BINARY_MULT:
					return left * right;
				case OP_BINARY_AND:
					return left & right;
				case OP_BINARY_OR:
					return left | right;
				case OP_BINARY_XOR:
					return left ^ right;
				case OP_BINARY_SHL:
					return left << right;
				case OP_BINARY_SHR:
					return left >> right;
				case OP_BINARY_LT:
				case OP_BINARY_GT:
				case OP_BINARY_LE:
				case OP_BINARY_GE:
				case OP_BINARY_EQUAL:
				case OP_BINARY_NOT_EQUAL:
				case OP_BINARY_LOGIC_AND:
				case OP_BINARY_LOGIC_OR:
					assert(0); // boolean type should not ever get here
					break;
				case OP_BINARY_VECTOR_ACCESS:
					// TODO(psv):
					assert(0);
					break;
				default: assert(0); break;
			}
		} break;
        case AST_EXPRESSION_DOT:{
			Light_Type* type = type_alias_root(expr->type);
			Light_Ast* decl = find_enum_field_decl(type->enumerator.enum_scope, expr->expr_dot.identifier, error);
			if(*error & TYPE_ERROR) return 0;
			if(decl) {
				s64 value = eval_expr_constant_int(decl, error);
				return value;
			}
			// TODO(psv): enum value
			assert(0);
		} break;
        case AST_EXPRESSION_UNARY: {
			switch(expr->expr_unary.op) {
				case OP_UNARY_DEREFERENCE:
				case OP_UNARY_ADDRESSOF:
					// not constant
					assert(0);
					break;
				case OP_UNARY_LOGIC_NOT:
					// not integer
					assert(0);
					break;
				case OP_UNARY_PLUS:{
					return eval_expr_constant_int(expr->expr_unary.operand, error);
				}break;
				case OP_UNARY_MINUS:{
					s64 opval = eval_expr_constant_int(expr->expr_unary.operand, error);
					return -opval;
				} break;
				case OP_UNARY_BITWISE_NOT:{
					s64 opval = eval_expr_constant_int(expr->expr_unary.operand, error);
					return ~opval;
				} break;
				case OP_UNARY_CAST:{
					assert(type_primitive_int(expr->expr_unary.type_to_cast));
					return eval_expr_constant_int(expr->expr_unary.operand, error);
				} break;
				default: assert(0); break;
			}
		} break;
        case AST_EXPRESSION_VARIABLE: {
			// Must be a constant declaration
			Light_Ast* decl = type_infer_decl_from_name(expr->scope_at, expr->expr_variable.name);
			if(!decl) {
				type_error_undeclared_identifier(error, expr->expr_variable.name);
				return 0;
			}
			if(decl->kind != AST_DECL_CONSTANT) {
				type_error(error, expr->expr_variable.name, 
					"expression '%.*s' is not a constant\n", expr->expr_variable.name);
				return 0;
			}
			return eval_expr_constant_int(decl->decl_constant.value, error);
		} break;
        case AST_EXPRESSION_DIRECTIVE:
            // TODO(psv): Implement
            return 2;
            break;
        default: assert(0); break;
    }
	return 0;
}

bool 
eval_literal_primitive(Light_Ast* p) {
    assert(p->type->kind == TYPE_KIND_PRIMITIVE);

	Light_Literal_Type ltype = p->expr_literal_primitive.type;

	char* token_data = (char*)p->expr_literal_primitive.token->data;
	s32 token_length = 	p->expr_literal_primitive.token->length;

	if(ltype == LITERAL_BOOL) {
		p->expr_literal_primitive.value_bool = (p->expr_literal_primitive.token->type == TOKEN_LITERAL_BOOL_TRUE);
	} else if( (ltype == LITERAL_FLOAT) || 
		(ltype == LITERAL_DEC_UINT) || 
		(ltype == LITERAL_DEC_SINT))
	{
		switch(p->type->primitive) {
			case TYPE_PRIMITIVE_R32:
				p->expr_literal_primitive.value_r32 = str_to_r32(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_R64:
				p->expr_literal_primitive.value_r64 = str_to_r64(token_data, token_length);
				break;

			case TYPE_PRIMITIVE_S8:
				p->expr_literal_primitive.value_s8 = (s8)str_to_s64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_S16:
				p->expr_literal_primitive.value_s16 = (s16)str_to_s64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_S32:
				p->expr_literal_primitive.value_s32 = (s32)str_to_s64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_S64:
				p->expr_literal_primitive.value_s64 = str_to_s64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_U8:
				p->expr_literal_primitive.value_u8 = (u8)str_to_u64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_U16:
				p->expr_literal_primitive.value_u16 = (u16)str_to_u64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_U32:
				p->expr_literal_primitive.value_u32 = (u32)str_to_u64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_U64:
				p->expr_literal_primitive.value_u64 = str_to_u64(token_data, token_length);
				break;
			case TYPE_PRIMITIVE_VOID: break;
			default: assert(0); break;
		}
	} else if(ltype == LITERAL_HEX_INT) {
		p->expr_literal_primitive.value_u64 = str_hex_to_u64(token_data, token_length);
	} else if(ltype == LITERAL_BIN_INT) {
		p->expr_literal_primitive.value_u64 = str_bin_to_u64(token_data, token_length);
	} else if(ltype == LITERAL_CHAR) {
		p->expr_literal_primitive.value_u32 = (u32)str_to_u8(token_data + 1, token_length - 2);
	}
    
    return true;
}