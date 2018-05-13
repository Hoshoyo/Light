#include "file_table.h"
#include "memory.h"
#include "hash_table.h"

Hash_Table file_table = {0};
static Memory_Arena file_table_arena(65536);
#define FTABLE_ALLOC() (File_Table_Entry*)file_table_arena.allocate(sizeof(File_Table_Entry))


u64 file_table_hash(void* data) {
    File_Table_Entry* e = (File_Table_Entry*)data;
    return fnv_1_hash(e->fullpath.data, e->fullpath.length);
}

bool file_table_compare(void* e1, void* e2) {
    File_Table_Entry* e_1 = (File_Table_Entry*)e1;
    File_Table_Entry* e_2 = (File_Table_Entry*)e2;
    str_equal(e_1->fullpath, e_2->fullpath);
}

void file_table_init() {
    hash_table_init(&file_table, 1 << 18, file_table_hash, file_table_compare);
}

// returns -1 if the entry exists
s64 file_table_push(char* fullpath) {
    File_Table_Entry* entry = FTABLE_ALLOC();
    entry->fullpath = string_make(fullpath);
    entry->filename = filename_from_path(entry->fullpath);

    u64 hash = file_table_hash(&entry->fullpath);
    s64 index = hash_table_entry_exist(&file_table, entry, hash);

    if(index == -1) {
        index = hash_table_add(&file_table, entry, sizeof(File_Table_Entry), hash);
    } else {
        return -1;
    }
}