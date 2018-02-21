#pragma once
#include "ast.h"

int generate_interpreter_code(Ast** ast);


#ifdef _WIN64
// @TODO linux version
HMODULE load_library_dynamic(string* library);
void free_library(HMODULE lib);
// @TODO linux version
// @TODO OPTIMIZE, load library once for all external calls
void* load_address_of_external_function(string* name, HMODULE library);
#else
//#error "os not supported"
#endif
