
#define GENERATE_HASH_TABLE(UNAME, LNAME, TYPE) \
typedef struct { \
    unsigned int       flags; \
    unsigned long int  hash; \
    TYPE data; \
} UNAME##_Table_Entry; \
\
typedef struct { \
    int                  entries_count; \
	int                  entries_capacity; \
	int                  hash_collision_count; \
	UNAME##_Table_Entry* entries; \
} UNAME##_Table; \
void LNAME##_table_new(UNAME##_Table* table, int capacity); \
void LNAME##_table_free(UNAME##_Table* table); \
int LNAME##_table_add(UNAME##_Table* table, TYPE v, int* out_index); \
int LNAME##_table_entry_exist(UNAME##_Table* table, TYPE v, int* out_index, unsigned long int* out_hash); \
int LNAME##_table_del(UNAME##_Table* table, TYPE v); \
TYPE LNAME##_table_get(UNAME##_Table* table, int index);

#define GENERATE_HASH_TABLE_IMPLEMENTATION(UNAME, LNAME, TYPE, HASHFUNC, ALLOCATOR, FREE) \
void LNAME##_table_new(UNAME##_Table* table, int capacity) { \
    table->entries = (UNAME##_Table_Entry*)ALLOCATOR(capacity * sizeof(UNAME##_Table_Entry)); \
    table->entries_capacity = capacity; \
    table->entries_count = 0; \
    table->hash_collision_count = 0; \
} \
\
void \
LNAME##_table_free(UNAME##_Table* table) { \
    FREE(table->entries); \
    table->entries_capacity = 0; \
    table->entries_count = 0; \
    table->hash_collision_count = 0; \
} \
\
int LNAME##_table_add(UNAME##_Table* table, TYPE v, int* out_index) { \
    unsigned long int hash = HASHFUNC(v); \
	int index = (int)(hash % table->entries_capacity); \
\
	while (table->entries[index].flags & (1 << 0)) { \
		if (table->entries[index].hash == hash) { \
            if(out_index) *out_index = index; \
			return 0; \
		} \
\
        table->entries[index].flags |= (1 << 1); \
		index = (index + 1) % table->entries_capacity; \
	} \
\
	table->entries[index].hash = hash; \
	table->entries[index].data = v; \
	table->entries[index].flags = (1 << 0); \
	table->entries_count += 1; \
    if(out_index) *out_index = index; \
    return true; \
} \
\
int LNAME##_table_entry_exist(UNAME##_Table* table, TYPE v, int* out_index, unsigned long int* out_hash) { \
    unsigned long int hash = HASHFUNC(v); \
	int index = (int)(hash % table->entries_capacity); \
\
	while (table->entries[index].flags & (1 << 0)) { \
		if (table->entries[index].hash == hash) { \
            if(out_index) { \
                *out_index = index; \
            } \
            if(out_hash) { \
                *out_hash = hash; \
            } \
            return 1; \
		} \
\
		index = (index + 1) % table->entries_capacity; \
	} \
	return 0; \
} \
\
int LNAME##_table_del(UNAME##_Table* table, TYPE v) { \
    int index = 0; \
    if(LNAME##_table_entry_exist(table, v, &index, 0)) { \
		table->entries[index].flags = 0; \
        table->entries[index].hash = 0; \
		table->entries_count -= 1; \
        return 1; \
    } \
    return 0; \
} \
\
TYPE LNAME##_table_get(UNAME##_Table* table, int index) { \
    return table->entries[index].data; \
}
