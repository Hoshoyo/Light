#include "type_table.h"
#include "utils/allocator.h"
#include "type.h"

void 
type_table_new(Type_Table* table, s32 capacity) {
    table->entries = (Type_Table_Entry*)light_alloc(capacity * sizeof(Type_Table_Entry));
    table->entries_capacity = capacity;
    table->entries_count = 0;
    table->hash_collision_count = 0;
}

void
type_table_free(Type_Table* table) {
    light_free(table->entries);
    table->entries_capacity = 0;
    table->entries_count = 0;
    table->hash_collision_count = 0;
}

bool 
type_table_add(Type_Table* table, Light_Type* t, s32* out_index) {
    u64 hash = type_hash(t);
	s32 index = (s32)(hash % table->entries_capacity);

	while (table->entries[index].flags & STRING_TABLE_OCCUPIED) {
		if (table->entries[index].hash == hash) {
            // The entry is already in the hash table
            if(out_index) *out_index = index;
			return false;
		}

        table->entries[index].flags |= STRING_TABLE_COLLIDED;
		index = (index + 1) % table->entries_capacity;
	}

	table->entries[index].hash = hash;
	table->entries[index].data = t;
	table->entries[index].flags = STRING_TABLE_OCCUPIED;
	table->entries_count += 1;
    if(out_index) *out_index = index;

    return true;
}

bool 
type_table_entry_exist(Type_Table* table, Light_Type* t, s32* out_index, u64* out_hash) {
    u64 hash = type_hash(t);
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
type_table_del(Type_Table* table, Light_Type* t) {
    s32 index = 0;
    if(type_table_entry_exist(table, t, &index, 0)) {
		table->entries[index].flags = 0;
        table->entries[index].hash = 0;
		table->entries_count -= 1;
        return true;
    }
    return false;
}

Light_Type*
type_table_get(Type_Table* table, s32 index) {
    return table->entries[index].data;
}