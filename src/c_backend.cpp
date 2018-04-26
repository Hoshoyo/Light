#include "c_backend.h"
#include <stdarg.h>

static void report_fatal_error(char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	//assert(0);
	exit(-1);
}

int C_Code_Generator::sprint(char* msg, ...) {
#define DEBUG 0
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

void C_Code_Generator::emit_type(Type_Instance* type, Token* name){
    assert_msg(type->flags & TYPE_FLAG_INTERNALIZED, "tried to emit a type that is not internalized");
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
            emit_type(type->array_desc.array_of);
            sprint("*");
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
            if(name){
                sprint("%.*s", TOKEN_STR(name));
            } else {
                sprint("%.*s", TOKEN_STR(type->struct_desc.name));
            }
        }break;
    }
}

void C_Code_Generator::emit_decl(Ast* decl) {
    assert(decl->flags & AST_FLAG_IS_DECLARATION);
    switch(decl->node_type) {
        case AST_DECL_PROCEDURE: {
            if(decl->decl_procedure.type_return->kind == KIND_FUNCTION){
                Type_Instance* typeproc = decl->decl_procedure.type_procedure;

                sprint("typedef ");
                emit_type(typeproc->function_desc.return_type);
                sprint("__ret_%.*s(", decl->decl_procedure.name);
                for(s32 i = 0; i < typeproc->function_desc.num_arguments; ++i){
                    if(i != 0) sprint(", ");
                    emit_type(typeproc->function_desc.arguments_type[i]);
                }
                sprint(";\n");
                sprint("__ret_%.*s", decl->decl_procedure.name);
            } else {
                emit_type(decl->decl_procedure.type_return);
            }
            sprint(" %.*s(", TOKEN_STR(decl->decl_procedure.name));
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
                emit_type(decl->decl_variable.variable_type);
                sprint(" %.*s", TOKEN_STR(decl->decl_variable.name));
            }
        }break;
        case AST_DECL_CONSTANT:{
            sprint("static const ");
            emit_type(decl->decl_constant.type_info);
            sprint("%.*s", TOKEN_STR(decl->decl_constant.name));
        }break;
        case AST_DECL_STRUCT:{
            sprint("typedef struct {\n");
            size_t nfields = decl->decl_struct.fields_count;
            for(size_t i = 0; nfields; ++i){
                emit_decl(decl->decl_struct.fields[i]);
            }
            sprint("} %.*s", decl->decl_struct.name);
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
				emit_command(comm->comm_block.commands[i]);
			}
			sprint("}\n");
		}break;
		case AST_COMMAND_BREAK: {
			sprint("break;\n");
		}break;
		case AST_COMMAND_CONTINUE: {

		}break;
		case AST_COMMAND_FOR:
		case AST_COMMAND_IF:
		case AST_COMMAND_RETURN:
		case AST_COMMAND_VARIABLE_ASSIGNMENT:
			break;
	}
}

void C_Code_Generator::emit_proc(Ast* decl) {
	if (decl->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN) return;

	emit_decl(decl);
	
	// body
	emit_command(decl->decl_procedure.body);
	sprint("\n");
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
    sprint("\n\n");

    size_t ndecls = array_get_length(toplevel);
    // emit all declarations forward
    for(size_t i = 0; i < ndecls; ++i){
        Ast* decl = toplevel[i];
        emit_decl(decl);
        sprint(";\n");
    }

	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_PROCEDURE) {
			emit_proc(decl);
		}
	}

	// Global variable initialization
	sprint("\nvoid __entry() {\n");
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* decl = toplevel[i];
		if (decl->node_type == AST_DECL_VARIABLE) {
			sprint("\t%.*s = ", TOKEN_STR(decl->decl_variable.name));
			if (decl->decl_variable.assignment) {
				// emit expression
				assert(0);
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
	sprint("\n\tmain();\n");
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
	sprintf(cmdbuffer, "gcc -c -g %s -o %.*s.obj", out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
	sprintf(cmdbuffer, "ld %.*s.obj examples/print_string.obj -e__entry -nostdlib -o %.*s.exe lib/kernel32.lib lib/msvcrt.lib",
		fname_len, out_obj.data, fname_len, out_obj.data);
	system(cmdbuffer);
}