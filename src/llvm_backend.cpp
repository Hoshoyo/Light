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

u32 LLVM_Code_Generator::alloc_temp() {
	return temp++;
}

void report_fatal_error(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	//assert(0);
	exit(-1);
}

void llvm_generate_ir(Ast** toplevel) {
	LLVM_Code_Generator code_generator = {};
	//
	// @TEMPORARY
	// @TEMPORARY
	// @TEMPORARY
	//
	{
		code_generator.sprint("declare cc 64 i32 @ExitProcess(i32) #0\n");
	}

	size_t num_decls = get_arr_length(toplevel);
	for (size_t i = 0; i < num_decls; ++i) {
		Ast* node = toplevel[i];

		llvm_emit_ir_for_node(&code_generator, node);
	}

	llvm_emit_function_attributes(&code_generator);

	HANDLE out = ho_createfile("temp//llvmtest.ll", FILE_WRITE, CREATE_ALWAYS);
	ho_writefile(out, code_generator.ptr, (u8*)code_generator.buffer);
	ho_closefile(out);
}

void llvm_emit_ir_for_type(LLVM_Code_Generator* cg, Type_Instance* type, bool convert_void_to_i8 /*= false */) {
	switch (type->type) {
		case TYPE_PRIMITIVE: {
			switch(type->primitive)
			{
				case TYPE_PRIMITIVE_U64:
				case TYPE_PRIMITIVE_S64:	cg->sprint("i64"); break;
				case TYPE_PRIMITIVE_U32:
				case TYPE_PRIMITIVE_S32:	cg->sprint("i32"); break;
				case TYPE_PRIMITIVE_U16:
				case TYPE_PRIMITIVE_S16:	cg->sprint("i16"); break;
				case TYPE_PRIMITIVE_U8:
				case TYPE_PRIMITIVE_S8:		cg->sprint("i8"); break;
				case TYPE_PRIMITIVE_BOOL:	cg->sprint("i8"); break;
				case TYPE_PRIMITIVE_R64:	cg->sprint("double"); break;
				case TYPE_PRIMITIVE_R32:	cg->sprint("float"); break;
				case TYPE_PRIMITIVE_VOID:	(convert_void_to_i8) ? cg->sprint("i8") : cg->sprint("void"); break;
				default: assert(0); break;	// @TODO log
			}
		}break;
		case TYPE_POINTER: {
			llvm_emit_ir_for_type(cg, type->pointer_to, true);
			cg->sprint("*");
		} break;
		default: {
			assert(0); //@TODO
		}break;
	}
}

