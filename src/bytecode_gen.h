#pragma once
#include "ast.h"

struct Gen_Environment {
	s64 code_offset;
	s64 stack_size;
	s64 stack_base_offset;
	s64 stack_temp_offset;
};

void bytecode_generate(Ast** top_level);