#include "global_tables.h"

String_Table global_identifiers_table = {0};
String_Table global_imports_table = {0};
string*      global_imports_queue = 0;

string global_compiler_path = {0};
string global_working_dir = {0};