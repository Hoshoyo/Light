#pragma once
#include "util.h"
#include "lexer.h"

struct Ast;
struct Scope;

enum Type_Primitive
{
	TYPE_PRIMITIVE_UNKNOWN,

	TYPE_NOT_PRIMITIVE,

	TYPE_PRIMITIVE_S64,
	TYPE_PRIMITIVE_S32,
	TYPE_PRIMITIVE_S16,
	TYPE_PRIMITIVE_S8,
	TYPE_PRIMITIVE_U64,
	TYPE_PRIMITIVE_U32,
	TYPE_PRIMITIVE_U16,
	TYPE_PRIMITIVE_U8,

	TYPE_PRIMITIVE_BOOL,

	TYPE_PRIMITIVE_R64,
	TYPE_PRIMITIVE_R32,

	TYPE_PRIMITIVE_VOID,

	NUM_PRIMITIVE_TYPES,
};

/*
Flags
*/
const u32 TYPE_FLAG_IS_REGISTER_SIZE = FLAG(0);
const u32 TYPE_FLAG_IS_RESOLVED = FLAG(1);
const u32 TYPE_FLAG_IS_SIZE_RESOLVED = FLAG(2);
const u32 TYPE_FLAG_ARRAY_STATIC = FLAG(3);
const u32 TYPE_FLAG_ARRAY_DYNAMIC = FLAG(4);
const u32 TYPE_FLAG_QUEUED = FLAG(5);
const u32 TYPE_FLAG_NOT_DELETE = FLAG(6);

/*
Types
*/
const u32 TYPE_UNKNOWN = 0;
const u32 TYPE_PRIMITIVE = 1;
const u32 TYPE_POINTER = 2;
const u32 TYPE_FUNCTION = 3;
const u32 TYPE_ARRAY = 4;
const u32 TYPE_STRUCT = 5;

struct Type_Instance {
	u32 flags;
	u32 type;
	s32 type_size;
	union {
		Type_Primitive primitive;
		Type_Instance* pointer_to;
		struct Type_Function {
			int num_arguments;
			Type_Instance* return_type;
			Type_Instance** arguments_type;
		} type_function;
		struct Type_Array {
			Type_Instance* array_of;
			int num_dimensions;
			Ast** dimensions_sizes;
		} type_array;
		struct Type_Struct {
			char* name;
			int name_length;
			Type_Instance** fields_types;
			string* fields_names;
			Ast* struct_descriptor;
		} type_struct;
	};

	Type_Instance() {
		flags = 0;
		type = TYPE_UNKNOWN;
	}
};

struct Type_Table_Entry {
	Type_Instance* entry;
	bool used;
	bool collided;
};

struct Type_Table {
	Type_Table_Entry* entries;
	int max_entries;
	int num_entries;
	int num_collisions;
	
	Type_Table() {
		entries = (Type_Table_Entry*)calloc(1, 1024 * 1024 * sizeof(Type_Table_Entry));
		max_entries = 1024 * 1024;
		num_entries = 0;
		num_collisions = 0;
	}

	u32 type_hash(Type_Instance* instance);
	bool entry_exist(Type_Instance* instance, s64* hash);
	bool insert_type(Type_Instance* instance, s64* hash);
	Type_Instance* get_entry(s64 hash);
};

extern Type_Table type_table;

s32 get_size_of_pointer();
s32 get_size_of_primitive_type(Type_Primitive primitive);
Type_Instance* get_primitive_type(Type_Primitive primitive_type);
Type_Instance* get_string_type();

void initialize_types(Scope* global_scope, Ast** ast);

// creates a type entry on the table and returns its index
s64 create_type(Type_Instance** instance, bool swap_and_delete);
Type_Instance* create_ptr_typeof(Type_Instance* inst);
bool types_equal(Type_Instance* i1, Type_Instance* i2);

const int INTEGER_SIGNED = 1;
const int INTEGER_UNSIGNED = 2;
// return 1 if signed 2 if unsigned 0 if not integer type
int is_integer_type(Type_Instance* inst);
int is_floating_point_type(Type_Instance* inst);
int is_boolean_type(Type_Instance* inst);
int is_pointer_type(Type_Instance* inst);

s32 get_size_of_primitive_type(Type_Primitive primitive);

void DEBUG_print_type_table();
void DEBUG_print_node_type(FILE* out, Ast** ast, bool decl_only);