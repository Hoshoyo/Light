#pragma once
#include <common.h>

typedef struct {
    s32   length;
    u32   value;
    char* data;
} string;

#define MAKE_STR(S, V) (string){ sizeof(S) - 1, V, S }
#define MAKE_STR_LEN(S, L) (string){ L, 0, S }

typedef enum {
    STRING_TABLE_OCCUPIED = (1 << 0),
    STRING_TABLE_COLLIDED = (1 << 1),
} String_Table_Flags;

typedef struct {
    u32    flags;
    u64    hash;
    string data;
} String_Table_Entry;

typedef struct {
    s32                 entries_count;
	s32                 entries_capacity;
	s32                 hash_collision_count;
	String_Table_Entry* entries;
} String_Table;

void   string_table_new(String_Table* table, s32 capacity);
void   string_table_free(String_Table* table);
bool   string_table_add(String_Table* table, string s, s32* out_index);
bool   string_table_entry_exist(String_Table* table, string s, s32* out_index, u64* out_hash);
bool   string_table_del(String_Table* table, string s);
string string_table_get(String_Table* table, s32 index);

// Hash functions
u64    fnv_1_hash(const u8* s, u64 length);