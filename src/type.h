#pragma once
#include "util.h"
#include "lexer.h"

struct Ast;

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
			Type_Instance* struct_descriptor;
		} type_struct;
	};

	Type_Instance() {
		flags = 0;
		type = TYPE_UNKNOWN;
	}
};

s32 get_size_of_pointer();

s32 get_size_of_primitive_type(Type_Primitive primitive);

struct Type_Hash_Entry {
	Type_Instance* entry;
	bool used;
	bool collided;
};

struct Type_Hash {
	Type_Hash_Entry* entries;
	int max_entries;
	int num_entries;
	int num_collisions;
	
	Type_Hash() {
		entries = (Type_Hash_Entry*)calloc(1, 1024 * 1024 * sizeof(Type_Hash_Entry));
		max_entries = 1024 * 1024;
		num_entries = 0;
		num_collisions = 0;
	}
	/*
	u32 hash_of_type(Type_Instance* type) {
		int extra_size = ((s64)&((Type_Instance*)0)->descriptor) - ((s64)&((Type_Instance*)0)->type);
		u32 hash = djb2_hash((u8*)&type->type, type->size_of_descriptor + extra_size) % max_entries;
		return hash;
	}

	s64 entry_exist(Type_Instance* type) {
		u32 hash = hash_of_type(type);
		if (entries[hash].used) {
			if (memcmp(entries[hash].entry, type, entry_size) == 0) {
				return (s64)hash;
			} else {
				while (entries[hash].collided) {
					hash += 1;
					if (hash >= max_entries) hash = 0;
					if (memcmp(entries[hash].entry, type, entry_size) == 0) {
						return (s64)hash;
					}
				}
			}
		}
		return -1;
	}

	Type_Instance* insert(Type_Instance* type) {
		if (num_entries == max_entries) return 0;

		u32 hash = hash_of_type(type);
		if (!entries[hash].used) {
			int entry_size = 0;
			bool collided = false;
			while (entries[hash].used) {
				collided = true;
				hash += 1;
				num_collisions += 1;
				if (hash >= max_entries) hash = 0;
			}
			entries[hash].used = true;
			entries[hash].collided = collided;
			entries[hash].entry = type;
		}
	}
	*/
};

extern Type_Hash* type_hash_table;
extern Type_Instance* DEBUG_types_inserted[1024];
extern int DBG_index;

struct Type_Allocator {

};

static Type_Allocator type_allocator = {};

Type_Instance* get_primitive_type(Type_Primitive primitive_type);