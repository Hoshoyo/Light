#include "global_tables.h"

String_Table global_identifiers_table = {0};
String_Table global_imports_table = {0};
Type_Table   global_type_table = {0};
string*      global_imports_queue = 0;

string global_compiler_path = {0};

Light_Arena* global_type_arena = 0;
