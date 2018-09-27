#include "hash_table.h"
#include "ho_system.h"
#include "util.h"

void hash_table_init(Hash_Table* table, s64 max_entries, hash_function_type* hash_function, hash_entries_equal_type* compare_entries) {
	//table->entries = (Hash_Table::Entry*)calloc(1, max_entries * sizeof(Hash_Table::Entry));
	table->entries = (Hash_Table::Entry*)ho_bigalloc_rw(max_entries * sizeof(Hash_Table::Entry));
	table->hash_function = hash_function;
	table->entries_equal = compare_entries;
	table->entries_capacity = max_entries;
	table->entries_count = 0;
	table->hash_collision_count = 0;
}

void hash_table_release(Hash_Table* table) {
	//free(table->entries);
	ho_bigfree(table->entries, table->entries_capacity * sizeof(Hash_Table::Entry));
	table->entries_count = 0;
	table->entries_capacity = 0;
	table->hash_collision_count = 0;
}

s64 hash_table_add(Hash_Table* table, void* data, s64 data_size_bytes, u64 hash) {
	u64 index = hash % table->entries_capacity;

	bool collided = false;

	while (table->entries[index].occupied) {
		if (table->entries[index].hash == hash) {
			// colisao entre hash's
			table->entries[index].collided = true;
			table->hash_collision_count++;
		}

		++index;
		if (index == table->entries_capacity)
			index = 0;
	}

	table->entries[index].hash = hash;
	table->entries[index].data = data;
	table->entries[index].data_size = data_size_bytes;
	table->entries[index].occupied = true;
	table->entries[index].collided = collided;
	table->entries_count += 1;

	table->entries_count++;
	return index;
}

s64 hash_table_add(Hash_Table* table, void* data, s64 data_size_bytes) {
	u64 hash = table->hash_function(data);
	return hash_table_add(table, data, data_size_bytes, hash);
}

s64 hash_table_entry_exist(Hash_Table* table, void* data, u64 hash) {
	u64 index = hash % table->entries_capacity;

	while (table->entries[index].occupied) {
		if (table->entries[index].hash == hash) {
			if (!table->entries[index].collided) {
				return index;
			} else {
				//Hash_Table::Entry entry = { hash, s, length, false, true };
				if (table->entries && table->entries_equal(&table->entries[index], data)) {
					return index;
				}
				else {
					return -1;
				}
			}
		}

		++index;
		if (index == table->entries_capacity)
			index = 0;
	}
	return -1;
}

s64 hash_table_entry_exist(Hash_Table* table, void* data) {
	u64 hash = table->hash_function(data);
	return hash_table_entry_exist(table, data, hash);
}

void hash_table_remove(Hash_Table* table, void* data) {
	s64 index = hash_table_entry_exist(table, data);
	if (index != -1) {
		table->entries[index].occupied = false;
		table->entries[index].collided = false;
		table->entries_count -= 1;
	}
}

void* hash_table_get_entry(Hash_Table* table, s64 index) {
	return table->entries[index].data;
}


// String specialized hash table
void hash_table_init(String_Hash_Table* table, s64 max_entries) {
	//table->entries = (String_Hash_Table::Entry*)calloc(max_entries, sizeof(String_Hash_Table::Entry));
	table->entries = (String_Hash_Table::Entry*)ho_bigalloc_rw(max_entries * sizeof(String_Hash_Table::Entry));
	table->entries_capacity = max_entries;
	table->entries_count = 0;
	table->hash_collision_count = 0;
}

void hash_table_add(String_Hash_Table* table, string s) {
	u64 hash = fnv_1_hash(s.data, s.length);
	u64 index = hash % table->entries_capacity;

	bool collided = false;

	while (table->entries[index].occupied) {
		if (table->entries[index].hash == hash) {
			// colisao entre hash's
			table->entries[index].collided = true;
			table->hash_collision_count++;
		}

		++index;
		if (index == table->entries_capacity)
			index = 0;
	}

	table->entries[index].hash = hash;
	table->entries[index].data = s;
	table->entries[index].occupied = true;
	table->entries[index].collided = collided;
	table->entries_count += 1;

	table->entries_count++;
}

static bool hash_table_entries_equal(String_Hash_Table* table, String_Hash_Table::Entry* e1, String_Hash_Table::Entry* e2) {
	if (e1->data.length != e2->data.length) return false;
	return (memcmp(e1->data.data, e2->data.data, e1->data.length) == 0);
}

s64  hash_table_entry_exist(String_Hash_Table* table, string s) {
	u64 hash = fnv_1_hash(s.data, s.length);
	u64 index = hash % table->entries_capacity;

	while (table->entries[index].occupied) {
		if (table->entries[index].hash == hash) {
			if (!table->entries[index].collided) {
				return index;
			} else {
				String_Hash_Table::Entry entry = { hash, s, false, true };
				if (hash_table_entries_equal(table, &table->entries[index], &entry)) {
					return index;
				}
			}
		}

		++index;
		if (index == table->entries_capacity)
			index = 0;
	}
	return -1;
}

void hash_table_remove(String_Hash_Table* table, string s) {
	s64 index = hash_table_entry_exist(table, s);
	if (index != -1) {
		table->entries[index].occupied = false;
		table->entries[index].collided = false;
		table->entries_count -= 1;
	}
}

string hash_table_get_string_entry(String_Hash_Table* table, s64 index) {
	return table->entries[index].data;
}