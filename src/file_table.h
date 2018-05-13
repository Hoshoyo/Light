#pragma once
#include "util.h"

struct File_Table_Entry {
    string filename;
    string fullpath;
};

void file_table_init();
s64 file_table_push(char* fullpath);