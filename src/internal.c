#include "parser.h"
#include "backend/backend.h"

static const char module_base_str_64bit[] = 
"string struct {\n"
    "capacity : u32;\n"
    "length   : u32;\n"
    "data     : ^u8;\n"
"}\n";

static const char module_base_str_32bit[] = 
"string struct {\n"
    "capacity : u32;\n"
    "length   : u32;\n"
    "data     : ^u8;\n"
"}\n";

static const char module_reflect_str_64bit[] = 
"array struct {\n"
"    capacity  : u64;\n"
"    length    : u64;\n"
"    type_info : ^User_Type_Info;\n"
"    data      : ^void;\n"
"}\n"
"\n"
"User_Type_Array struct {\n"
"    array_of  : ^User_Type_Info;\n"
"    dimension : u64;\n"
"}\n"
"\n"
"User_Type_Union struct {\n"
"    fields_types : ^^User_Type_Info;\n"
"	fields_names : ^string;\n"
"	fields_count : s32;\n"
"	alignment : s32;\n"
"}\n"
"\n"
"User_Type_Struct struct {\n"
"	fields_types        : ^^User_Type_Info;\n"
"	fields_names        : ^string;\n"
"	fields_offsets_bits : ^s64;\n"
"	fields_count        : s32;\n"
"	alignment           : s32;\n"
"}\n"
"\n"
"User_Type_Function struct {\n"
"	return_type     : ^User_Type_Info;\n"
"	arguments_type  : ^^User_Type_Info;\n"
"	arguments_name  : ^string;\n"
"	arguments_count : s32;\n"
"}\n"
"\n"
"User_Type_Alias struct {\n"
"    name     : string;\n"
"    alias_to : ^User_Type_Info;\n"
"}\n"
"\n"
"//User_Type_Enum struct {\n"
"//    type   : ^User_Type_Info;\n"
"//    fields : \n"
"//}\n"
"\n"
"User_Type_Desc union {\n"
"    primitive     : u32;\n"
"    pointer_to    : ^User_Type_Info;\n"
"    array_desc    : User_Type_Array;\n"
"    struct_desc   : User_Type_Struct;\n"
"    union_desc    : User_Type_Union;\n"
"    function_desc : User_Type_Function;\n"
"    alias_desc    : User_Type_Alias;\n"
"    //enum_desc     : User_Type_Enum;\n"
"}\n"
"\n"
"User_Type_Info struct {\n"
"    kind            : u32;\n"
"	flags           : u32;\n"
"	type_size_bytes : s64;\n"
"    description     : User_Type_Desc;\n"
"}\n"
"\n"
"User_Type_Value struct {\n"
"    value : ^void;\n"
"    type  : ^User_Type_Info;\n"
"}\n"
"\n"
"// TODO(psv): substitute this for an enum once is ready\n"
"LIGHT_TYPE_KIND_PRIMITIVE :u32: 1;\n"
"LIGHT_TYPE_KIND_POINTER   :u32: 2;\n"
"LIGHT_TYPE_KIND_STRUCT    :u32: 3;\n"
"LIGHT_TYPE_KIND_UNION     :u32: 4;\n"
"LIGHT_TYPE_KIND_ARRAY     :u32: 5;\n"
"LIGHT_TYPE_KIND_FUNCTION  :u32: 6;\n"
"LIGHT_TYPE_KIND_ENUM      :u32: 7;\n"
"LIGHT_TYPE_KIND_ALIAS     :u32: 8;\n"
"\n"
"LIGHT_TYPE_PRIMITIVE_VOID     :u32: 0;\n"
"LIGHT_TYPE_PRIMITIVE_S8       :u32: 1;\n"
"LIGHT_TYPE_PRIMITIVE_S16      :u32: 2;\n"
"LIGHT_TYPE_PRIMITIVE_S32      :u32: 3;\n"
"LIGHT_TYPE_PRIMITIVE_S64      :u32: 4;\n"
"LIGHT_TYPE_PRIMITIVE_U8       :u32: 5;\n"
"LIGHT_TYPE_PRIMITIVE_U16      :u32: 6;\n"
"LIGHT_TYPE_PRIMITIVE_U32      :u32: 7;\n"
"LIGHT_TYPE_PRIMITIVE_U64      :u32: 8;\n"
"LIGHT_TYPE_PRIMITIVE_R32      :u32: 9;\n"
"LIGHT_TYPE_PRIMITIVE_R64      :u32: 10;\n"
"LIGHT_TYPE_PRIMITIVE_BOOL     :u32: 11;\n"
"\n"
"LIGHT_TYPE_FUNCTION_VARIADIC :u32: 1 << 0;\n"
"LIGHT_STRUCT_FLAG_PACKED     :u32: 1 << 0;\n";

