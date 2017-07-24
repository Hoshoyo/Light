#include "symbol_table.h"

Symbol_Table::Symbol_Table(s32 size) {
	entries = (Symbol_Entry*)calloc(1, size * sizeof(Symbol_Entry));
	max_entries = size;
	num_entries = 0;
	num_collisions = 0;
}

u32 Symbol_Table::hash_of_ident(Token* id) {
	u32 hash = djb2_hash((u8*)id->value.data, id->value.length) % max_entries;
	return hash;
}

s64 Symbol_Table::entry_exist(Token* ident) {
	u32 hash = hash_of_ident(ident);
	if (entries[hash].used) {
		if (str_equal(ident->value.data, ident->value.length, entries[hash].identifier->value.data, entries[hash].identifier->value.length)) {
			return (s64)hash;
		}
		else {
			while (entries[hash].collided) {
				hash += 1;
				if (hash >= max_entries) hash = 0;
				if (str_equal(ident->value.data, ident->value.length, entries[hash].identifier->value.data, entries[hash].identifier->value.length)) {
					return (s64)hash;
				}
			}
		}
	}
	return -1;
}

s64 Symbol_Table::insert(Scope* scope, Token* ident, Ast* node) {
	assert(num_entries < max_entries);

	u32 hash = hash_of_ident(ident);
	bool collided = false;
	if (!entries[hash].used) {
		int entry_size = 0;
		while (entries[hash].used) {
			collided = true;
			hash += 1;
			num_collisions += 1;
			if (hash >= max_entries) hash = 0;
		}
	}
	entries[hash].used = true;
	entries[hash].collided = collided;
	entries[hash].identifier = ident;
	entries[hash].node = node;
	entries[hash].scope = scope;

	num_entries += 1;
	return hash;
}