u32 llvm_emit_ir_for_expression(LLVM_Code_Generator* cg, Ast* expr) {
	u32 result_temp = 0;
	switch (expr->node) {
		case AST_NODE_VARIABLE_EXPRESSION: {
			Ast* decl_node = is_declared(expr);
			if (decl_node) {
				result_temp = cg->alloc_temp();
				cg->sprint(" %%%d = load ", result_temp);
				llvm_emit_ir_for_type(cg, expr->return_type, true);
				cg->sprint(", ");
				llvm_emit_ir_for_type(cg, expr->return_type, true);
				cg->sprint("* %%%d\n", decl_node->var_decl.temporary_register);
			} else {
				report_fatal_error("Internal compiler error: llvm emit expression got a variable that is not declared somehow.\n");
			}
		}break;

		case AST_NODE_BINARY_EXPRESSION: {
			Ast* left  = expr->expression.binary_exp.left;
			Ast* right = expr->expression.binary_exp.right;
			bool left_reg_size = left->node == AST_NODE_LITERAL_EXPRESSION;
			bool right_reg_size = right->node == AST_NODE_LITERAL_EXPRESSION;

			u32 left_temp = 0;
			u32 right_temp = 0;

			if (!left_reg_size) {
				left_temp = llvm_emit_ir_for_expression(cg, left);
			}
			if (!right_reg_size) {
				right_temp = llvm_emit_ir_for_expression(cg, right);
			}

			result_temp = cg->alloc_temp();
			cg->sprint(" %%%d = ", result_temp);
			
			int int_type = is_integer_type(expr->return_type);
			if (int_type) {
				char* operation_str = 0;
				switch (expr->expression.binary_exp.op) {
					case BINARY_OP_PLUS:  operation_str = "add "; break;
					case BINARY_OP_MINUS: operation_str = "sub "; break;
					case BINARY_OP_MULT:  operation_str = "mul "; break;
					case BINARY_OP_DIV:   operation_str = (int_type == INTEGER_SIGNED) ? "sdiv " : "udiv "; break;
					case BINARY_OP_MOD:   operation_str = (int_type == INTEGER_SIGNED) ? "srem " : "urem "; break;
					case BINARY_OP_AND:   operation_str = "and "; break;
					case BINARY_OP_OR:    operation_str = "or "; break;
					case BINARY_OP_XOR:   operation_str = "xor "; break;
				}
				cg->sprint(operation_str);
			} else if(is_floating_point_type(expr->return_type)) {
				switch (expr->expression.binary_exp.op) {
					case BINARY_OP_PLUS:  cg->sprint("fadd "); break;
					case BINARY_OP_MINUS: cg->sprint("fsub "); break;
					case BINARY_OP_MULT:  cg->sprint("fmul "); break;
					case BINARY_OP_DIV:   cg->sprint("fdiv "); break;
					//case BINARY_OP_MOD:   cg->sprint("frem "); break;
				}
			}

			llvm_emit_ir_for_type(cg, left->return_type);

			if (left->node == AST_NODE_LITERAL_EXPRESSION) {
				cg->sprint(" ");
				llvm_emit_ir_for_literal(cg, &left->expression.literal_exp);
			} else
				cg->sprint(" %%%d", left_temp);

			cg->sprint(", ");

			if (right->node == AST_NODE_LITERAL_EXPRESSION) {
				llvm_emit_ir_for_literal(cg, &right->expression.literal_exp);
			} else
				cg->sprint("%%%d", right_temp);

			cg->sprint("\n");

		}break;

		case AST_NODE_LITERAL_EXPRESSION: {
			assert(0);	// should not try to load literal in a temp variable
		}break;
		default: assert(0); break; // @TODO
	}
	return result_temp;
}

void llvm_emit_ir_for_node(LLVM_Code_Generator* cg, Ast* node)
{
	switch (node->node) {
		case AST_NODE_PROC_DECLARATION: {
			// @TODO(psv): replace with the calling convention
			if (node->proc_decl.flags & PROC_DECL_FLAG_IS_EXTERNAL) {
				cg->sprint("declare cc 64");
			} else {
				cg->sprint("define ");
			}

			llvm_emit_ir_for_type(cg, node->proc_decl.proc_ret_type);

			// @TODO unique name for nested procedures
			cg->sprint(" @%.*s(", TOKEN_STR(node->proc_decl.name));

			for (size_t i = 0; i < node->proc_decl.num_args; ++i) {
				Ast* arg = node->proc_decl.arguments[i];
				assert(arg->node == AST_NODE_NAMED_ARGUMENT);
				llvm_emit_ir_for_type(cg, arg->named_arg.arg_type);

				if (!(node->proc_decl.flags & PROC_DECL_FLAG_IS_EXTERNAL)) {
					cg->sprint(" %%%.*s", TOKEN_STR(arg->named_arg.arg_name));
				}

				if (i + 1 < node->proc_decl.num_args)
					cg->sprint(", ");
			}

			// attributes for normal function
			// @TODO noreturn for entry point
			cg->sprint(") #0\n");

			if (node->proc_decl.body) {
				cg->sprint("{\n");
				cg->sprint("decls-0:\n");
				llvm_emit_ir_for_node(cg, node->proc_decl.body);
				cg->sprint("}\n");
			}
		}break;

		case AST_NODE_BLOCK: {
			size_t num_commands = get_arr_length(node->block.commands);
			for (size_t i = 0; i < num_commands; ++i) {
				llvm_emit_ir_for_node(cg, node->block.commands[i]);
			}
		}break;

		case AST_NODE_VARIABLE_DECL: {
			u32 temp = cg->alloc_temp();
			node->var_decl.temporary_register = temp;
			cg->sprint(" %%%d = alloca ", temp);
			llvm_emit_ir_for_type(cg, node->var_decl.type);
			cg->sprint(", align %d\n", node->var_decl.alignment);

			// has default parameters
			if (node->var_decl.assignment) {
				llvm_instr_emit_store(cg, node->var_decl.type, node->var_decl.assignment, node->var_decl.temporary_register);
			} else {
				llvm_instr_emit_store(cg, node->var_decl.type, 0, node->var_decl.temporary_register);
			}
		}break;

		case AST_NODE_RETURN_STATEMENT: {
			bool direct_literal_return = false;
			u32 temp = 0;
			if (node->ret_stmt.expr->node != AST_NODE_LITERAL_EXPRESSION) {
				temp = llvm_emit_ir_for_expression(cg, node->ret_stmt.expr);
			} else {
				direct_literal_return = true;
			}

			//
			// @TEMPORARY
			// @TEMPORARY
			// @TEMPORARY
			//
			{
				cg->sprint(" call cc 64 i32 @ExitProcess(i32 ");
				if (direct_literal_return)
					llvm_emit_ir_for_literal(cg, &node->ret_stmt.expr->expression.literal_exp);
				else
					cg->sprint("%%%d", temp);
				cg->sprint(")\n");
			}

			cg->sprint(" ret ");
			llvm_emit_ir_for_type(cg, node->ret_stmt.expr->return_type, true);
			if (direct_literal_return) {
				cg->sprint(" ");
				llvm_emit_ir_for_literal(cg, &node->ret_stmt.expr->expression.literal_exp);
				cg->sprint("\n");
			} else
				cg->sprint(" %%%d\n", temp);

		}break;

		default: {
			
			//assert(0);
		} break;
	}
}

