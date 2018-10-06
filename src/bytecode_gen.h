#pragma once
#include "ast.h"
#include "interpreter.h"

struct Gen_Proc_Addresses {
	u64* fill_address;
	Ast*  decl;
};

struct Gen_Environment {
	Interpreter* interp;

	u8* code;

	s64 code_offset;
	s64 stack_size;
	s64 stack_base_offset;
	s64 stack_temp_offset;

	Gen_Proc_Addresses* proc_addressing_queue;
};

void bytecode_generate(Interpreter* interp, Ast** top_level);