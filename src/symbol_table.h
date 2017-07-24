#pragma once
#include <ho_system.h>
#include "ast.h"

struct Symbol_Entry {
	Token* identifier;
	Ast* node;
	Scope* scope;
	bool used;
	bool collided;
};

struct Symbol_Table{
	Symbol_Entry* entries;
	int num_entries;
	int max_entries;
	int num_collisions;

	Symbol_Table(s32 size);

	u32 hash_of_ident(Token* id);
	s64 entry_exist(Token* ident);
	s64 insert(Scope* scope, Token* ident, Ast* node);
};