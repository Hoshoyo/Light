#pragma once
#include <common.h>
#include "type.h"

typedef enum {
    TYPE_TABLE_OCCUPIED = (1 << 0),
    TYPE_TABLE_COLLIDED = (1 << 1),
} Type_Table_Flags;

typedef struct {
    u32         flags;
    u64         hash;
    Light_Type* data;
} Type_Table_Entry;

typedef struct {
    s32               entries_count;
	s32               entries_capacity;
	s32               hash_collision_count;
	Type_Table_Entry* entries;
} Type_Table;

void        type_table_new(Type_Table* table, s32 capacity);
void        type_table_free(Type_Table* table);
bool        type_table_add(Type_Table* table, Light_Type* t, s32* out_index);
bool        type_table_entry_exist(Type_Table* table, Light_Type* t, s32* out_index, u64* out_hash);
bool        type_table_del(Type_Table* table, Light_Type* s);
Light_Type* type_table_get(Type_Table* table, s32 index);