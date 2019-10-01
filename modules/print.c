typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 bool;
typedef float r32;
typedef double r64;
#define true 1
#define false 0


void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef struct string { u64 capacity; u64 length; u8* data; } string;
typedef struct array { u64 capacity; u64 length; struct User_Type_Info* type_info; void* data; } array;
typedef union u { s32 a; r32 b; } u;
typedef struct User_Type_Array { struct User_Type_Info* array_of; u64 dimension; } User_Type_Array;
typedef struct User_Type_Union { struct User_Type_Info** fields_types; struct string* fields_names; s32 fields_count; s32 alignment; } User_Type_Union;
typedef struct User_Type_Struct { struct User_Type_Info** fields_types; struct string* fields_names; s64* fields_offsets_bits; s32 fields_count; s32 alignment; } User_Type_Struct;
typedef struct User_Type_Function { struct User_Type_Info* return_type; struct User_Type_Info** arguments_type; struct string* arguments_name; s32 arguments_count; } User_Type_Function;
typedef struct User_Type_Alias { string name; struct User_Type_Info* alias_to; } User_Type_Alias;
typedef union User_Type_Desc { u32 primitive; struct User_Type_Info* pointer_to; User_Type_Array array_desc; User_Type_Struct struct_desc; User_Type_Union union_desc; User_Type_Function function_desc; User_Type_Alias alias_desc; } User_Type_Desc;
typedef struct User_Type_Info { u32 kind; u32 flags; s64 type_size_bytes; User_Type_Desc description; } User_Type_Info;
typedef struct User_Type_Value { void* value; struct User_Type_Info* type; } User_Type_Value;

// Forward Declarations

s64( write)(s32 fd, void* buf, u64 count);
void*( memcpy)(void* dst, void* src, u64 size);
u64( print_string)(string s);
u64( print_test)(void* args);
u64( print)(string fmt, void* args);
u64( print_bytes)(u8* v, s32 count);
u64( print_value_literal_struct)(User_Type_Value v);
u64( print_value_literal_array)(User_Type_Value v);
u64( print_value_literal)(User_Type_Value v);
u64( print_type_primitive)(struct User_Type_Info* t);
u64( print_type_pointer)(struct User_Type_Info* t);
u64( print_type_struct)(struct User_Type_Info* t);
u64( print_type_union)(struct User_Type_Info* t);
u64( print_type_alias)(struct User_Type_Info* t);
u64( print_type_array)(struct User_Type_Info* t);
u64( print_type_function)(struct User_Type_Info* t);
u64( print_type)(struct User_Type_Info* t);
u64( u64_to_str)(u64 val, u8( buffer)[32]);
u64( s64_to_str)(s64 val, u8( buffer)[32]);
u64( unsigned_to_str_base16)(u64 value, u64 bitsize, bool leading_zeros, bool capitalized, u8( buffer)[16]);
u64( print_s8)(s8 value);
u64( print_s16)(s16 value);
u64( print_s32)(s32 value);
u64( print_s64)(s64 value);
u64( print_u8)(u8 value, s32 base);
u64( print_u16)(u16 value, s32 base);
u64( print_u32)(u32 value, s32 base);
u64( print_u64)(u64 value, s32 base);
u64( r64_to_str)(r64 v, u8( buffer)[64]);
u64( r32_to_str)(r32 v, u8( buffer)[32]);
u64( print_r32)(r32 v);
u64( print_r64)(r64 v);
s32( __light_main)();

// Type table

