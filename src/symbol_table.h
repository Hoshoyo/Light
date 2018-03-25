#pragma once
#include <ho_system.h>
#include "util.h"

struct Token;
struct Ast;

struct Symbol_Table {
	s64 entries_count;
	s64 entries_capacity;
	struct Entry {
		Token* identifier;
		Ast*   decl_node;
		bool   occupied;
	}*entries;
};

void   symbol_table_init(Symbol_Table* table, s64 max_entries);
u64    symbol_table_add(Symbol_Table* table, Token* t, Ast* decl_node);
s64    symbol_table_entry_exist(Symbol_Table* table, Token* t);
void   symbol_table_remove(Symbol_Table* table, Token* t);
Token* symbol_table_get_entry(Symbol_Table* table, u64 index);
void   symbol_table_release(Symbol_Table* table);