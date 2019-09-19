#pragma once
#include "../../ast.h"
#include "../../global_tables.h"

void backend_c_generate_top_level(Light_Ast** ast, Type_Table type_table, Light_Scope* global_scope, const char* path, const char* filename, const char* compiler_path);
void backend_c_compile_with_gcc(Light_Ast** ast, const char* filename, const char* working_directory);