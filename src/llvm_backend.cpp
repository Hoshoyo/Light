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
			if (node->proc_decl.flags & PROC_DECL_FLAG_IS_EXTERNAL) {
				sprint("declare cc 64 ");
			} else {
				sprint("define ");
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
				sprint("#1 {\ndecls-0:\n");
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

			if (node->var_decl.assignment) {
				sprint("\n");
				llvm_emit_assignment(node);
			}
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
			sprint("\n");
		} break;

		case AST_NODE_IF_STATEMENT: {
			// TODO(psv): need a few more jumps
			s32 true_label  = gen_branch_label();
			s32 false_label = (node->if_stmt.else_exp) ? gen_branch_label() : -1;
			s32 end_label = true_label;

			if (ast_node_is_embeded_literal(node->if_stmt.bool_exp)) {
				sprint("br i1 ");
				llvm_emit_expression(node->if_stmt.bool_exp);			
			} else {
				s32 boolexp_temp = llvm_emit_expression(node->if_stmt.bool_exp);
				sprint("\nbr i1 %%%d", boolexp_temp);
			}
			if (false_label != -1)
				sprint(", label %%if-stmt-%d, label %%if-stmt-%d\n", true_label, false_label);
			else
				sprint(", label %%if-stmt-%d, label %%if-end-%d\n", true_label, end_label);
			sprint("if-stmt-%d:\n", true_label);
			llvm_emit_node(node->if_stmt.body);
			sprint("br label %%if-end-%d\n", end_label);
			if(false_label != -1)
				sprint("if-stmt-%d:", false_label);
			if (node->if_stmt.else_exp) {
				llvm_emit_node(node->if_stmt.else_exp);
				sprint("br label %%if-end-%d\n", end_label);
			}
			sprint("if-end-%d:\n", end_label);
		}break;

		default: {
			if (node->is_expr) {
				llvm_emit_expression(node);
				sprint("\n");
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
			
			if (expr->expression.proc_call.args)
				num_args = array_get_length(expr->expression.proc_call.args);

			if (num_args) {
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
			if (expr->return_type->type == TYPE_PRIMITIVE && expr->return_type->primitive == TYPE_PRIMITIVE_VOID) {
				sprint("call ");
			} else {
				result = alloc_temp_register();
				sprint("%%%d = call ", result);
			}
			llvm_emit_type(expr->return_type);
			sprint(" @%.*s(", TOKEN_STR(expr->expression.proc_call.name));
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
				case TOKEN_STRING_LITERAL: {
					result = alloc_temp_register();
					s32 index = llvm_define_string_literal(l);
					size_t len = l->lit_tok->value.length + 1; // \0
					sprint("%%%d = getelementptr [%d x i8], [%d x i8]* @__str$%d, i64 0, i64 0\n", result, len, len, index);
				}break;
			}
		} break;

		case AST_NODE_VARIABLE_EXPRESSION: {
			Ast* var_decl = get_declaration_from_variable_expression(expr);

			// @TODO IMPORTANT variable expression node doesnt get type from type inference pass
			
			assert(var_decl->node == AST_NODE_VARIABLE_DECL);
			//assert(expr->return_type);
			// @TODO HACK, return_type should be set earlier
			if (!expr->return_type) {
				expr->return_type = var_decl->var_decl.type;
			}

			// get temporary register containing the variable address
			s32 var_temp_reg = var_decl->var_decl.temporary_register;
			
			result = alloc_temp_register();

			sprint("%%%d = load ", result);
			llvm_emit_type(expr->return_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint(", ");
			llvm_emit_type(expr->return_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
			sprint("* %%%d", var_temp_reg);
		} break;

		case AST_NODE_BINARY_EXPRESSION: {
			if (expr_is_assignment(&expr->expression)) {
				result = llvm_emit_assignment(expr);
			} else {
				result = llvm_emit_binary_expression(expr);
			}
		} break;
	}

	return result;
}

s32 LLVM_Code_Generator::llvm_emit_binary_expression(Ast* expr) {
	assert(expr->node == AST_NODE_BINARY_EXPRESSION);

	BinaryOperation op = expr->expression.binary_exp.op;
	Type_Instance* left_type  = expr->expression.binary_exp.left->return_type;
	Type_Instance* right_type = expr->expression.binary_exp.right->return_type;

	s32 t_left = -1;
	s32 t_right = -1;
	if (!ast_node_is_embeded_literal(expr->expression.binary_exp.left)) {
		t_left = llvm_emit_expression(expr->expression.binary_exp.left);
		sprint("\n");
	}
	if (!ast_node_is_embeded_literal(expr->expression.binary_exp.right)) {
		t_right = llvm_emit_expression(expr->expression.binary_exp.right);
		sprint("\n");
	}
	s32 temp_reg = alloc_temp_register();
	sprint("%%%d = ", temp_reg);

	// If it is a floating point type llvm offers operations like
	// fcmp, fadd, fsub, ...
	if (is_floating_point_type(left_type) && is_floating_point_type(right_type)) {
		switch (op) {
			case BINARY_OP_PLUS:		sprint("fadd "); break;
			case BINARY_OP_MINUS:		sprint("fsub "); break;
			case BINARY_OP_MULT:		sprint("fmul "); break;
			case BINARY_OP_DIV:			sprint("fdiv "); break;

			case BINARY_OP_EQUAL_EQUAL:		sprint("fcmp oeq "); break;
			case BINARY_OP_GREATER_EQUAL:	sprint("fcmp oge "); break;
			case BINARY_OP_GREATER_THAN:	sprint("fcmp ogt "); break;
			case BINARY_OP_LESS_EQUAL:		sprint("fcmp ole "); break;
			case BINARY_OP_LESS_THAN:		sprint("fcmp olt "); break;
		}
		// float or double always
		llvm_emit_type(left_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
		sprint(" ");
		if (t_left == -1) {
			llvm_emit_expression(expr->expression.binary_exp.left);
			sprint(", ");
		} else {
			sprint("%%%d, ", t_left);
		}

		if (t_right == -1) {
			llvm_emit_expression(expr->expression.binary_exp.right);
		} else {
			sprint("%%%d", t_right);
		}

	} else {
		// is integer type or pointer arithmetic
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
	if (expr->node == AST_NODE_BINARY_EXPRESSION) {
		left = expr->expression.binary_exp.left;
		right = expr->expression.binary_exp.right;
		var_decl = get_declaration_from_variable_expression(left);
		node_type = left->return_type;
	} else if(expr->node == AST_NODE_VARIABLE_DECL) {
		right = expr->var_decl.assignment;
		var_decl = expr;
		node_type = var_decl->var_decl.type;
	}

	s32 temp = var_decl->var_decl.temporary_register;
	
	if (!ast_node_is_embeded_literal(right)) {
		switch (right->return_type->type) {
			case TYPE_STRUCT: {
				if (right->node == AST_NODE_LITERAL_EXPRESSION && right->expression.literal_exp.flags & LITERAL_FLAG_STRING) {
					s32 str_index = llvm_define_string_literal(&right->expression.literal_exp);
					s32 temp_char_ptr = alloc_temp_register();
					size_t length = right->expression.literal_exp.lit_tok->value.length + 1;
					// get char array ptr to put on data field of string
					sprint("%%%d = getelementptr [%lld x i8], [%lld x i8]* @__str$%d, i64 0, i64 0\n", temp_char_ptr, length, length, str_index);
					s32 length_reg = alloc_temp_register();
					s32 data_reg   = alloc_temp_register();
					sprint("%%%d = getelementptr %%string, %%string* %%%d, i64 0, i32 0\n", length_reg, temp);
					sprint("%%%d = getelementptr %%string, %%string* %%%d, i64 0, i32 1\n", data_reg, temp);
					sprint("store i64 %lld, i64* %%%d\n", length, length_reg);
					sprint("store i8* %%%d, i8** %%%d\n", temp_char_ptr, data_reg);

				} else if (right->node == AST_NODE_VARIABLE_EXPRESSION) {
					int x = 0;
				} else {
					// @TODO(psv): other kinds of literals
					report_fatal_error("other literals are not yet supported\n");
				}
			}break;

			case TYPE_POINTER:
			case TYPE_FUNCTION:
			case TYPE_ARRAY:
			case TYPE_UNKNOWN: {
				report_fatal_error("type not supported or unknown\n");
			}break;
			case TYPE_PRIMITIVE: {
				s32 result = llvm_emit_expression(right);
				sprint("\nstore ");
				llvm_emit_type(right->return_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
				sprint(" %%%d, ", result);
				llvm_emit_type(right->return_type, EMIT_TYPE_FLAG_STRUCT_NAMED | EMIT_TYPE_FLAG_CONVERT_VOID_TO_I8);
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

s32 LLVM_Code_Generator::llvm_define_string_literal(Ast_Literal* lit) {
	s32 n = alloc_strlit_temp();

	size_t litlength = lit->lit_tok->value.length;
	lit->llvm_index = n;
	// @TODO(psv): temporary print of literal string
	sprint_strlit("@__str$%d = private global [%d x i8] c\"%.*s\\00\"\n", n, litlength + 1, TOKEN_STR(lit->lit_tok));
	return n;
}

bool ast_node_is_embeded_literal(Ast* node) {
	if (node->return_type->type != TYPE_PRIMITIVE) return false;
	return (node->node == AST_NODE_LITERAL_EXPRESSION && node->expression.literal_exp.flags & LITERAL_FLAG_NUMERIC);
}











static size_t filename_length_strip_extension(char* f) {
	// Extract the base filename without the extension
	size_t baselen = strlen(f);
	size_t startlen = baselen;
	while (f[baselen] != '.') baselen--;
	if (baselen == 0) baselen = startlen;
	return baselen;
}

// @TEMPORARY Windows only for now
void llvm_generate_ir(Ast** toplevel, Type_Table* type_table, char* filename) {
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

	size_t fname_len = filename_length_strip_extension(filename);
	string out_obj(fname_len + sizeof(".ll"), fname_len, filename);
	out_obj.cat(".ll", sizeof(".ll"));
	
	HANDLE out = ho_createfile(out_obj.data, FILE_WRITE, CREATE_ALWAYS);
	ho_writefile(out, code_generator.ptr, (u8*)code_generator.buffer);
	ho_writefile(out, code_generator.strlit_ptr, (u8*)code_generator.strlit_buffer);
	ho_closefile(out);

	char cmdbuffer[1024];
	sprintf(cmdbuffer, "llc -filetype=obj -march=x86-64 %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj -nostdlib -o %.*s.exe lib/kernel32.lib lib/msvcrt.lib", fname_len, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
}


void LLVM_Code_Generator::llvm_generate_temporary_code() {
#if 0
	char temp_code[] = R"(
define void @print_int(i32 %value) #0 {
	%str = getelementptr [3 x i8], [3 x i8]* @__str$3, i64 0, i64 0
	%1 = call i32 (i8*, ...) @printf(i8* %str, i32 %value)
	ret void
}

define void @print_double(double %value) #0 {
	%str = getelementptr [3 x i8], [3 x i8]* @__str$4, i64 0, i64 0
	%1 = call i32 (i8*, ...) @printf(i8* %str, double %value)
	ret void
}
declare cc 64 i32 @ExitProcess(i32) #0
declare cc 64 i8* @GetStdHandle(i32) #1
declare cc 64 i32 @WriteConsoleA(i8*, i8*, i32, i32*, i64) #1
)";

	char entrypoint[] = R"(
section .data
section .text

extern main

global _start
_start:
	call main
	mov ebx, eax
	mov eax, 1
	int 80h
	)";
	FILE* entry_file = fopen("temp/entry.asm", "w");
	size_t written = fwrite(entrypoint, sizeof(entrypoint), 1, entry_file);
	fclose(entry_file);

	// @TODO linux only
	system("nasm -g -f elf64 temp/entry.asm -o temp/entry.o");
	//system("ld ")
#endif	
}