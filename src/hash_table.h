#pragma once
#include "hash_table.h"
#include "ho_system.h"
#include "util.h"

typedef u64 hash_function_type(void* data);
typedef bool hash_entries_equal_type(void* e1, void* e2);

struct Hash_Table {
	s64 entries_count;
	s64 entries_capacity;
	s64 hash_collision_count;
	struct Entry {
		u64 hash;
		void* data;
		s32 data_size;
		bool occupied;
		bool collided;
	}*entries;

	hash_function_type*			hash_function;
	hash_entries_equal_type*	entries_equal;
};

void hash_table_init(Hash_Table* table, s64 max_entries, hash_function_type* hash_function, hash_entries_equal_type* compare_entries);
void hash_table_release(Hash_Table* table);

void hash_table_add(Hash_Table* table, void* data, s64 data_size_bytes);
s64  hash_table_entry_exist(Hash_Table* table, void* data);
void hash_table_remove(Hash_Table* table, void* data);
void* hash_table_get_entry(Hash_Table* table, s64 index);

// string specialized hash table
struct String_Hash_Table {
	s64 entries_count;
	s64 entries_capacity;
	s64 hash_collision_count;
	struct Entry {
		u64 hash;
		string data;
		bool occupied;
		bool collided;
	}*entries;
};
void hash_table_init(String_Hash_Table* table, s64 max_entries);

void hash_table_add(String_Hash_Table* table, string s);
s64  hash_table_entry_exist(String_Hash_Table* table, string s);
void hash_table_remove(String_Hash_Table* table, string s);
string hash_table_get_string_entry(String_Hash_Table* table, s64 index);