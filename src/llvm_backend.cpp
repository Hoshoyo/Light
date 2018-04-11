#include "llvm_backend.h"
#include "ho_system.h"
#include "lexer.h"
#include "decl_check.h"
#include <stdio.h>
#include <stdarg.h>

#define TOKEN_STR(T) T->value.length, T->value.data

int LLVM_Code_Generator::sprint(char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	int num_written = vsprintf(buffer + ptr, msg, args);
	va_end(args);
	ptr += num_written;
	return num_written;
}

int LLVM_Code_Generator::sprint_strlit(char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	int num_written = vsprintf(strlit_buffer + strlit_ptr, msg, args);
	va_end(args);
	strlit_ptr += num_written;
	return num_written;
}

s32 LLVM_Code_Generator::alloc_temp_register() {
	return temp++;
}

s32 LLVM_Code_Generator::gen_branch_label() {
	return br_label_temp++;
}

s32 LLVM_Code_Generator::alloc_strlit_temp() {
	return str_lit_temp++;
}

void LLVM_Code_Generator::reset_temp() {
	temp = 0;
	br_label_temp = 0;
}

void report_fatal_error(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	//assert(0);
	exit(-1);
}

const u32 EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 = FLAG(0);
const u32 EMIT_TYPE_FLAG_STRUCT_NAMED = FLAG(1);

