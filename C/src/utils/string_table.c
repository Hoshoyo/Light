#include "string_table.h"
#include "allocator.h"

void 
string_table_new(String_Table* table, s32 capacity) {
    table->entries = (String_Table_Entry*)light_alloc(capacity * sizeof(String_Table_Entry));
    table->entries_capacity = capacity;
    table->entries_count = 0;
    table->hash_collision_count = 0;
}

void
string_table_free(String_Table* table) {
    light_free(table->entries);
    table->entries_capacity = 0;
    table->entries_count = 0;
    table->hash_collision_count = 0;
}

bool 
string_table_add(String_Table* table, string s, s32* out_index) {
    u64 hash = fnv_1_hash(s.data, s.length);
	s32 index = (s32)(hash % table->entries_capacity);

	while (table->entries[index].flags & STRING_TABLE_OCCUPIED) {
		if (table->entries[index].hash == hash) {
            // The entry is already in the hash table
			return false;
		}

        table->entries[index].flags |= STRING_TABLE_COLLIDED;
		index = (index + 1) % table->entries_capacity;
	}

	table->entries[index].hash = hash;
	table->entries[index].data = s;
	table->entries[index].flags = STRING_TABLE_OCCUPIED;
	table->entries_count += 1;
}

bool 
string_table_entry_exist(String_Table* table, string s, s32* out_index, u64* out_hash) {
    u64 hash = fnv_1_hash(s.data, s.length);
	s32 index = (s32)(hash % table->entries_capacity);

	while (table->entries[index].flags & STRING_TABLE_OCCUPIED) {
		if (table->entries[index].hash == hash) {
            // No need to check if it is the same entry, since
            // we assume the hash to be unique
            if(out_index) {
                *out_index = index;
            }
            if(out_hash) {
                *out_hash = hash;
            }
            return true;
		}

		index = (index + 1) % table->entries_capacity;
	}
	return false;
}

bool
string_table_del(String_Table* table, string s) {
    s32 index = 0;
    if(string_table_entry_exist(table, s, &index, 0)) {
		table->entries[index].flags = 0;
        table->entries[index].hash = 0;
		table->entries_count -= 1;
        return true;
    }
    return false;
}

string 
string_table_get(String_Table* table, s32 index) {
    return table->entries[index].data;
}

// Hash functions
u64
fnv_1_hash(const u8* s, u64 length) {
	u64 hash = 14695981039346656037ULL;
	u64 fnv_prime = 1099511628211ULL;
	for (u64 i = 0; i < length; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}