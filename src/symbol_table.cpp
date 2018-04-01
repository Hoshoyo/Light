#include "symbol_table.h"
#include "lexer.h"

void symbol_table_init(Symbol_Table* table, s64 max_entries) {
	table->entries = (Symbol_Table::Entry*)calloc(max_entries, sizeof(Symbol_Table::Entry));
	table->entries_capacity = max_entries;
	table->entries_count = 0;
}

// Assumes it was already check for the existance of the entry
u64 symbol_table_add(Symbol_Table* table, Token* t, Ast* decl_node) {
	u64 index = (u64)t->value.data % table->entries_capacity;

	while (table->entries[index].occupied) {
		++index;
		if (index == table->entries_capacity)
			index = 0;
	}

	table->entries[index].identifier = t;
	table->entries[index].decl_node = decl_node;
	table->entries[index].occupied = true;
	table->entries_count += 1;
	return index;
}

s64 symbol_table_entry_exist(Symbol_Table* table, Token* t) {
	u64 index = (u64)t->value.data % table->entries_capacity;
	u64 start = index;
	while (table->entries[index].occupied) {
		if (table->entries[index].identifier->value.data == t->value.data)
			return (s64)index;
		index++;
		if (index == table->entries_capacity) index = 0;
		if (index == start) return -1;
	}
	return -1;
}

void symbol_table_remove(Symbol_Table* table, Token* t) {
	u64 index = (u64)t->value.data % table->entries_capacity;
	if (table->entries[index].occupied &&
		table->entries[index].identifier->value.data == t->value.data) {
		table->entries->occupied = false;
	}
}

Token* symbol_table_get_entry(Symbol_Table* table, u64 index) {
	return table->entries[index].identifier;
}

void symbol_table_release(Symbol_Table* table) {
	free(table->entries);
	table->entries_count = 0;
	table->entries_capacity = 0;
}