static const char module_reflect_str_32bit[] = 
"array struct {\n"
"    capacity  : u32;\n"
"    length    : u32;\n"
"    type_info : ^User_Type_Info;\n"
"    data      : ^void;\n"
"}\n"
"\n"
"User_Type_Array struct {\n"
"    array_of  : ^User_Type_Info;\n"
"    dimension : u32;\n"
"}\n"
"\n"
"User_Type_Union struct {\n"
"    fields_types : ^^User_Type_Info;\n"
"	fields_names : ^string;\n"
"	fields_count : s32;\n"
"	alignment : s32;\n"
"}\n"
"\n"
"User_Type_Struct struct {\n"
"	fields_types        : ^^User_Type_Info;\n"
"	fields_names        : ^string;\n"
"	fields_offsets_bits : ^s32;\n"
"	fields_count        : s32;\n"
"	alignment           : s32;\n"
"}\n"
"\n"
"User_Type_Function struct {\n"
"	return_type     : ^User_Type_Info;\n"
"	arguments_type  : ^^User_Type_Info;\n"
"	arguments_name  : ^string;\n"
"	arguments_count : s32;\n"
"}\n"
"\n"
"User_Type_Alias struct {\n"
"    name     : string;\n"
"    alias_to : ^User_Type_Info;\n"
"}\n"
"\n"
"//User_Type_Enum struct {\n"
"//    type   : ^User_Type_Info;\n"
"//    fields : \n"
"//}\n"
"\n"
"User_Type_Desc union {\n"
"    primitive     : u32;\n"
"    pointer_to    : ^User_Type_Info;\n"
"    array_desc    : User_Type_Array;\n"
"    struct_desc   : User_Type_Struct;\n"
"    union_desc    : User_Type_Union;\n"
"    function_desc : User_Type_Function;\n"
"    alias_desc    : User_Type_Alias;\n"
"    //enum_desc     : User_Type_Enum;\n"
"}\n"
"\n"
"User_Type_Info struct {\n"
"    kind            : u32;\n"
"	flags           : u32;\n"
"	type_size_bytes : s32;\n"
"    description     : User_Type_Desc;\n"
"}\n"
"\n"
"User_Type_Value struct {\n"
"    value : ^void;\n"
"    type  : ^User_Type_Info;\n"
"}\n"
"\n"
"// TODO(psv): substitute this for an enum once is ready\n"
"LIGHT_TYPE_KIND_PRIMITIVE :u32: 1;\n"
"LIGHT_TYPE_KIND_POINTER   :u32: 2;\n"
"LIGHT_TYPE_KIND_STRUCT    :u32: 3;\n"
"LIGHT_TYPE_KIND_UNION     :u32: 4;\n"
"LIGHT_TYPE_KIND_ARRAY     :u32: 5;\n"
"LIGHT_TYPE_KIND_FUNCTION  :u32: 6;\n"
"LIGHT_TYPE_KIND_ENUM      :u32: 7;\n"
"LIGHT_TYPE_KIND_ALIAS     :u32: 8;\n"
"\n"
"LIGHT_TYPE_PRIMITIVE_VOID     :u32: 0;\n"
"LIGHT_TYPE_PRIMITIVE_S8       :u32: 1;\n"
"LIGHT_TYPE_PRIMITIVE_S16      :u32: 2;\n"
"LIGHT_TYPE_PRIMITIVE_S32      :u32: 3;\n"
"LIGHT_TYPE_PRIMITIVE_S64      :u32: 4;\n"
"LIGHT_TYPE_PRIMITIVE_U8       :u32: 5;\n"
"LIGHT_TYPE_PRIMITIVE_U16      :u32: 6;\n"
"LIGHT_TYPE_PRIMITIVE_U32      :u32: 7;\n"
"LIGHT_TYPE_PRIMITIVE_U64      :u32: 8;\n"
"LIGHT_TYPE_PRIMITIVE_R32      :u32: 9;\n"
"LIGHT_TYPE_PRIMITIVE_R64      :u32: 10;\n"
"LIGHT_TYPE_PRIMITIVE_BOOL     :u32: 11;\n"
"\n"
"LIGHT_TYPE_FUNCTION_VARIADIC :u32: 1 << 0;\n"
"LIGHT_STRUCT_FLAG_PACKED     :u32: 1 << 0;\n";

static Light_Ast**
load_internal_module(Light_Parser* parser, Light_Scope* global_scope, const char* module_str, int module_str_length_bytes) {
    Light_Lexer lexer = {0};
    Light_Token* tokens = lexer_cstr(&lexer, (char*)module_str, module_str_length_bytes, 0);

    u32 parser_error = PARSER_OK;
    Light_Ast** ast = parse_top_level(parser, &lexer, global_scope, &parser_error);
    if(parser_error & PARSER_ERROR_FATAL)
        return 0;
    return ast;
}

Light_Ast**
load_internal_modules(Light_Parser* parser, Light_Scope* global_scope, Light_Backend backend) {
    Light_Ast** ast = 0;
    switch(backend) {
        case BACKEND_X86_ELF:
        case BACKEND_X86_PECOFF:
        case BACKEND_X86_RAWX: {
            ast = load_internal_module(parser, global_scope, (char*)module_base_str_32bit, sizeof(module_base_str_32bit));
            ast = load_internal_module(parser, global_scope, module_reflect_str_32bit, sizeof(module_reflect_str_32bit));
        } break;
        case BACKEND_C: {
            //ast = load_internal_module(parser, global_scope, (char*)module_base_str_64bit, sizeof(module_base_str_64bit));
            //ast = load_internal_module(parser, global_scope, module_reflect_str_64bit, sizeof(module_reflect_str_64bit));
        } break;
        case BACKEND_LIGHT_VM: {
            //ast = load_internal_module(parser, global_scope, (char*)module_base_str_64bit, sizeof(module_base_str_64bit));
            //ast = load_internal_module(parser, global_scope, module_reflect_str_64bit, sizeof(module_reflect_str_64bit));
        } break;
    }
    
    return ast;
}