User_Type_Info* __function_args_types_0x134b6a0[3] = {0};
User_Type_Info* __function_args_types_0x134b7a0[3] = {0};
User_Type_Info* __function_args_types_0x134b8a0[1] = {0};
User_Type_Info* __function_args_types_0x134bc60[2] = {0};
User_Type_Info* __function_args_types_0x134d460[1] = {0};
User_Type_Info* __function_args_types_0x134d520[1] = {0};
User_Type_Info* __function_args_types_0x134d5e0[1] = {0};
User_Type_Info* __function_args_types_0x134d6a0[1] = {0};
User_Type_Info* __function_args_types_0x134d760[2] = {0};
User_Type_Info* __function_args_types_0x134d820[2] = {0};
User_Type_Info* __function_args_types_0x134d8e0[2] = {0};
User_Type_Info* __function_args_types_0x134d9a0[2] = {0};
User_Type_Info* __function_args_types_0x134dce0[1] = {0};
User_Type_Info* __function_args_types_0x134dda0[1] = {0};
User_Type_Info* __struct_field_types_0x134b4a0[3] = {0};
string __struct_field_names_0x134b4a0[3] = {{ 0, 8, "capacity" }, { 0, 6, "length" }, { 0, 4, "data" }};
s64 __struct_field_offsets_0x134b4a0[3] = {0, 64, 128};
User_Type_Info* __struct_field_types_0x134b5e0[4] = {0};
string __struct_field_names_0x134b5e0[4] = {{ 0, 8, "capacity" }, { 0, 6, "length" }, { 0, 9, "type_info" }, { 0, 4, "data" }};
s64 __struct_field_offsets_0x134b5e0[4] = {0, 64, 128, 192};
User_Type_Info* __function_args_types_0x134b820[1] = {0};
User_Type_Info* __function_args_types_0x134baa0[2] = {0};
User_Type_Info* __function_args_types_0x134c5e0[1] = {0};
User_Type_Info* __function_args_types_0x134d2a0[2] = {0};
User_Type_Info* __function_args_types_0x134d360[2] = {0};
User_Type_Info* __function_args_types_0x134d420[5] = {0};
User_Type_Info* __function_args_types_0x134db20[2] = {0};
User_Type_Info* __function_args_types_0x134dc20[2] = {0};
User_Type_Info* __union_field_types_0x134de60[2] = {0};
string __union_field_names_0x134de60[2] = {{ 0, 1, "a" }, { 0, 1, "b" }};
User_Type_Info* __struct_field_types_0x134dfe0[2] = {0};
string __struct_field_names_0x134dfe0[2] = {{ 0, 8, "array_of" }, { 0, 9, "dimension" }};
s64 __struct_field_offsets_0x134dfe0[2] = {0, 64};
User_Type_Info* __struct_field_types_0x134e1a0[4] = {0};
string __struct_field_names_0x134e1a0[4] = {{ 0, 12, "fields_types" }, { 0, 12, "fields_names" }, { 0, 12, "fields_count" }, { 0, 9, "alignment" }};
s64 __struct_field_offsets_0x134e1a0[4] = {0, 64, 128, 160};
User_Type_Info* __struct_field_types_0x134e3a0[5] = {0};
string __struct_field_names_0x134e3a0[5] = {{ 0, 12, "fields_types" }, { 0, 12, "fields_names" }, { 0, 19, "fields_offsets_bits" }, { 0, 12, "fields_count" }, { 0, 9, "alignment" }};
s64 __struct_field_offsets_0x134e3a0[5] = {0, 64, 128, 192, 224};
User_Type_Info* __struct_field_types_0x134e5e0[4] = {0};
string __struct_field_names_0x134e5e0[4] = {{ 0, 11, "return_type" }, { 0, 14, "arguments_type" }, { 0, 14, "arguments_name" }, { 0, 15, "arguments_count" }};
s64 __struct_field_offsets_0x134e5e0[4] = {0, 64, 128, 192};
User_Type_Info* __struct_field_types_0x134e720[2] = {0};
string __struct_field_names_0x134e720[2] = {{ 0, 4, "name" }, { 0, 8, "alias_to" }};
s64 __struct_field_offsets_0x134e720[2] = {0, 192};
User_Type_Info* __union_field_types_0x134e960[7] = {0};
string __union_field_names_0x134e960[7] = {{ 0, 9, "primitive" }, { 0, 10, "pointer_to" }, { 0, 10, "array_desc" }, { 0, 11, "struct_desc" }, { 0, 10, "union_desc" }, { 0, 13, "function_desc" }, { 0, 10, "alias_desc" }};
User_Type_Info* __struct_field_types_0x134ea20[4] = {0};
string __struct_field_names_0x134ea20[4] = {{ 0, 4, "kind" }, { 0, 5, "flags" }, { 0, 15, "type_size_bytes" }, { 0, 11, "description" }};
s64 __struct_field_offsets_0x134ea20[4] = {0, 32, 64, 256};
User_Type_Info* __struct_field_types_0x134eb60[2] = {0};
string __struct_field_names_0x134eb60[2] = {{ 0, 5, "value" }, { 0, 4, "type" }};
s64 __struct_field_offsets_0x134eb60[2] = {0, 64};
User_Type_Info* __function_args_types_0x134bd60[1] = {0};
User_Type_Info __light_type_table[97] = {
{ 1, 44, 4, { .primitive = 9}},
{ 1, 44, 8, { .primitive = 10}},
{ 1, 44, 1, { .primitive = 1}},
{ 1, 44, 2, { .primitive = 2}},
{ 1, 44, 4, { .primitive = 3}},
{ 1, 44, 8, { .primitive = 4}},
{ 1, 44, 1, { .primitive = 5}},
{ 1, 44, 2, { .primitive = 6}},
{ 1, 44, 4, { .primitive = 7}},
{ 1, 44, 8, { .primitive = 8}},
{ 1, 44, 0, { .primitive = 0}},
{ 1, 44, 4, { .primitive = 11}},
{ 3, 44, 0, { .struct_desc = { 0, 0, 0, 0, 0 }}},
{ 4, 44, 0, { .union_desc = { 0, 0, 0, 0 }}},
{ 7, 44, 0, { .primitive = 0}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[6]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[10]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[5], (User_Type_Info**)&__function_args_types_0x134b6a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[16], (User_Type_Info**)&__function_args_types_0x134b7a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134b8a0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134bc60, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[2]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[3]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[4]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[5]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[7]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[8]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[9]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[1]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[11]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[16]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d460, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d520, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d5e0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d6a0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d760, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d820, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d8e0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d9a0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134dce0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134dda0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], 0, 0, 0 }}},
{ 3, 44, 24, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134b4a0, __struct_field_names_0x134b4a0, (s64*)__struct_field_offsets_0x134b4a0, 3, 4 }}},
{ 8, 36, 24, { .alias_desc = { {0, 6, "string"}, &__light_type_table[43] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134b5e0, __struct_field_names_0x134b5e0, (s64*)__struct_field_offsets_0x134b5e0, 4, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 5, "array"}, &__light_type_table[46] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134b820, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[47]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 5, 44, 4, { .array_desc = { &__light_type_table[6], 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134baa0, 0, 2 }}},
{ 5, 44, 3, { .array_desc = { &__light_type_table[6], 3 }}},
{ 5, 44, 6, { .array_desc = { &__light_type_table[6], 6 }}},
{ 5, 44, 7, { .array_desc = { &__light_type_table[6], 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134c5e0, 0, 1 }}},
{ 5, 44, 2, { .array_desc = { &__light_type_table[6], 2 }}},
{ 5, 44, 5, { .array_desc = { &__light_type_table[6], 5 }}},
{ 5, 44, 11, { .array_desc = { &__light_type_table[6], 11 }}},
{ 5, 44, 10, { .array_desc = { &__light_type_table[6], 10 }}},
{ 5, 44, 32, { .array_desc = { &__light_type_table[6], 32 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d2a0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d360, 0, 2 }}},
{ 5, 44, 16, { .array_desc = { &__light_type_table[6], 16 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134d420, 0, 5 }}},
{ 5, 44, 64, { .array_desc = { &__light_type_table[6], 64 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134db20, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134dc20, 0, 2 }}},
{ 4, 44, 4, { .union_desc = { (User_Type_Info**)&__union_field_types_0x134de60, __union_field_names_0x134de60, 2, 4 }}},
{ 8, 36, 4, { .alias_desc = { {0, 1, "u"}, &__light_type_table[69] }}},
{ 5, 44, 8, { .array_desc = { &__light_type_table[4], 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[71]}},
{ 3, 44, 16, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134dfe0, __struct_field_names_0x134dfe0, (s64*)__struct_field_offsets_0x134dfe0, 2, 4 }}},
{ 8, 36, 16, { .alias_desc = { {0, 15, "User_Type_Array"}, &__light_type_table[73] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[45]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[44]}},
{ 3, 44, 24, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134e1a0, __struct_field_names_0x134e1a0, (s64*)__struct_field_offsets_0x134e1a0, 4, 4 }}},
{ 8, 36, 24, { .alias_desc = { {0, 15, "User_Type_Union"}, &__light_type_table[77] }}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134e3a0, __struct_field_names_0x134e3a0, (s64*)__struct_field_offsets_0x134e3a0, 5, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 16, "User_Type_Struct"}, &__light_type_table[79] }}},
{ 3, 44, 28, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134e5e0, __struct_field_names_0x134e5e0, (s64*)__struct_field_offsets_0x134e5e0, 4, 4 }}},
{ 8, 36, 28, { .alias_desc = { {0, 18, "User_Type_Function"}, &__light_type_table[81] }}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134e720, __struct_field_names_0x134e720, (s64*)__struct_field_offsets_0x134e720, 2, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 15, "User_Type_Alias"}, &__light_type_table[83] }}},
{ 4, 44, 32, { .union_desc = { (User_Type_Info**)&__union_field_types_0x134e960, __union_field_names_0x134e960, 7, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 14, "User_Type_Desc"}, &__light_type_table[85] }}},
{ 3, 44, 64, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134ea20, __struct_field_names_0x134ea20, (s64*)__struct_field_offsets_0x134ea20, 4, 4 }}},
{ 8, 36, 64, { .alias_desc = { {0, 14, "User_Type_Info"}, &__light_type_table[87] }}},
{ 3, 44, 16, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x134eb60, __struct_field_names_0x134eb60, (s64*)__struct_field_offsets_0x134eb60, 2, 4 }}},
{ 8, 36, 16, { .alias_desc = { {0, 15, "User_Type_Value"}, &__light_type_table[89] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x134bd60, 0, 1 }}},
{ 5, 44, 8, { .array_desc = { &__light_type_table[72], 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[92]}},
{ 5, 44, 8, { .array_desc = { &__light_type_table[42], 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[94]}},
{ 5, 44, 32, { .array_desc = { &__light_type_table[90], 2 }}}};

void __light_load_type_table() {
__function_args_types_0x134b6a0[0] = &__light_type_table[4];
__function_args_types_0x134b6a0[1] = &__light_type_table[16];
__function_args_types_0x134b6a0[2] = &__light_type_table[9];

__function_args_types_0x134b7a0[0] = &__light_type_table[16];
__function_args_types_0x134b7a0[1] = &__light_type_table[16];
__function_args_types_0x134b7a0[2] = &__light_type_table[9];

__function_args_types_0x134b8a0[0] = &__light_type_table[16];

__function_args_types_0x134bc60[0] = &__light_type_table[15];
__function_args_types_0x134bc60[1] = &__light_type_table[4];

__function_args_types_0x134d460[0] = &__light_type_table[2];

__function_args_types_0x134d520[0] = &__light_type_table[3];

__function_args_types_0x134d5e0[0] = &__light_type_table[4];

__function_args_types_0x134d6a0[0] = &__light_type_table[5];

__function_args_types_0x134d760[0] = &__light_type_table[6];
__function_args_types_0x134d760[1] = &__light_type_table[4];

__function_args_types_0x134d820[0] = &__light_type_table[7];
__function_args_types_0x134d820[1] = &__light_type_table[4];

__function_args_types_0x134d8e0[0] = &__light_type_table[8];
__function_args_types_0x134d8e0[1] = &__light_type_table[4];

__function_args_types_0x134d9a0[0] = &__light_type_table[9];
__function_args_types_0x134d9a0[1] = &__light_type_table[4];

__function_args_types_0x134dce0[0] = &__light_type_table[0];

__function_args_types_0x134dda0[0] = &__light_type_table[1];

__struct_field_types_0x134b4a0[0] = &__light_type_table[9];
__struct_field_types_0x134b4a0[1] = &__light_type_table[9];
__struct_field_types_0x134b4a0[2] = &__light_type_table[15];

__struct_field_types_0x134b5e0[0] = &__light_type_table[9];
__struct_field_types_0x134b5e0[1] = &__light_type_table[9];
__struct_field_types_0x134b5e0[2] = &__light_type_table[45];
__struct_field_types_0x134b5e0[3] = &__light_type_table[16];

__function_args_types_0x134b820[0] = &__light_type_table[44];

__function_args_types_0x134baa0[0] = &__light_type_table[44];
__function_args_types_0x134baa0[1] = &__light_type_table[16];

__function_args_types_0x134c5e0[0] = &__light_type_table[45];

__function_args_types_0x134d2a0[0] = &__light_type_table[9];
__function_args_types_0x134d2a0[1] = &__light_type_table[61];

__function_args_types_0x134d360[0] = &__light_type_table[5];
__function_args_types_0x134d360[1] = &__light_type_table[61];

__function_args_types_0x134d420[0] = &__light_type_table[9];
__function_args_types_0x134d420[1] = &__light_type_table[9];
__function_args_types_0x134d420[2] = &__light_type_table[11];
__function_args_types_0x134d420[3] = &__light_type_table[11];
__function_args_types_0x134d420[4] = &__light_type_table[64];

__function_args_types_0x134db20[0] = &__light_type_table[1];
__function_args_types_0x134db20[1] = &__light_type_table[66];

__function_args_types_0x134dc20[0] = &__light_type_table[0];
__function_args_types_0x134dc20[1] = &__light_type_table[61];

__union_field_types_0x134de60[0] = &__light_type_table[4];
__union_field_types_0x134de60[1] = &__light_type_table[0];

__struct_field_types_0x134dfe0[0] = &__light_type_table[45];
__struct_field_types_0x134dfe0[1] = &__light_type_table[9];

__struct_field_types_0x134e1a0[0] = &__light_type_table[75];
__struct_field_types_0x134e1a0[1] = &__light_type_table[76];
__struct_field_types_0x134e1a0[2] = &__light_type_table[4];
__struct_field_types_0x134e1a0[3] = &__light_type_table[4];

__struct_field_types_0x134e3a0[0] = &__light_type_table[75];
__struct_field_types_0x134e3a0[1] = &__light_type_table[76];
__struct_field_types_0x134e3a0[2] = &__light_type_table[24];
__struct_field_types_0x134e3a0[3] = &__light_type_table[4];
__struct_field_types_0x134e3a0[4] = &__light_type_table[4];

__struct_field_types_0x134e5e0[0] = &__light_type_table[45];
__struct_field_types_0x134e5e0[1] = &__light_type_table[75];
__struct_field_types_0x134e5e0[2] = &__light_type_table[76];
__struct_field_types_0x134e5e0[3] = &__light_type_table[4];

__struct_field_types_0x134e720[0] = &__light_type_table[44];
__struct_field_types_0x134e720[1] = &__light_type_table[45];

__union_field_types_0x134e960[0] = &__light_type_table[8];
__union_field_types_0x134e960[1] = &__light_type_table[45];
__union_field_types_0x134e960[2] = &__light_type_table[74];
__union_field_types_0x134e960[3] = &__light_type_table[80];
__union_field_types_0x134e960[4] = &__light_type_table[78];
__union_field_types_0x134e960[5] = &__light_type_table[82];
__union_field_types_0x134e960[6] = &__light_type_table[84];

__struct_field_types_0x134ea20[0] = &__light_type_table[8];
__struct_field_types_0x134ea20[1] = &__light_type_table[8];
__struct_field_types_0x134ea20[2] = &__light_type_table[5];
__struct_field_types_0x134ea20[3] = &__light_type_table[86];

__struct_field_types_0x134eb60[0] = &__light_type_table[16];
__struct_field_types_0x134eb60[1] = &__light_type_table[45];

__function_args_types_0x134bd60[0] = &__light_type_table[90];

}

// Declarations

s64( write)(s32 fd, void* buf, u64 count);
void*( memcpy)(void* dst, void* src, u64 size);
u64( print_string)(string s){
(write)(1,(s).data,(s).length);
}
u64( print_test)(void* args){
struct array* a = 0;
a = ((struct array* )args);
u64 count = 0;
count = (a)->length;
struct User_Type_Info* type_info = 0;
type_info = (a)->type_info;
struct User_Type_Value* values = 0;
values = ((struct User_Type_Value* )(a)->data);
label_continue_9:
{
u64 i = 0;
while((i < count)){{
User_Type_Value value = {0};
value = (values[i]);
(print_type)((value).type);
u8( _lit_array_67)[4] = "\n";
string _lit_struct_69 = {0x0, 0x2, ((u8* )_lit_array_67)};
(print_string)(_lit_struct_69);
}
i = (i + 0x1);
}}
label_break_9:;
}
u64( print)(string fmt, void* args){
struct array* a = 0;
a = ((struct array* )args);
u64 count = 0;
count = (a)->length;
struct User_Type_Value* values = 0;
values = ((struct User_Type_Value* )(a)->data);
label_continue_13:
{
u64 i = 0;
while((i < count)){{
User_Type_Value value = {0};
value = (values[i]);
(print_value_literal)(value);
u8( _lit_array_108)[4] = "\n";
string _lit_struct_110 = {0x0, 0x2, ((u8* )_lit_array_108)};
(print_string)(_lit_struct_110);
}
i = (i + 0x1);
}}
label_break_13:;
}
u64( print_bytes)(u8* v, s32 count){
label_continue_17:
{
s32 i = 0;
while((i < count)){{
if ((i > 0)){
u8( _lit_array_134)[4] = ", ";
string _lit_struct_136 = {0x0, 0x2, ((u8* )_lit_array_134)};
(print_string)(_lit_struct_136);
}
u8( _lit_array_143)[4] = "0x";
string _lit_struct_145 = {0x0, 0x2, ((u8* )_lit_array_143)};
(print_string)(_lit_struct_145);
(print_u8)((v[i]),16);
}
i = (i + 1);
}}
label_break_17:;
}
u64( print_value_literal_struct)(User_Type_Value v){
struct User_Type_Info* t = 0;
t = (v).type;
void* val = 0;
val = (v).value;
u8( _lit_array_169)[4] = "{ ";
string _lit_struct_171 = {0x0, 0x2, ((u8* )_lit_array_169)};
(print_string)(_lit_struct_171);
User_Type_Struct s_type = {0};
s_type = ((t)->description).struct_desc;
label_continue_21:
{
s32 i = 0;
while((i < (s_type).fields_count)){{
if ((i > 0)){
u8( _lit_array_194)[4] = ", ";
string _lit_struct_196 = {0x0, 0x2, ((u8* )_lit_array_194)};
(print_string)(_lit_struct_196);
}
struct User_Type_Info* ftype = 0;
ftype = ((s_type).fields_types[i]);
s64 offset = 0;
offset = (((s_type).fields_offsets_bits[i]) / 8);
void* vv = 0;
vv = (val + offset);
User_Type_Value type_val = {0};
User_Type_Value _lit_struct_220 = {vv, ftype};
type_val = _lit_struct_220;
(print_value_literal)(type_val);
}
i = (i + 1);
}}
label_break_21:;
u8( _lit_array_229)[4] = " }";
string _lit_struct_231 = {0x0, 0x2, ((u8* )_lit_array_229)};
(print_string)(_lit_struct_231);
}
u64( print_value_literal_array)(User_Type_Value v){
struct User_Type_Info* t = 0;
t = (v).type;
void* val = 0;
val = (v).value;
User_Type_Array array_type = {0};
array_type = ((t)->description).array_desc;
struct User_Type_Info* element_type = 0;
element_type = (array_type).array_of;
u8( _lit_array_253)[3] = "[";
string _lit_struct_255 = {0x0, 0x1, ((u8* )_lit_array_253)};
(print_string)(_lit_struct_255);
label_continue_25:
{
u64 i = 0;
while((i < (array_type).dimension)){{
if ((i > 0x0)){
u8( _lit_array_274)[4] = ", ";
string _lit_struct_276 = {0x0, 0x2, ((u8* )_lit_array_274)};
(print_string)(_lit_struct_276);
}
void* offset = 0;
offset = (val + (((u64 )(element_type)->type_size_bytes) * i));
User_Type_Value type_val = {0};
User_Type_Value _lit_struct_292 = {offset, element_type};
type_val = _lit_struct_292;
(print_value_literal)(type_val);
}
i = (i + 0x1);
}}
label_break_25:;
u8( _lit_array_301)[3] = "]";
string _lit_struct_303 = {0x0, 0x1, ((u8* )_lit_array_301)};
(print_string)(_lit_struct_303);
}
u64( print_value_literal)(User_Type_Value v){
struct User_Type_Info* t = 0;
t = (v).type;
void* val = 0;
val = (v).value;
if (((t)->kind == 0x1)){
u32 p = 0;
p = ((t)->description).primitive;
if ((p == 0x0)){
}
 else {
if ((p == 0x1)){
s8 l = 0;
l = (*((s8* )val));
(print_s8)(l);
}
 else {
if ((p == 0x2)){
s16 l = 0;
l = (*((s16* )val));
(print_s16)(l);
}
 else {
if ((p == 0x3)){
s32 l = 0;
l = (*((s32* )val));
(print_s32)(l);
}
 else {
if ((p == 0x4)){
s64 l = 0;
l = (*((s64* )val));
(print_s64)(l);
}
 else {
if ((p == 0x5)){
u8 l = 0;
l = (*((u8* )val));
u8( _lit_array_385)[4] = "0x";
string _lit_struct_387 = {0x0, 0x2, ((u8* )_lit_array_385)};
(print_string)(_lit_struct_387);
(print_u8)(l,16);
}
 else {
if ((p == 0x6)){
u16 l = 0;
l = (*((u16* )val));
u8( _lit_array_406)[4] = "0x";
string _lit_struct_408 = {0x0, 0x2, ((u8* )_lit_array_406)};
(print_string)(_lit_struct_408);
(print_u16)(l,16);
}
 else {
if ((p == 0x7)){
u32 l = 0;
l = (*((u32* )val));
u8( _lit_array_427)[4] = "0x";
string _lit_struct_429 = {0x0, 0x2, ((u8* )_lit_array_427)};
(print_string)(_lit_struct_429);
(print_u32)(l,16);
}
 else {
if ((p == 0x8)){
u64 l = 0;
l = (*((u64* )val));
u8( _lit_array_448)[4] = "0x";
string _lit_struct_450 = {0x0, 0x2, ((u8* )_lit_array_448)};
(print_string)(_lit_struct_450);
(print_u64)(l,16);
}
 else {
if ((p == 0x9)){
r32 l = 0;
l = (*((r32* )val));
(print_r32)(l);
}
 else {
if ((p == 0xa)){
r64 l = 0;
l = (*((r64* )val));
(print_r64)(l);
}
 else {
if ((p == 0xb)){
bool l = 0;
l = (*((bool* )val));
if (l){
u8( _lit_array_494)[6] = "true";
string _lit_struct_496 = {0x0, 0x4, ((u8* )_lit_array_494)};
(print_string)(_lit_struct_496);
}
 else {
u8( _lit_array_503)[7] = "false";
string _lit_struct_505 = {0x0, 0x5, ((u8* )_lit_array_503)};
(print_string)(_lit_struct_505);
}

}

}
}
}
}
}
}
}
}
}
}
}
}
 else {
if ((((t)->kind == 0x2) || ((t)->kind == 0x6))){
u64 l = 0;
l = ((u64 )(*((void** )val)));
u8( _lit_array_541)[4] = "0x";
string _lit_struct_543 = {0x0, 0x2, ((u8* )_lit_array_541)};
(print_string)(_lit_struct_543);
(print_u64)(l,16);
}
 else {
if (((t)->kind == 0x3)){
(print_value_literal_struct)(v);
}
 else {
if (((t)->kind == 0x8)){
if ((((((t)->description).alias_desc).alias_to)->kind == 0x3)){
(print_string)((((t)->description).alias_desc).name);
u8( _lit_array_582)[3] = ":";
string _lit_struct_584 = {0x0, 0x1, ((u8* )_lit_array_582)};
(print_string)(_lit_struct_584);
User_Type_Value s = {0};
User_Type_Value _lit_struct_594 = {val, (((t)->description).alias_desc).alias_to};
s = _lit_struct_594;
(print_value_literal_struct)(s);
}

}
 else {
if (((t)->kind == 0x5)){
(print_value_literal_array)(v);
}

}
}
}
}
return 0x0;
}
u64( print_type_primitive)(struct User_Type_Info* t){
u32 p = 0;
p = ((t)->description).primitive;
string str = {0};
u8( _lit_array_626)[2] = "";
string _lit_struct_628 = {0x0, 0x0, ((u8* )_lit_array_626)};
str = _lit_struct_628;
if ((p == 0x0)){
u8( _lit_array_636)[6] = "void";
string _lit_struct_638 = {0x0, 0x4, ((u8* )_lit_array_636)};
str = _lit_struct_638;
}
 else {
if ((p == 0x1)){
u8( _lit_array_647)[4] = "s8";
string _lit_struct_649 = {0x0, 0x2, ((u8* )_lit_array_647)};
str = _lit_struct_649;
}
 else {
if ((p == 0x2)){
u8( _lit_array_658)[5] = "s16";
string _lit_struct_660 = {0x0, 0x3, ((u8* )_lit_array_658)};
str = _lit_struct_660;
}
 else {
if ((p == 0x3)){
u8( _lit_array_669)[5] = "s32";
string _lit_struct_671 = {0x0, 0x3, ((u8* )_lit_array_669)};
str = _lit_struct_671;
}
 else {
if ((p == 0x4)){
u8( _lit_array_680)[5] = "s64";
string _lit_struct_682 = {0x0, 0x3, ((u8* )_lit_array_680)};
str = _lit_struct_682;
}
 else {
if ((p == 0x5)){
u8( _lit_array_691)[4] = "u8";
string _lit_struct_693 = {0x0, 0x2, ((u8* )_lit_array_691)};
str = _lit_struct_693;
}
 else {
if ((p == 0x6)){
u8( _lit_array_702)[5] = "u16";
string _lit_struct_704 = {0x0, 0x3, ((u8* )_lit_array_702)};
str = _lit_struct_704;
}
 else {
if ((p == 0x7)){
u8( _lit_array_713)[5] = "u32";
string _lit_struct_715 = {0x0, 0x3, ((u8* )_lit_array_713)};
str = _lit_struct_715;
}
 else {
if ((p == 0x8)){
u8( _lit_array_724)[5] = "u64";
string _lit_struct_726 = {0x0, 0x3, ((u8* )_lit_array_724)};
str = _lit_struct_726;
}
 else {
if ((p == 0x9)){
u8( _lit_array_735)[5] = "r32";
string _lit_struct_737 = {0x0, 0x3, ((u8* )_lit_array_735)};
str = _lit_struct_737;
}
 else {
if ((p == 0xa)){
u8( _lit_array_746)[5] = "r64";
string _lit_struct_748 = {0x0, 0x3, ((u8* )_lit_array_746)};
str = _lit_struct_748;
}
 else {
if ((p == 0xb)){
u8( _lit_array_757)[6] = "bool";
string _lit_struct_759 = {0x0, 0x4, ((u8* )_lit_array_757)};
str = _lit_struct_759;
}

}
}
}
}
}
}
}
}
}
}
}
return (print_string)(str);
}
u64( print_type_pointer)(struct User_Type_Info* t){
u64 length = 0;
u8( _lit_array_784)[3] = "^";
string _lit_struct_786 = {0x0, 0x1, ((u8* )_lit_array_784)};
length = (print_string)(_lit_struct_786);
length = (length + (print_type)(((t)->description).pointer_to));
return length;
}
u64( print_type_struct)(struct User_Type_Info* t){
User_Type_Struct s = {0};
s = ((t)->description).struct_desc;
u64 length = 0;
u8( _lit_array_809)[11] = "struct { ";
string _lit_struct_811 = {0x0, 0x9, ((u8* )_lit_array_809)};
length = (print_string)(_lit_struct_811);
label_continue_66:
{
s32 i = 0;
while((i < (s).fields_count)){{
if ((i > 0)){
u8( _lit_array_831)[4] = "; ";
string _lit_struct_833 = {0x0, 0x2, ((u8* )_lit_array_831)};
length = (length + (print_string)(_lit_struct_833));
}
length = (length + (print_string)(((s).fields_names[i])));
u8( _lit_array_851)[5] = " : ";
string _lit_struct_853 = {0x0, 0x3, ((u8* )_lit_array_851)};
length = (length + (print_string)(_lit_struct_853));
length = (length + (print_type)(((s).fields_types[i])));
}
i = (i + 1);
}}
label_break_66:;
u8( _lit_array_872)[4] = " }";
string _lit_struct_874 = {0x0, 0x2, ((u8* )_lit_array_872)};
length = (length + (print_string)(_lit_struct_874));
return length;
}
u64( print_type_union)(struct User_Type_Info* t){
User_Type_Union s = {0};
s = ((t)->description).union_desc;
u64 length = 0;
u8( _lit_array_890)[10] = "union { ";
string _lit_struct_892 = {0x0, 0x8, ((u8* )_lit_array_890)};
length = (print_string)(_lit_struct_892);
label_continue_70:
{
s32 i = 0;
while((i < (s).fields_count)){{
if ((i > 0)){
u8( _lit_array_912)[4] = "; ";
string _lit_struct_914 = {0x0, 0x2, ((u8* )_lit_array_912)};
length = (length + (print_string)(_lit_struct_914));
}
length = (length + (print_string)(((s).fields_names[i])));
u8( _lit_array_932)[5] = " : ";
string _lit_struct_934 = {0x0, 0x3, ((u8* )_lit_array_932)};
length = (length + (print_string)(_lit_struct_934));
length = (length + (print_type)(((s).fields_types[i])));
}
i = (i + 1);
}}
label_break_70:;
u8( _lit_array_953)[4] = " }";
string _lit_struct_955 = {0x0, 0x2, ((u8* )_lit_array_953)};
length = (length + (print_string)(_lit_struct_955));
return length;
}
u64( print_type_alias)(struct User_Type_Info* t){
User_Type_Alias s = {0};
s = ((t)->description).alias_desc;
u64 length = 0;
length = (print_string)((s).name);
u8( _lit_array_977)[3] = " ";
string _lit_struct_979 = {0x0, 0x1, ((u8* )_lit_array_977)};
length = (length + (print_string)(_lit_struct_979));
length = (length + (print_type)((s).alias_to));
return length;
}
u64( print_type_array)(struct User_Type_Info* t){
User_Type_Array s = {0};
s = ((t)->description).array_desc;
u64 length = 0;
u8( _lit_array_1002)[3] = "[";
string _lit_struct_1004 = {0x0, 0x1, ((u8* )_lit_array_1002)};
length = (print_string)(_lit_struct_1004);
length = (length + (print_u64)((s).dimension,10));
u8( _lit_array_1019)[3] = "]";
string _lit_struct_1021 = {0x0, 0x1, ((u8* )_lit_array_1019)};
length = (length + (print_string)(_lit_struct_1021));
length = (length + (print_type)((s).array_of));
return length;
}
u64( print_type_function)(struct User_Type_Info* t){
User_Type_Function s = {0};
s = ((t)->description).function_desc;
u64 length = 0;
u8( _lit_array_1044)[3] = "(";
string _lit_struct_1046 = {0x0, 0x1, ((u8* )_lit_array_1044)};
length = (print_string)(_lit_struct_1046);
label_continue_78:
{
s32 i = 0;
while((i < (s).arguments_count)){{
if ((i > 0)){
u8( _lit_array_1066)[4] = ", ";
string _lit_struct_1068 = {0x0, 0x2, ((u8* )_lit_array_1066)};
length = (length + (print_string)(_lit_struct_1068));
}
struct User_Type_Info* arg = 0;
arg = ((s).arguments_type[i]);
length = (length + (print_type)(arg));
}
i = (i + 1);
}}
label_break_78:;
u8( _lit_array_1090)[7] = ") -> ";
string _lit_struct_1092 = {0x0, 0x5, ((u8* )_lit_array_1090)};
length = (length + (print_string)(_lit_struct_1092));
length = (length + (print_type)((s).return_type));
return length;
}
u64( print_type)(struct User_Type_Info* t){
u64 length = 0;
length = 0x0;
if (((t)->kind == 0x1)){
length = (length + (print_type_primitive)(t));
}
 else {
if (((t)->kind == 0x2)){
length = (length + (print_type_pointer)(t));
}
 else {
if (((t)->kind == 0x3)){
length = (length + (print_type_struct)(t));
}
 else {
if (((t)->kind == 0x4)){
length = (length + (print_type_union)(t));
}
 else {
if (((t)->kind == 0x8)){
length = (length + (print_type_alias)(t));
}
 else {
if (((t)->kind == 0x5)){
length = (length + (print_type_array)(t));
}
 else {
if (((t)->kind == 0x6)){
length = (length + (print_type_function)(t));
}

}
}
}
}
}
}
return length;
}
u64( u64_to_str)(u64 val, u8( buffer)[32]){
u8( b)[32] = {0};
s32 sum = 0;
if ((val == 0x0)){
(buffer[0]) = 0x30;
return 0x1;
}

u8* auxbuffer = 0;
auxbuffer = (&(b[32]));
u8* start = 0;
start = (auxbuffer + sum);
label_continue_92:
while ((val != 0x0)) {
{
u64 rem = 0;
rem = (val % 0xa);
val = (val / 0xa);
(*auxbuffer) = (0x30 + ((u8 )rem));
auxbuffer = (auxbuffer - 1);
}
}
label_break_92:;
s64 size = 0;
size = (start - auxbuffer);
(memcpy)((&(buffer[sum])),(auxbuffer + 1),((u64 )size));
return ((u64 )size);
}
u64( s64_to_str)(s64 val, u8( buffer)[32]){
u8( b)[32] = {0};
s32 sum = 0;
if ((val == 0)){
(buffer[0]) = 0x30;
return 0x1;
}

if ((val < 0)){
val = (-val);
(buffer[0]) = 0x2d;
sum = 1;
}

u8* auxbuffer = 0;
auxbuffer = (&(b[32]));
u8* start = 0;
start = (auxbuffer + sum);
label_continue_97:
while ((val != 0)) {
{
s64 rem = 0;
rem = (val % 10);
val = (val / 10);
(*auxbuffer) = (0x30 + ((u8 )rem));
auxbuffer = (auxbuffer - 1);
}
}
label_break_97:;
s64 size = 0;
size = (start - auxbuffer);
(memcpy)((&(buffer[sum])),(auxbuffer + 1),((u64 )size));
return ((u64 )size);
}
u64( unsigned_to_str_base16)(u64 value, u64 bitsize, bool leading_zeros, bool capitalized, u8( buffer)[16]){
u64 i = 0;
i = 0x0;
u64 mask = 0;
mask = (0xf000000000000000 >> (0x40 - bitsize));
s32 cap = 0;
cap = 87;
if (capitalized){
cap = 55;
}

label_continue_101:
{
while((i < (bitsize / 0x4))){{
u64 f = 0;
f = ((value & mask) >> (bitsize - 0x4));
if ((f > 0x9)){
(buffer[i]) = (((u8 )f) + 0x57);
} else {
(buffer[i]) = (((u8 )f) + 0x30);
}
value = (value << 0x4);
}
i = (i + 0x1);
}}
label_break_101:;
return i;
}
u64( print_s8)(s8 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(((s64 )value),buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_s16)(s16 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(((s64 )value),buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_s32)(s32 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(((s64 )value),buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_s64)(s64 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(value,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u8)(u8 value, s32 base){
u8( buffer)[16] = {0};
u64 length = 0;
length = (unsigned_to_str_base16)(((u64 )value),0x8,true,false,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u16)(u16 value, s32 base){
u8( buffer)[16] = {0};
u64 length = 0;
length = (unsigned_to_str_base16)(((u64 )value),0x10,true,false,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u32)(u32 value, s32 base){
u8( buffer)[16] = {0};
u64 length = 0;
length = (unsigned_to_str_base16)(((u64 )value),0x20,true,false,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u64)(u64 value, s32 base){
u64 length = 0;
if ((base == 16)){
u8( buffer)[16] = {0};
length = (unsigned_to_str_base16)(value,0x40,true,false,buffer);
(write)(1,((void* )buffer),length);
}
 else {
if ((base == 10)){
u8( buffer)[32] = {0};
length = (u64_to_str)(value,buffer);
(write)(1,((void* )buffer),length);
}

}
return length;
}
u64( r64_to_str)(r64 v, u8( buffer)[64]){
u64 l = 0;
l = 0x0;
if ((v < 0.000000)){
(buffer[l]) = 0x2d;
l = (l + 0x1);
v = (-v);
}

l = (l + (s64_to_str)(((s64 )v),((u8* )(((u8* )buffer) + l))));
s32 precision = 0;
precision = 6;
r64 fractional_part = 0;
fractional_part = (v - ((r64 )((s64 )v)));
(buffer[l]) = 0x2e;
l = (l + 0x1);
if ((fractional_part == 0.000000)){
(buffer[l]) = 0x30;
l = (l + 0x1);
return l;
}

label_continue_125:
while (((precision > 0) && (fractional_part > 0.000000))) {
{
fractional_part = (fractional_part * 10.000000);
(buffer[l]) = (((u8 )fractional_part) + 0x30);
fractional_part = (fractional_part - ((r64 )((s64 )fractional_part)));
l = (l + 0x1);
precision = (precision - 1);
}
}
label_break_125:;
return l;
}
u64( r32_to_str)(r32 v, u8( buffer)[32]){
u64 l = 0;
l = 0x0;
if ((v < 0.000000)){
(buffer[l]) = 0x2d;
l = (l + 0x1);
v = (-v);
}

l = (l + (s64_to_str)(((s64 )v),((u8* )(((u8* )buffer) + l))));
s32 precision = 0;
precision = 6;
r32 fractional_part = 0;
fractional_part = (v - ((r32 )((s32 )v)));
(buffer[l]) = 0x2e;
l = (l + 0x1);
if ((fractional_part == 0.000000)){
(buffer[l]) = 0x30;
l = (l + 0x1);
return l;
}

label_continue_130:
while (((precision > 0) && (fractional_part > 0.000000))) {
{
fractional_part = (fractional_part * 10.000000);
(buffer[l]) = (((u8 )fractional_part) + 0x30);
fractional_part = (fractional_part - ((r32 )((s32 )fractional_part)));
l = (l + 0x1);
precision = (precision - 1);
}
}
label_break_130:;
return l;
}
u64( print_r32)(r32 v){
u8( buffer)[32] = {0};
u64 length = 0;
length = (r32_to_str)(v,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_r64)(r64 v){
u8( buffer)[64] = {0};
u64 length = 0;
length = (r64_to_str)(v,buffer);
(write)(1,((void* )buffer),length);
return length;
}
s32( __light_main)(){
s8 a = 0;
a = -23;
s16 b = 0;
b = 1231;
s32 c = 0;
c = 43332424;
s64 d = 0;
d = 1234567891011;
u8 e = 0;
e = 0xe9;
u16 f = 0;
f = 0x4cf;
u32 g = 0;
g = 0x2953348;
u64 h = 0;
h = 0x11f71fb0843;
r32 i = 0;
i = 3.140000;
r32 j = 0;
j = 2.222340;
u8( _lit_array_1912)[5] = "foo";
string _lit_struct_1914 = {0x0, 0x3, ((u8* )_lit_array_1912)};
s32( _lit_array_1919)[2] = {1, 2};
s32(*( _lit_array_2018)[1])[2] = {(&_lit_array_1919)};
User_Type_Value _lit_struct_2021 = {(&_lit_array_2018), &__light_type_table[72]};
s32(*( _lit_array_2022)[1])() = {__light_main};
User_Type_Value _lit_struct_2025 = {(&_lit_array_2022), &__light_type_table[42]};
struct User_Type_Value( _lit_array_2029)[2] = {_lit_struct_2021, _lit_struct_2025};
array _lit_struct_2031 = {0x2, 0x2, &__light_type_table[45], ((void* )_lit_array_2029)};
(print)(_lit_struct_1914,(&_lit_struct_2031));
return 0;
}
int main() { __light_load_type_table(); return __light_main(); }
