#include <stdio.h>
#include "lexer.h"
#include "utils/os.h"
#include "global_tables.h"
#include <light_array.h>

static void
compiler_set_fullpath(const char* path) {
    size_t compiler_fullpath_length = 0;
    const char* compiler_fullpath = light_path_from_filename(path, &compiler_fullpath_length);
    global_compiler_path.data = (char*)compiler_fullpath;
    global_compiler_path.length = (s32)compiler_fullpath_length;
}

static void
compiler_setup_global_import_table() {
    if(global_imports_table.entries_capacity == 0) {
        //global_imports_table
        string_table_new(&global_imports_table, 65536);
    }
}

static void
light_set_global_tables(const char* compiler_path) {
    compiler_set_fullpath(compiler_path);
    compiler_setup_global_import_table();
    global_imports_queue = array_new(string);
}

int main(int argc, char** argv) {
    light_set_global_tables(argv[0]);

    //Light_Lexer lexer = {0};
    //Light_Token* tokens = lexer_file(&lexer, "test/foo.li", 0);

    const char* s = light_real_path("test/foo.li", 0);
    //const char* f = light_path_from_filename("bin/light", 0);
    //const char* s = light_real_path_from(f, 0, "../test/foo.li", 0);
    printf("%s\n", s);


    return 0;
}