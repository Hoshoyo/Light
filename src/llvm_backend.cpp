#include "llvm_backend.h"
#include "lexer.h"
#include "ho_system.h"
#include "semantic.h"
#include <stdio.h>

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

s32 LLVM_Code_Generator::alloc_temp_register() {
	return temp++;
}

s32 LLVM_Code_Generator::gen_branch_label() {
	return br_label_temp++;
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
	switch (type->type) {
		case TYPE_PRIMITIVE: {
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
		case TYPE_POINTER: {
			llvm_emit_type(type->pointer_to, EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint("*");
		}break;
		case TYPE_FUNCTION: {
			// @TODO:
		}break;
		case TYPE_ARRAY: {
			// @TODO:
		}break;
		case TYPE_STRUCT: {
			if (flags & EMIT_TYPE_FLAG_STRUCT_NAMED) {
				sprint("%%%.*s", type->type_struct.name_length, type->type_struct.name);
			} else {
				sprint("{ ");
				size_t num_fields = array_get_length(type->type_struct.fields_types);
				for (size_t i = 0; i < num_fields; ++i) {
					if (i != 0) sprint(", ");
					llvm_emit_type(type->type_struct.fields_types[i]);
				}
				sprint(" }");
			}
		}break;
		default: {
			report_fatal_error("Internal compiler error: could not generate LLVM IR for unknown type id=%d\n", type->type);
		}break;
	}
}

void LLVM_Code_Generator::llvm_emit_type_decls(Type_Table* type_table)
{
	for (int i = 0; i < type_table->type_entries_index; ++i) {
		s64 hash = type_table->type_entries_hashes[i];
		Type_Instance* entry = type_table->get_entry(hash);
		switch (entry->type) {
			case TYPE_PRIMITIVE:
			case TYPE_POINTER:
			case TYPE_FUNCTION:
			case TYPE_ARRAY:
				// ... do nothing since they are all mapped to llvm directly
				break;
			case TYPE_STRUCT: {
				sprint("%%%.*s = type ", entry->type_struct.name_length, entry->type_struct.name);
				llvm_emit_type(entry);
				sprint("\n");
			}break;
			default: {
				report_fatal_error("Internal compiler error: could not generate LLVM IR for unknown type id=%d\n", entry->type);
			}break;
		}
	}
	sprint("\n");
}

void LLVM_Code_Generator::llvm_emit_node(Ast* node) {
	switch (node->node) {

		case AST_NODE_PROC_FORWARD_DECL: {
			// @TODO
		}break;

		/*
			Emit llvm code for Ast Node Procedure declaration of types:
			foreign declarations
			normal procedure calls

			@TODO: local calls
		*/
		case AST_NODE_PROC_DECLARATION: {
			sprint("declare ");
			
			if (node->proc_decl.flags & PROC_DECL_FLAG_IS_EXTERNAL) {
				sprint("cc 64 ");
			} else {
				sprint("ccc i32 ");
			}
			llvm_emit_type(node->proc_decl.proc_ret_type);
			sprint(" @%.*s(", TOKEN_STR(node->proc_decl.name));
			for (size_t i = 0; i < node->proc_decl.num_args; ++i) {
				if (i != 0) sprint(",");
				llvm_emit_type(node->proc_decl.arguments[i]->named_arg.arg_type);
				sprint(" %%%.*s", TOKEN_STR(node->proc_decl.arguments[i]->named_arg.arg_name));
			}
			sprint(") ");
			if (node->proc_decl.flags & PROC_DECL_FLAG_IS_EXTERNAL) {
				sprint("#0\n");
			} else {
				sprint("#1 ");
				sprint("{\n");
				llvm_emit_node(node->proc_decl.body);
				sprint("}\n");
				reset_temp();
			}
		}break;

		case AST_NODE_BLOCK: {
			size_t num_commands = array_get_length(node->block.commands);
			for (size_t i = 0; i < num_commands; ++i) {
				llvm_emit_node(node->block.commands[i]);
				sprint("\n");
			}
		} break;

		case AST_NODE_VARIABLE_DECL: {
			s32 temp = alloc_temp_register();
			node->var_decl.temporary_register = temp;

			sprint("%%%d = alloca ", temp);
			llvm_emit_type(node->var_decl.type, EMIT_TYPE_FLAG_STRUCT_NAMED);
			sprint(", align %d", node->var_decl.alignment);

			// @TODO: assignment if there is
		} break;

		case AST_NODE_RETURN_STATEMENT: {
			if (!node->ret_stmt.expr) {
				sprint("ret void");
			} else if (ast_node_is_embeded_literal(node->ret_stmt.expr)) {
				sprint("ret ");
				llvm_emit_type(node->ret_stmt.expr->return_type);
				sprint(" ");
				llvm_emit_expression(node->ret_stmt.expr);
			} else {
				s32 temp = llvm_emit_expression(node->ret_stmt.expr);
				sprint("\nret ");
				llvm_emit_type(node->ret_stmt.expr->return_type);
				sprint(" %%%d", temp);
			}
		} break;

		case AST_NODE_IF_STATEMENT: {
			// TODO(psv): need a few more jumps
			s32 true_label  = gen_branch_label();
			s32 false_label = gen_branch_label();

			if (ast_node_is_embeded_literal(node->if_stmt.bool_exp)) {
				sprint("br i1 ");
				llvm_emit_expression(node->if_stmt.bool_exp);			
			} else {
				s32 boolexp_temp = llvm_emit_expression(node->if_stmt.bool_exp);
				sprint("\nbr i1 %%%d", boolexp_temp);
			}
			sprint(", label %%if-stmt-%d, label %%if-stmt-%d\n", true_label, false_label);
			sprint("if-stmt-%d:\n", true_label);
			llvm_emit_node(node->if_stmt.body);
			sprint("\nif-stmt-%d:\n", false_label);
			if (node->if_stmt.else_exp) {
				llvm_emit_node(node->if_stmt.else_exp);
			}
		}break;

		default: {
			if (node->is_expr) {
				llvm_emit_expression(node);
			}
		}break;
	}
}

s32 LLVM_Code_Generator::llvm_emit_expression(Ast* expr) {
	s32 result = -1;

	switch (expr->node) {
		case AST_NODE_PROCEDURE_CALL: {
			// evaluate the arguments first
			size_t num_args = 0;
			s32* arg_temps = 0;
			
			if (num_args) {
				if (expr->expression.proc_call.args)
					num_args = array_get_length(expr->expression.proc_call.args);
				// temporary array to hold result register
				arg_temps = array_create(s32, num_args);
				for (size_t i = 0; i < num_args; ++i) {
					Ast* e = expr->expression.proc_call.args[i];
					if (e->node == AST_NODE_LITERAL_EXPRESSION && e->expression.literal_exp.flags & LITERAL_FLAG_NUMERIC) {
						arg_temps[i] = -1;
					}
					else {
						arg_temps[i] = llvm_emit_expression(e);
					}
				}
				sprint("\n");
			}

			// alloc the proc call result register
			result = alloc_temp_register();
			sprint("%%%d = call ", result);
			sprint("@%.*s(", TOKEN_STR(expr->expression.proc_call.name));
			for (size_t i = 0; i < num_args; ++i) {
				assert(arg_temps);
				if (i != 0) sprint(", ");
				Ast* arg = expr->expression.proc_call.args[i];
				llvm_emit_type(arg->return_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
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

		case AST_NODE_LITERAL_EXPRESSION: {
			Ast_Literal* l = &expr->expression.literal_exp;
			//if(expr->expression.literal_exp.flags)
			switch (l->lit_tok->type) {
				case TOKEN_INT_LITERAL: {
					// @TODO: convert it properly
					sprint("%.*s", TOKEN_STR(l->lit_tok));
				}break;
				case TOKEN_FLOAT_LITERAL: {
					// @TODO: convert it properly
					sprint("%.*s", TOKEN_STR(l->lit_tok));
				}break;
				case TOKEN_CHAR_LITERAL: {
					// @TODO:
				}break;
				case TOKEN_BOOL_LITERAL: {
					if (l->lit_value == 0) {
						sprint("false");
					} else {
						sprint("true");
					}
				}break;
			}
		} break;

		case AST_NODE_VARIABLE_EXPRESSION: {
			Ast_Variable* v = &expr->expression.variable_exp;
			s64 index = v->scope->symb_table->entry_exist(v->name);
			Ast* var_decl = v->scope->symb_table->entries[index].node;
			assert(var_decl->node == AST_NODE_VARIABLE_DECL);
			assert(v->type);
			assert(expr->return_type);

			// get temporary register containing the variable address
			s32 var_temp_reg = var_decl->var_decl.temporary_register;
			
			result = alloc_temp_register();

			sprint("%%%d = load ", result);
			llvm_emit_type(v->type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint(", ");
			llvm_emit_type(v->type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint("* %%%d", var_temp_reg);
		} break;

		case AST_NODE_BINARY_EXPRESSION: {
			s32 temp_left = -1;
			s32 temp_right = -1;

			// left composite expression
			if (!ast_node_is_embeded_literal(expr->expression.binary_exp.left)) {
				temp_left = llvm_emit_expression(expr->expression.binary_exp.left);
				sprint("\n");
			}
			// right composite expression
			if (!ast_node_is_embeded_literal(expr->expression.binary_exp.right)) {
				temp_right = llvm_emit_expression(expr->expression.binary_exp.right);
				sprint("\n");
			}
			result = alloc_temp_register();

			sprint("%%%d = ", result);
			char* operation = 0;

			if (is_floating_point_type(expr->return_type)) {
				// Floating point type operations
				switch (expr->expression.binary_exp.op) {
					case BINARY_OP_PLUS:	operation = "fadd"; break;
					case BINARY_OP_MINUS:	operation = "fsub"; break;
					case BINARY_OP_MULT:	operation = "fmul"; break;
					case BINARY_OP_DIV:		operation = "fdiv"; break;

					case BINARY_OP_EQUAL_EQUAL:		operation = "fcmp ueq"; break;
					case BINARY_OP_NOT_EQUAL:		operation = "fcmp une"; break;
					case BINARY_OP_GREATER_EQUAL:	operation = "fcmp uge"; break;
					case BINARY_OP_GREATER_THAN:	operation = "fcmp ugt"; break;
					case BINARY_OP_LESS_THAN:		operation = "fcmp ult"; break;
					case BINARY_OP_LESS_EQUAL:		operation = "fcmp ule"; break;
				}
			} else {
				// Integer and pointer arithmetic operations
				bool signed_int_type = is_integer_signed_type(expr->return_type);
				switch (expr->expression.binary_exp.op) {
					case BINARY_OP_PLUS:	operation = "add"; break;
					case BINARY_OP_MINUS:	operation = "sub"; break;
					case BINARY_OP_MULT:	operation = "mul"; break;
					case BINARY_OP_AND:		operation = "and"; break;
					case BINARY_OP_OR:		operation = "or";  break;
					case BINARY_OP_XOR:		operation = "xor"; break;
					case BINARY_OP_DIV:		operation = (signed_int_type) ? "sdiv" : "udiv"; break;
					case BINARY_OP_MOD:		operation = (signed_int_type) ? "srem" : "urem"; break;

					case BINARY_OP_BITSHIFT_LEFT:	operation = "shl"; break;
					case BINARY_OP_BITSHIFT_RIGHT:	operation = (signed_int_type) ? "ashr" : "lshr"; break;

					case BINARY_OP_EQUAL_EQUAL:		operation = "icmp eq"; break;
					case BINARY_OP_NOT_EQUAL:		operation = "icmp ne"; break;
					case BINARY_OP_GREATER_EQUAL:	operation = (signed_int_type) ? "icmp sge" : "icmp uge"; break;
					case BINARY_OP_GREATER_THAN:	operation = (signed_int_type) ? "icmp sgt" : "icmp ugt"; break;
					case BINARY_OP_LESS_THAN:		operation = (signed_int_type) ? "icmp slt" : "icmp ult"; break;
					case BINARY_OP_LESS_EQUAL:		operation = (signed_int_type) ? "icmp sle" : "icmp ule"; break;

					//case BINARY_OP_LOGIC_AND:
					//case BINARY_OP_LOGIC_OR:
				}
			}
			// Operation
			sprint("%s ", operation);
			
			// Left side
			llvm_emit_type(expr->return_type);
			sprint(" ");
			if (temp_left == -1) {
				llvm_emit_expression(expr->expression.binary_exp.left);
			} else {
				sprint("%%%d, ", temp_left);
			}

			sprint(", ");

			// Right side
			if (temp_right == -1) {
				llvm_emit_expression(expr->expression.binary_exp.right);
			} else {
				sprint("%%%d", temp_right);
			}
			
		} break;
	}

	return result;
}

void llvm_generate_ir(Ast** toplevel, Type_Table* type_table) {
	LLVM_Code_Generator code_generator = {};

	code_generator.llvm_emit_type_decls(type_table);

	size_t num_decls = array_get_length(toplevel);
	for (size_t i = 0; i < num_decls; ++i) {
		Ast* node = toplevel[i];
		code_generator.llvm_emit_node(node);
	}

	code_generator.sprint("\n");
	code_generator.sprint("attributes #0 = { nounwind uwtable }\n");
	code_generator.sprint("attributes #1 = { nounwind uwtable }\n");

	HANDLE out = ho_createfile("temp//llvmtest.ll", FILE_WRITE, CREATE_ALWAYS);
	ho_writefile(out, code_generator.ptr, (u8*)code_generator.buffer);
	ho_closefile(out);
}

bool ast_node_is_embeded_literal(Ast* node) {
	return (node->node == AST_NODE_LITERAL_EXPRESSION && node->expression.literal_exp.flags & LITERAL_FLAG_NUMERIC);
}