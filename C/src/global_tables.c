#include "global_tables.h"
#include <light_array.h>
#include "utils/os.h"

String_Table global_identifiers_table = {0};
String_Table global_imports_table = {0};
Symbol_Table global_symbol_table = {0};
Type_Table   global_type_table = {0};
string*      global_imports_queue = 0;

string global_compiler_path = {0};

Light_Arena* global_type_arena = 0;
Light_Ast**  global_infer_queue = 0;

static void
compiler_setup_global_import_table() {
    if(global_imports_table.entries_capacity == 0) {
        //global_imports_table
        string_table_new(&global_imports_table, 65536);
    }
}

static void
compiler_setup_global_type_table() {
    if(global_type_table.entries_capacity == 0) {
        type_table_new(&global_type_table, 65536);
    }
}

static void
compiler_set_fullpath(const char* path) {
    size_t compiler_fullpath_length = 0;
    const char* compiler_fullpath = light_path_from_filename(path, &compiler_fullpath_length);
    global_compiler_path.data = (char*)compiler_fullpath;
    global_compiler_path.length = (s32)compiler_fullpath_length;
}

void
light_set_global_tables(const char* compiler_path) {
    compiler_set_fullpath(compiler_path);
    compiler_setup_global_import_table();
    compiler_setup_global_type_table();
    global_imports_queue = array_new_len(string, 1024);
    global_infer_queue = array_new_len(Light_Ast*, 2048);
    global_type_arena = arena_create(65536);
    type_tables_initialize();
}