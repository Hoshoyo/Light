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

void LLVM_Code_Generator::reset_temp() {
	temp = 0;
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
				llvm_emit_node(node->proc_decl.body);
			}
		}break;

		case AST_NODE_BLOCK: {
			size_t num_commands = array_get_length(node->block.commands);
			sprint("{\n");
			for (size_t i = 0; i < num_commands; ++i) {
				sprint("\t");
				llvm_emit_node(node->block.commands[i]);
				sprint("\n");
			}
			sprint("}\n");
			reset_temp();
		} break;

		case AST_NODE_VARIABLE_DECL: {
			u32 temp = alloc_temp();
			node->var_decl.temporary_register = temp;

			sprint("%%%d = alloca ", temp);
			llvm_emit_type(node->var_decl.type, EMIT_TYPE_FLAG_STRUCT_NAMED);
			sprint(", align %d", node->var_decl.alignment);

			// @TODO: assignment if there is
		} break;

		default: {
			if (node->is_expr) {
				llvm_emit_expression(node);
			}
		}break;
	}
}

u32 LLVM_Code_Generator::llvm_emit_expression(Ast* expr) {
	u32 result = -1;

	switch (expr->node) {
		case AST_NODE_PROCEDURE_CALL: {
			// evaluate the arguments first
			size_t num_args = 0;
			if(expr->expression.proc_call.args)
				num_args = array_get_length(expr->expression.proc_call.args);
			// temporary array to hold result register
			s32* arg_temps = array_create(s32, num_args);
			for (size_t i = 0; i < num_args; ++i) {
				Ast* e = expr->expression.proc_call.args[i];
				if (e->node == AST_NODE_LITERAL_EXPRESSION && e->expression.literal_exp.flags & LITERAL_FLAG_NUMERIC) {
					arg_temps[i] = -1;
				} else {
					arg_temps[i] = (s32)llvm_emit_expression(e);
				}
			}

			// alloc the proc call result register
			result = alloc_temp();
			sprint("%%%d = call ", result);
			sprint("@%.*s(", TOKEN_STR(expr->expression.proc_call.name));
			for (size_t i = 0; i < num_args; ++i) {
				if (i != 0) sprint(", ");
				Ast* arg = expr->expression.proc_call.args[i];
				llvm_emit_type(arg->return_type);
				sprint(" ");
				if (arg_temps[i] == -1) {
					llvm_emit_expression(arg);
				} else {
					sprint(" %%%d", arg_temps[i]);
				}
			}
			sprint(")\n");
			array_release(arg_temps);
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