void LLVM_Code_Generator::llvm_emit_type(Type_Instance* type, u32 flags) {
	switch (type->kind) {
		case KIND_PRIMITIVE: {
			switch (type->primitive) {
				case TYPE_PRIMITIVE_S64:  sprint("i64"); break;
				case TYPE_PRIMITIVE_S32:  sprint("i32"); break;
				case TYPE_PRIMITIVE_S16:  sprint("i16"); break;
				case TYPE_PRIMITIVE_S8:	  sprint("i8"); break;
				case TYPE_PRIMITIVE_U64:  sprint("i64"); break;
				case TYPE_PRIMITIVE_U32:  sprint("i32"); break;
				case TYPE_PRIMITIVE_U16:  sprint("i16"); break;
				case TYPE_PRIMITIVE_U8:	  sprint("i8"); break;
				case TYPE_PRIMITIVE_BOOL: sprint("i8"); break;
				case TYPE_PRIMITIVE_R64:  sprint("double"); break;
				case TYPE_PRIMITIVE_R32:  sprint("float"); break;
				case TYPE_PRIMITIVE_VOID: {
					(flags & EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8) ? sprint("i8") : sprint("void");
					break;
				}
			}
		}break;
		case KIND_POINTER: {
			llvm_emit_type(type->pointer_to, flags | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint("*");
		}break;
		case KIND_FUNCTION: {
			// @TODO:
		}break;
		case KIND_ARRAY: {
			// @TODO:
		}break;
		case KIND_STRUCT: {
			if (flags & EMIT_TYPE_FLAG_STRUCT_NAMED) {
				sprint("%%%.*s", TOKEN_STR(type->struct_desc.name));
			} else {
				sprint("{ ");
				size_t num_fields = array_get_length(type->struct_desc.fields_types);
				for (size_t i = 0; i < num_fields; ++i) {
					if (i != 0) sprint(", ");
					llvm_emit_type(type->struct_desc.fields_types[i]);
				}
				sprint(" }");
			}
		}break;
		default: {
			report_fatal_error("Internal compiler error: could not generate LLVM IR for unknown type id=%d\n", type->kind);
		}break;
	}
}

void LLVM_Code_Generator::llvm_emit_type_decls(Type_Instance** type_table)
{
	size_t n = array_get_length(type_table);
	for (int i = 0; i < n; ++i) {
		Type_Instance* entry = type_table[i];
		switch (entry->kind) {
			case KIND_PRIMITIVE:
			case KIND_POINTER:
			case KIND_FUNCTION:
			case KIND_ARRAY:
				// ... do nothing since they are all mapped to llvm directly
				break;
			case KIND_STRUCT: {
				sprint("%%%.*s = type ", TOKEN_STR(entry->struct_desc.name));
				llvm_emit_type(entry);
				sprint("\n");
			}break;
			default: {
				report_fatal_error("Internal compiler error: could not generate LLVM IR for unknown type id=%d\n", entry->kind);
			}break;
		}
	}
	sprint("\n");
}

void LLVM_Code_Generator::llvm_emit_node(Ast* node) {
	switch (node->node_type) {

		/*
			Emit llvm code for Ast Node Procedure declaration of types:
			foreign declarations
			normal procedure calls

			@TODO: local calls
		*/
		case AST_DECL_PROCEDURE: {
			if (node->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) {
				sprint("declare cc 64 ");
				llvm_emit_type(node->decl_procedure.type_return);
				sprint(" @%.*s(", TOKEN_STR(node->decl_procedure.name));
				for (size_t i = 0; i < node->decl_procedure.arguments_count; ++i) {
					if (i != 0) sprint(",");
					llvm_emit_type(node->decl_procedure.arguments[i]->decl_variable.variable_type);
					sprint(" %%%.*s", TOKEN_STR(node->decl_procedure.arguments[i]->decl_variable.name));
				}
				sprint(") #0\n");
				reset_temp();
			} else {
				sprint("define ");
				llvm_emit_type(node->decl_procedure.type_return);
				sprint(" @%.*s(", TOKEN_STR(node->decl_procedure.name));
				for (size_t i = 0; i < node->decl_procedure.arguments_count; ++i) {
					if (i != 0) sprint(",");
					llvm_emit_type(node->decl_procedure.arguments[i]->decl_variable.variable_type);
					sprint(" %%%.*s", TOKEN_STR(node->decl_procedure.arguments[i]->decl_variable.name));
				}
				sprint(") ");
				sprint("#1 {\ndecls-0:\n");
				llvm_emit_node(node->decl_procedure.body);
				sprint("}\n");
				reset_temp();
			}
		}break;

		case AST_COMMAND_BLOCK: {
			for (size_t i = 0; i < node->comm_block.command_count; ++i) {
				llvm_emit_node(node->comm_block.commands[i]);
				sprint("\n");
			}
		} break;

		case AST_DECL_VARIABLE: {
			s32 temp = alloc_temp_register();
			node->decl_variable.temporary_register = temp;

			sprint("%%%d = alloca ", temp);
			llvm_emit_type(node->decl_variable.variable_type, EMIT_TYPE_FLAG_STRUCT_NAMED);
			sprint(", align %d", node->decl_variable.alignment);

			if (node->decl_variable.assignment) {
				sprint("\n");
				llvm_emit_assignment(node);
			}
		} break;

		case AST_COMMAND_RETURN: {
			if (!node->comm_return.expression) {
				sprint("ret void");
			} else if (ast_node_is_embeded_literal(node->comm_return.expression)) {
				sprint("ret ");
				llvm_emit_type(node->comm_return.expression->type_return);
				sprint(" ");
				llvm_emit_expression(node->comm_return.expression);
			} else {
				s32 temp = llvm_emit_expression(node->comm_return.expression);
				sprint("\nret ");
				llvm_emit_type(node->comm_return.expression->type_return);
				sprint(" %%%d", temp);
			}
			// This needs to be here because llvm expects a label named after a return statement for some reason
			// see: https://stackoverflow.com/questions/36094685/instruction-expected-to-be-numbered
			alloc_temp_register();
			sprint("\n");
		} break;

		case AST_COMMAND_IF: {
			s32 true_label  = gen_branch_label();
			s32 false_label = (node->comm_if.body_false) ? gen_branch_label() : -1;
			s32 end_label = true_label;

			if (ast_node_is_embeded_literal(node->comm_if.condition)) {
				sprint("br i1 ");
				llvm_emit_expression(node->comm_if.condition);
			} else {
				s32 boolexp_temp = llvm_emit_expression(node->comm_if.condition);
				sprint("\nbr i1 %%%d", boolexp_temp);
			}
			if (false_label != -1)
				sprint(", label %%if-stmt-%d, label %%if-stmt-%d\n", true_label, false_label);
			else
				sprint(", label %%if-stmt-%d, label %%if-end-%d\n", true_label, end_label);
			sprint("if-stmt-%d:\n", true_label);
			llvm_emit_node(node->comm_if.body_true);
			sprint("br label %%if-end-%d\n", end_label);
			if(false_label != -1)
				sprint("if-stmt-%d:", false_label);
			if (node->comm_if.body_false) {
				llvm_emit_node(node->comm_if.body_false);
				sprint("br label %%if-end-%d\n", end_label);
			}
			sprint("if-end-%d:\n", end_label);
		}break;

		case AST_COMMAND_VARIABLE_ASSIGNMENT: {
			s32 var_reg = -1;

			if (node->comm_var_assign.lvalue) {
				// Evaluate lvalue first
				if (ast_node_is_embeded_variable(node->comm_var_assign.lvalue)) {
					Ast* decl = decl_from_name(node->scope, node->comm_var_assign.lvalue->expr_variable.name);
					assert(decl->node_type == AST_DECL_VARIABLE);
					var_reg = decl->decl_variable.temporary_register;
				} else {
					// TODO(psv): compose values assignment, copy semantic
					var_reg = llvm_emit_expression(node->comm_var_assign.lvalue);
					sprint("\n");
				}

				// Then evaluate rvalue
				if (ast_node_is_embeded_literal(node->comm_var_assign.rvalue)) {
					sprint("store ");
					llvm_emit_type(node->comm_var_assign.rvalue->type_return, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
					sprint(" ");
					llvm_emit_expression(node->comm_var_assign.rvalue);
				}
				else {
					s32 res_temp = llvm_emit_expression(node->comm_var_assign.rvalue);
					sprint("store ");
					llvm_emit_type(node->comm_var_assign.rvalue->type_return, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
					sprint(" %%%d", res_temp);
				}
				sprint(", ");


				llvm_emit_type(node->comm_var_assign.lvalue->type_return, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
				sprint(" %%%d", var_reg);
			} else {
				llvm_emit_node(node->comm_var_assign.rvalue);
			}
		}break;

		default: {
			if (node->flags & AST_FLAG_IS_EXPRESSION) {
				llvm_emit_expression(node);
				sprint("\n");
			}
		}break;
	}
}

s32 LLVM_Code_Generator::llvm_emit_expression(Ast* expr) {
	s32 result = -1;

	switch (expr->node_type) {
		case AST_EXPRESSION_PROCEDURE_CALL: {
			// evaluate the arguments first
			size_t num_args = 0;
			s32*   arg_temps = 0;
			
			if (expr->expr_proc_call.args)
				num_args = array_get_length(expr->expr_proc_call.args);

			if (num_args) {
				// temporary array to hold result register
				arg_temps = array_create(s32, num_args);
				for (size_t i = 0; i < num_args; ++i) {
					Ast* e = expr->expr_proc_call.args[i];
					//if (e->node == AST_NODE_LITERAL_EXPRESSION && e->expression.literal_exp.flags & LITERAL_FLAG_NUMERIC) {
					if(ast_node_is_embeded_literal(e)) {
						arg_temps[i] = -1;
					} else {
						arg_temps[i] = llvm_emit_expression(e);
					}
				}
				sprint("\n");
			}

			// alloc the proc call result register
			if (expr->type_return->kind == KIND_PRIMITIVE && expr->type_return->primitive == TYPE_PRIMITIVE_VOID) {
				sprint("call ");
			} else {
				result = alloc_temp_register();
				sprint("%%%d = call ", result);
			}
			llvm_emit_type(expr->type_return);
			sprint(" @%.*s(", TOKEN_STR(expr->expr_proc_call.name));
			for (size_t i = 0; i < num_args; ++i) {
				assert(arg_temps);
				if (i != 0) sprint(", ");
				Ast* arg = expr->expr_proc_call.args[i];
				llvm_emit_type(arg->type_return, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
				sprint(" ");
				if (arg_temps[i] == -1) {
					llvm_emit_expression(arg);
				} else {
					sprint(" %%%d", arg_temps[i]);
				}
			}
			sprint(")\n");
			if(arg_temps) array_release(arg_temps);
		}break;

		case AST_EXPRESSION_LITERAL: {
			Ast_Expr_Literal* l = &expr->expr_literal;
			//if(expr->expression.literal_exp.flags)
			switch (l->token->type) {
				case TOKEN_LITERAL_HEX_INT:
				case TOKEN_LITERAL_BIN_INT:{
					sprint("%llx", l->value_u64);
				}break;
				case TOKEN_LITERAL_INT: {
					// @TODO: convert it properly
					sprint("%.*s", TOKEN_STR(l->token));
				}break;
				case TOKEN_LITERAL_FLOAT: {
					// @TODO: convert it properly
					sprint("%.*s", TOKEN_STR(l->token));
				}break;
				case TOKEN_LITERAL_BOOL_FALSE: {
					sprint("false");
				}break;
				case TOKEN_LITERAL_BOOL_TRUE: {
					sprint("true");
				}break;
				case TOKEN_LITERAL_STRING: {
					result = alloc_temp_register();
					s32 index = llvm_define_string_literal(l);
					size_t len = l->token->value.length + 1; // \0
					sprint("%%%d = getelementptr [%d x i8], [%d x i8]* @__str$%d, i64 0, i64 0\n", result, len, len, index);
				}break;
			}
		} break;

		case AST_EXPRESSION_VARIABLE: {
			Ast* var_decl = decl_from_name(expr->scope, expr->expr_variable.name);
			if(ast_node_is_embeded_literal(expr)){
				sprint("%%%.*s", TOKEN_STR(expr->expr_variable.name));
				return -1;
			}
			assert(var_decl->node_type == AST_DECL_VARIABLE);

			// get temporary register containing the variable address
			s32 var_temp_reg = var_decl->decl_variable.temporary_register;
			
			result = alloc_temp_register();

			sprint("%%%d = load ", result);
			llvm_emit_type(expr->type_return, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint(", ");
			llvm_emit_type(expr->type_return, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint("* %%%d", var_temp_reg);
		} break;

		case AST_COMMAND_VARIABLE_ASSIGNMENT: {
			result = llvm_emit_assignment(expr);
		}break;
		case AST_EXPRESSION_BINARY: {
			result = llvm_emit_binary_expression(expr);
		} break;
		case AST_EXPRESSION_UNARY:{
			result = llvm_emit_unary_expression(expr);
		}break;
		default: assert_msg(0, "node type llvm_emit not implemented\n"); break;
	}

	return result;
}

s32 LLVM_Code_Generator::llvm_emit_unary_expression(Ast* expr) {
	Operator_Unary op = expr->expr_unary.op;
	Ast* operand = expr->expr_unary.operand;
	Type_Instance* optype = operand->type_return;

	s32 result = -1;
	s32 operand_result = -1;
	bool embeded_operand = ast_node_is_embeded_literal(operand);
	if(!embeded_operand){
		operand_result = llvm_emit_expression(operand);
		sprint("\n");
	}

	switch(op){
		case OP_UNARY_ADDRESSOF:
		case OP_UNARY_BITWISE_NOT:
			assert_msg(0, "emit '&' and '!' not implemented yet\n");
			break;

		// Unary Cast
		case OP_UNARY_CAST:{
			Type_Instance* ttc = expr->expr_unary.type_to_cast;
			if(ttc->kind == KIND_POINTER) {
				if(optype->kind == KIND_POINTER){
					// cast(^T)^V
					result = alloc_temp_register();
					sprint("%%%d = bitcast ", result);
					llvm_emit_type(optype, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
					sprint(" ");
					if(embeded_operand){
						llvm_emit_expression(operand);
					} else {
						sprint("%%%d", operand_result);
					}
					sprint(" to ");
					llvm_emit_type(ttc, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
					sprint("\n");
				} else if(type_primitive_int(optype)) {
					// cast(^T)INT
					s32 r = -1;
					if(optype->type_size_bits < type_pointer_size_bits()){
						// extend
						r = alloc_temp_register();
						if(type_primitive_int_signed(optype)){
							sprint("%%%d = sext ", r);
						} else if(type_primitive_int_unsigned(optype)){
							sprint("%%%d = zext ", r);
						} else {
							assert_msg(0, "unknown integer type");
						}
						llvm_emit_type(optype, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
						sprint(" ");
						if(embeded_operand){
							llvm_emit_expression(operand);
						} else {
							sprint("%%%d", operand_result);
						}
						sprint(" to i64\n");	// @HARDCODE pointer type is always 64 bits
					}
					result = alloc_temp_register();
					sprint("%%%d = inttoptr i64 ", result);
					if(r != -1){
						sprint("%%%d", r);
					} else if(embeded_operand) {
						llvm_emit_expression(operand);
					} else {
						sprint("%%%d", operand_result);
					}
					sprint(" to ");					
					llvm_emit_type(ttc, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
				}
			} else if(type_primitive_int(ttc) && operand->type_return->kind == KIND_POINTER){
				// cast(INT)^T
				result = alloc_temp_register();
				sprint("%%%d = ptrtoint ", result);
				llvm_emit_type(operand->type_return, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
				if(embeded_operand){
					llvm_emit_expression(operand);
				} else {
					sprint(" %%%d", operand_result);
				}
				sprint(" to ");
				llvm_emit_type(ttc, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
				sprint("\n");
			} else if(type_primitive_int(ttc) && type_primitive_int(operand->type_return)) {
				if(ttc != operand->type_return){
					result = alloc_temp_register();
					if(ttc->type_size_bits < operand->type_return->type_size_bits) {
						sprint("%%%d = trunc ", result);
					} else {
						if(type_primitive_int_signed(operand->type_return)) {
							sprint("%%%d = sext ", result);
						} else if(type_primitive_int_unsigned(operand->type_return)) {
							sprint("%%%d = zext ", result);
						} else {
							assert_msg(0, "undefined integer type");
						}
					}
					llvm_emit_type(operand->type_return, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
					if(embeded_operand){
						llvm_emit_expression(operand);
					} else {
						sprint(" %%%d", operand_result);
					}
					sprint(" to ");
					llvm_emit_type(ttc, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8 | EMIT_TYPE_FLAG_STRUCT_NAMED);
					sprint("\n");
				} else {
					result = operand_result;
				}
			}
		}break;


		case OP_UNARY_DEREFERENCE:


		case OP_UNARY_LOGIC_NOT:{

		}break;


		case OP_UNARY_MINUS:{
			if(type_primitive_int(operand->type_return)){
				result = alloc_temp_register();
				sprint("%%%d = mul ", result);
				llvm_emit_type(operand->type_return);
				sprint(" -1, ");
				if(embeded_operand){
					llvm_emit_expression(operand);
				} else {
					assert(operand_result != -1);
					sprint(" %%%d", operand_result);
				}
				sprint("\n");
			} else if(type_primitive_float(operand->type_return)) {
				result = alloc_temp_register();
				sprint("%%%d = fmul ", result);
				llvm_emit_type(operand->type_return);
				sprint(" -1.0, ");
				if(embeded_operand){
					llvm_emit_expression(operand);
				} else {
					assert(operand_result != -1);
					sprint(" %%%d", operand_result);
				}
				sprint("\n");
			} else {
				// internal compiler error, how did this pass type check?
				assert(0);
			}
		}break;
		case OP_UNARY_PLUS: break;
		default: assert(0); break; // internal compiler error here
	}
	return result;
}

s32 LLVM_Code_Generator::llvm_emit_binary_expression(Ast* expr) {
	Operator_Binary op = expr->expr_binary.op;
	Type_Instance* left_type  = expr->expr_binary.left->type_return;
	Type_Instance* right_type = expr->expr_binary.right->type_return;

	s32 t_left = -1;
	s32 t_right = -1;
	if (!ast_node_is_embeded_literal(expr->expr_binary.left)) {
		t_left = llvm_emit_expression(expr->expr_binary.left);
		sprint("\n");
	}
	if (!ast_node_is_embeded_literal(expr->expr_binary.right)) {
		t_right = llvm_emit_expression(expr->expr_binary.right);
		sprint("\n");
	}
	s32 temp_reg = alloc_temp_register();
	sprint("%%%d = ", temp_reg);

	if(op == OP_BINARY_DOT){
		// Struct accessing
		printf("Here");
	}

	// If it is a floating point type llvm offers operations like
	// fcmp, fadd, fsub, ...
	if (type_primitive_float(left_type) && type_primitive_float(right_type)) {
		switch (op) {
			case OP_BINARY_PLUS:		sprint("fadd "); break;
			case OP_BINARY_MINUS:		sprint("fsub "); break;
			case OP_BINARY_MULT:		sprint("fmul "); break;
			case OP_BINARY_DIV:			sprint("fdiv "); break;

			case OP_BINARY_EQUAL:		sprint("fcmp oeq "); break;
			case OP_BINARY_NOT_EQUAL:	sprint("fcmp one "); break;
			case OP_BINARY_GE:			sprint("fcmp oge "); break;
			case OP_BINARY_GT:			sprint("fcmp ogt "); break;
			case OP_BINARY_LE:			sprint("fcmp ole "); break;
			case OP_BINARY_LT:			sprint("fcmp olt "); break;
		}
		// float or double always
	} else {
		bool is_signed = type_primitive_int_signed(left_type) && type_primitive_int_signed(right_type);
		// is integer type or pointer arithmetic
		if(type_primitive_int(left_type) && type_primitive_int(right_type)){
			bool comparison = false;
			switch(op){
				case OP_BINARY_PLUS:		sprint("add "); break;
				case OP_BINARY_MINUS:		sprint("sub "); break;
				case OP_BINARY_MULT:		sprint("mul "); break;
				case OP_BINARY_DIV:			sprint("div "); break;
				case OP_BINARY_MOD:			(is_signed) ? sprint("srem ") : sprint("urem "); break;

				case OP_BINARY_EQUAL:
				case OP_BINARY_NOT_EQUAL:
				case OP_BINARY_GE:
				case OP_BINARY_GT:
				case OP_BINARY_LE:
				case OP_BINARY_LT:			comparison = true; sprint("icmp "); break;
			}
			if(comparison && is_signed){
				switch(op){
					case OP_BINARY_EQUAL:		sprint("eq "); break;
					case OP_BINARY_NOT_EQUAL:	sprint("ne "); break;
					case OP_BINARY_GE:			sprint("sge "); break;
					case OP_BINARY_GT:			sprint("sgt "); break;
					case OP_BINARY_LE:			sprint("sle "); break;
					case OP_BINARY_LT:			sprint("slt "); break;
				}
			} else {
				switch(op){
					case OP_BINARY_EQUAL:		sprint("eq "); break;
					case OP_BINARY_NOT_EQUAL:	sprint("ne "); break;
					case OP_BINARY_GE:			sprint("uge "); break;
					case OP_BINARY_GT:			sprint("ugt "); break;
					case OP_BINARY_LE:			sprint("ule "); break;
					case OP_BINARY_LT:			sprint("ult "); break;
				}
			}
		}
	}

	llvm_emit_type(left_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
	sprint(" ");
	if (t_left == -1) {
		llvm_emit_expression(expr->expr_binary.left);
		sprint(", ");
	} else {
		sprint("%%%d, ", t_left);
	}

	if (t_right == -1) {
		llvm_emit_expression(expr->expr_binary.right);
	} else {
		sprint("%%%d", t_right);
	}
	return temp_reg;
}

s32 LLVM_Code_Generator::llvm_emit_assignment(Ast* expr) {
	// TODO(psv): get lvalue (address of expression in the left)
	// assume variable for now

	Ast* left = 0;
	Ast* right = 0;
	Ast* var_decl = 0;
	Type_Instance* node_type = 0;
	if (expr->node_type == AST_EXPRESSION_BINARY) {
		left = expr->expr_binary.left;
		right = expr->expr_binary.right;
		var_decl = decl_from_name(left->scope, left->expr_variable.name);
		node_type = left->type_return;
	} else if(expr->node_type == AST_DECL_VARIABLE) {
		right = expr->decl_variable.assignment;
		var_decl = expr;
		node_type = var_decl->decl_variable.variable_type;
	}

	s32 temp = var_decl->decl_variable.temporary_register;
	
	if (!ast_node_is_embeded_literal(right)) {
		switch (right->type_return->kind) {
			case KIND_STRUCT: {
				if (right->node_type == AST_EXPRESSION_LITERAL && right->expr_literal.flags & LITERAL_FLAG_STRING) {
					s32 str_index = llvm_define_string_literal(&right->expr_literal);
					s32 temp_char_ptr = alloc_temp_register();
					size_t length = right->expr_literal.token->value.length + 1;
					// get char array ptr to put on data field of string
					sprint("%%%d = getelementptr [%lld x i8], [%lld x i8]* @__str$%d, i64 0, i64 0\n", temp_char_ptr, length, length, str_index);
					s32 length_reg = alloc_temp_register();
					s32 data_reg   = alloc_temp_register();
					sprint("%%%d = getelementptr %%string, %%string* %%%d, i64 0, i32 0\n", length_reg, temp);
					sprint("%%%d = getelementptr %%string, %%string* %%%d, i64 0, i32 1\n", data_reg, temp);
					sprint("store i64 %lld, i64* %%%d\n", length, length_reg);
					sprint("store i8* %%%d, i8** %%%d\n", temp_char_ptr, data_reg);

				} else if (right->node_type == AST_EXPRESSION_VARIABLE) {
					int x = 0;
				} else {
					// @TODO(psv): other kinds of literals
					report_fatal_error("other literals are not yet supported\n");
				}
			}break;

			case KIND_FUNCTION:
			case KIND_ARRAY:
			case TYPE_UNKNOWN: {
				report_fatal_error("type not supported or unknown\n");
			}break;
			case KIND_POINTER:
			case KIND_PRIMITIVE: {
				s32 result = llvm_emit_expression(right);
				sprint("\nstore ");
				llvm_emit_type(right->type_return, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
				sprint(" %%%d, ", result);
				llvm_emit_type(right->type_return, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
				sprint("* %%%d", temp);
			}break;
		}
	} else {
		sprint("store ");
		llvm_emit_type(node_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
		sprint(" ");
		llvm_emit_expression(right);

		sprint(", ");
		llvm_emit_type(node_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
		sprint("* %%%d", temp);
	}

	return 0;
}

s32 LLVM_Code_Generator::llvm_define_string_literal(Ast_Expr_Literal* lit) {
	s32 n = alloc_strlit_temp();

	size_t litlength = lit->token->value.length;
	lit->llvm_index = n;
	// @TODO(psv): temporary print of literal string
	sprint_strlit("@__str$%d = private global [%d x i8] c\"%.*s\\00\"\n", n, litlength + 1, TOKEN_STR(lit->token));
	return n;
}

bool ast_node_is_embeded_literal(Ast* node) {
	if(node->node_type == AST_EXPRESSION_VARIABLE && node->type_return->kind == KIND_PRIMITIVE) {
		Ast* decl = decl_from_name(node->scope, node->expr_variable.name);
		if(decl->node_type == AST_DECL_VARIABLE && decl->scope->flags & SCOPE_PROCEDURE_ARGUMENTS)
			return true;
	}
	if (node->type_return->kind != KIND_PRIMITIVE) return false;
	return (node->node_type == AST_EXPRESSION_LITERAL && type_primitive_numeric(node->type_return));
}

bool ast_node_is_embeded_variable(Ast* node) {
	if (node->node_type == AST_EXPRESSION_VARIABLE) {
		Ast* decl = decl_from_name(node->scope, node->expr_variable.name);
		if (decl->node_type == AST_DECL_VARIABLE)
			return true;
	}
	return false;
}









static size_t filename_length_strip_extension(char* f) {
	// Extract the base filename without the extension
	size_t baselen = strlen(f);
	size_t startlen = baselen;
	while (f[baselen] != '.') baselen--;
	if (baselen == 0) baselen = startlen;
	return baselen;
}

#if defined(_WIN32) || defined(_WIN64)
// @TEMPORARY Windows only for now
void llvm_generate_ir(Ast** toplevel, Type_Instance** type_table, char* filename) {
	LLVM_Code_Generator code_generator = {};
	code_generator.in_filename = filename;

	code_generator.sprint("target triple = \"x86_64-pc-windows-msvc\"\n\n");
	code_generator.llvm_emit_type_decls(type_table);

	size_t num_decls = array_get_length(toplevel);
	for (size_t i = 0; i < num_decls; ++i) {
		Ast* node = toplevel[i];
		code_generator.llvm_emit_node(node);
	}

	code_generator.sprint("\n");
	code_generator.sprint("attributes #0 = { nounwind uwtable }\n");
	code_generator.sprint("attributes #1 = { nounwind uwtable }\n");

	string out_obj = string_make("examples/factorial.ll", sizeof("examples/factorial.ll") - 1);
	
	HANDLE out = ho_createfile((const char*)out_obj.data, FILE_WRITE, CREATE_ALWAYS);
	ho_writefile(out, code_generator.ptr, (u8*)code_generator.buffer);
	ho_writefile(out, code_generator.strlit_ptr, (u8*)code_generator.strlit_buffer);
	ho_closefile(out);

	char cmdbuffer[1024];
	sprintf(cmdbuffer, "llc -filetype=obj -march=x86-64 %s -o %.*s.obj", out_obj.data, sizeof("examples/factorial.ll") - 1, out_obj.data);
	system(cmdbuffer);
	sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj -emain -nostdlib -o %.*s.exe lib/kernel32.lib lib/msvcrt.lib", 
		sizeof("examples/factorial.ll") - 1, out_obj.data, sizeof("examples/factorial.ll") - 1, out_obj.data);
	system(cmdbuffer);
}
#elif defined(__linux__)
void llvm_generate_ir(Ast** toplevel, Type_Instance** type_table, char* filename) {
	LLVM_Code_Generator code_generator = {};
	code_generator.in_filename = filename;

	code_generator.sprint("target triple = \"x86_64-linux-gnu\"\n\n");
	code_generator.llvm_emit_type_decls(type_table);

	size_t num_decls = array_get_length(toplevel);
	for (size_t i = 0; i < num_decls; ++i) {
		Ast* node = toplevel[i];
		code_generator.llvm_emit_node(node);
	}

	code_generator.sprint("\n");
	code_generator.sprint("attributes #0 = { nounwind uwtable }\n");
	code_generator.sprint("attributes #1 = { nounwind uwtable }\n");

	size_t fname_len = filename_length_strip_extension(filename);
	//string out_obj(fname_len + sizeof(".ll"), fname_len, filename);
	string out_obj = string_new(filename, fname_len);
	string_append(&out_obj, ".ll");
	
	HANDLE out = ho_createfile(out_obj.data, FILE_WRITE, CREATE_ALWAYS);
	if(out == INVALID_HANDLE_VALUE){
		report_fatal_error("Could not create file %s", out_obj.data);
	}
	ho_writefile(out, code_generator.ptr, (u8*)code_generator.buffer);
	ho_writefile(out, code_generator.strlit_ptr, (u8*)code_generator.strlit_buffer);
	ho_closefile(out);

	char cmdbuffer[1024];
	sprintf(cmdbuffer, "llc -filetype=obj -march=x86-64 %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	//sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj -nostdlib -o %.*s.exe lib/kernel32.lib lib/msvcrt.lib", fname_len, out_obj.data, fname_len, out_obj.data);
	sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj temp/entry.o -o %.*s -s -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc", fname_len, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
}
#endif

