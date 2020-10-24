#pragma once
#include "../ast.h"

Light_Ast** light_lex_parse_string(const char* str, int length_bytes);
Light_Ast** light_typeinfer_check(Light_Ast** ast);