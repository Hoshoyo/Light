#include "file_table.h"
#include "hash_table.h"
#include <light_arena.h>

Hash_Table file_table = {0};

static Light_Arena* file_table_arena;
#define FTABLE_ALLOC() (File_Table_Entry*)arena_alloc(file_table_arena, sizeof(File_Table_Entry))

struct File_Table_Arena_Init {
	File_Table_Arena_Init(size_t size) {
		file_table_arena = arena_create(size);
	}
};
File_Table_Arena_Init file_table_arena_init(65536);

u64 file_table_hash(void* data) {
    File_Table_Entry* e = (File_Table_Entry*)data;
    return fnv_1_hash(e->fullpath.data, e->fullpath.length);
}

bool file_table_compare(void* e1, void* e2) {
    File_Table_Entry* e_1 = (File_Table_Entry*)e1;
    File_Table_Entry* e_2 = (File_Table_Entry*)e2;
    return str_equal(e_1->fullpath, e_2->fullpath);
}

void file_table_init() {
    hash_table_init(&file_table, 1 << 18, file_table_hash, file_table_compare);
}

// returns -1 if the entry exists
s64 file_table_push(char* fullpath) {
    File_Table_Entry* entry = FTABLE_ALLOC();
    entry->fullpath = string_make(fullpath);
    entry->filename = filename_from_path(entry->fullpath);

    u64 hash = file_table_hash(entry);
    s64 index = hash_table_entry_exist(&file_table, entry, hash);

    if(index == -1) {
        index = hash_table_add(&file_table, entry, sizeof(File_Table_Entry), hash);
    } else {
        return -1;
    }
}