void llvm_emit_function_attributes(LLVM_Code_Generator* cg) {
	cg->sprint("attributes #0 = {nounwind uwtable}");
}

void llvm_emit_ir_for_literal(LLVM_Code_Generator* cg, Ast_Literal* lit_exp) {
	// @TODO(psv): substitute literal token for more sofisticated print
	cg->sprint("%.*s", TOKEN_STR(lit_exp->lit_tok));
}

void llvm_instr_emit_store(LLVM_Code_Generator* cg, Type_Instance* type_variable, Ast* value, u32 temp_reg) {
	if (!value) {
		cg->sprint(" store ");
		llvm_emit_ir_for_type(cg, type_variable, true);
		cg->sprint(" zeroinitializer, ");
		llvm_emit_ir_for_type(cg, type_variable, true);
		cg->sprint("* %%%d\n", temp_reg);
		return;
	}

	if (ast_is_expression(value)) {
		if (value->expression.expression_type == EXPRESSION_TYPE_LITERAL) {
			cg->sprint(" store ");
			llvm_emit_ir_for_type(cg, type_variable, true);
			cg->sprint(" ");
			llvm_emit_ir_for_literal(cg, &value->expression.literal_exp);
			cg->sprint(", ");
			llvm_emit_ir_for_type(cg, type_variable, true);
			cg->sprint("* %%%d\n", temp_reg);
		} else if (value->expression.expression_type == EXPRESSION_TYPE_VARIABLE) {
			Ast* decl_node = is_declared(value);
			if (decl_node) {
				u32 load_temp = cg->alloc_temp();
				cg->sprint(" %%%d = load ", load_temp);
				llvm_emit_ir_for_type(cg, type_variable, true);
				cg->sprint(", ");
				llvm_emit_ir_for_type(cg, type_variable, true);
				cg->sprint("* %%%d\n", decl_node->var_decl.temporary_register);

				cg->sprint(" store ");
				llvm_emit_ir_for_type(cg, decl_node->var_decl.type);
				cg->sprint(" %%%d, ", load_temp);
				llvm_emit_ir_for_type(cg, decl_node->var_decl.type);
				cg->sprint("* %%%d\n", temp_reg);
			} else {
				report_fatal_error("Internal compiler error: llvm code generator tried to use a not declared variable.\n");
			}
		} else {
			u32 temp_exp = llvm_emit_ir_for_expression(cg, value);
			cg->sprint(" store ");
			llvm_emit_ir_for_type(cg, type_variable, true);
			cg->sprint(" %%%d, ", temp_exp);
			llvm_emit_ir_for_type(cg, type_variable);
			cg->sprint("* %%%d\n", temp_reg);
		}
	}
}