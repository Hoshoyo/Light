#include "type.h"
#include "lexer.h"
#include "util.h"

s32 get_size_of_primitive_type(Type_Primitive primitive);

Type_Instance* get_primitive_type(Type_Primitive primitive_type)
{
	Type_Instance* ti = new Type_Instance();
	ti->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
	ti->type = TYPE_PRIMITIVE;
	ti->primitive = primitive_type;
	ti->type_size = get_size_of_primitive_type(primitive_type);
	create_type(&ti, true);
	return ti;
}

Type_Instance* create_ptr_typeof(Type_Instance* inst)
{
	Type_Instance* ti = new Type_Instance();
	ti->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
	ti->type = TYPE_POINTER;
	ti->pointer_to = inst;
	ti->type_size = get_size_of_pointer();
	create_type(&ti, true);
	return ti;
}

s32 get_size_of_pointer() {
	return 8;
}

s32 get_size_of_primitive_type(Type_Primitive primitive) {
	switch (primitive) {
	case TYPE_PRIMITIVE_S64:	return 8;
	case TYPE_PRIMITIVE_S32:	return 4;
	case TYPE_PRIMITIVE_S16:	return 2;
	case TYPE_PRIMITIVE_S8:		return 1;
	case TYPE_PRIMITIVE_U64:	return 8;
	case TYPE_PRIMITIVE_U32:	return 4;
	case TYPE_PRIMITIVE_U16:	return 2;
	case TYPE_PRIMITIVE_U8:		return 1;
	case TYPE_PRIMITIVE_BOOL:	return 1;
	case TYPE_PRIMITIVE_R64:	return 8;
	case TYPE_PRIMITIVE_R32:	return 4;
	case TYPE_PRIMITIVE_VOID:	return 0;
	}
}
Type_Table type_table;
static s64 DEBUG_type_entries[1024] = { 0 };
static s64 DEBUG_type_index = 0;

u32 Type_Table::type_hash(Type_Instance* instance)
{
	switch (instance->type) {
	case TYPE_PRIMITIVE: {
		u32 hash = djb2_hash((u8*)&instance->primitive, sizeof(Type_Primitive));
		return hash % max_entries;
	}break;
		// --
	case TYPE_POINTER: {
		u32 hash = type_hash(instance->pointer_to);
		hash = djb2_hash(hash, (u8*)&instance->type, sizeof(u32));
		return hash % max_entries;
	}break;
		// --
	case TYPE_ARRAY: {

	}break;
		// --
	case TYPE_STRUCT: {
		u32 hash = djb2_hash((u8*)instance->type_struct.name, instance->type_struct.name_length);
		return hash % max_entries;
	}break;
		// --
	case TYPE_FUNCTION: {
		u32 hash = type_hash(instance->type_function.return_type);
		int num_args = instance->type_function.num_arguments;
		for (int i = 0; i < num_args; ++i) {
			Type_Instance* argtype = instance->type_function.arguments_type[i];
			hash = djb2_hash(hash, type_hash(argtype));
		}
		return hash % max_entries;
	}break;
	}
}

bool types_equal(Type_Instance* i1, Type_Instance* i2)
{
	switch (i1->type) {
		case TYPE_PRIMITIVE: {
			if (i2->type != TYPE_PRIMITIVE) return false;
			if (i1->primitive != i2->primitive) return false;
			return true;
		}break;
			// --
		case TYPE_POINTER: {
			if (i2->type != TYPE_POINTER) return false;
			if (types_equal(i1->pointer_to, i2->pointer_to)) return true;
			return false;
		}break;
			// --
		case TYPE_ARRAY: {
			assert(0);	// @todo
		}break;
			// --
		case TYPE_STRUCT: {
			return str_equal(i1->type_struct.name, i1->type_struct.name_length, i2->type_struct.name, i2->type_struct.name_length);
		}break;
			// --
		case TYPE_FUNCTION: {
			if (!types_equal(i1->type_function.return_type, i2->type_function.return_type)) return false;
			int num_args = i1->type_function.num_arguments;
			if (num_args != i2->type_function.num_arguments) return false;
			for (int i = 0; i < num_args; ++i) {
				if (!types_equal(i1->type_function.arguments_type[i], i2->type_function.arguments_type[i])) return false;
			}
			return true;
		}break;
	}
}

bool Type_Table::entry_exist(Type_Instance* instance, s64* hash)
{
	//assert(instance->flags & TYPE_FLAG_IS_REGISTER_SIZE);
	//assert(instance->flags & TYPE_FLAG_IS_RESOLVED);
	//assert(instance->flags & TYPE_FLAG_IS_SIZE_RESOLVED);

	u32 h = type_table.type_hash(instance);

	if (type_table.entries[h].used) {
		// might be the same
		if (types_equal(instance, type_table.entries[h].entry)) {
			if (hash) *hash = h;
			return true;
		} else if(type_table.entries[h].collided) {
			// continue searching
			do {
				h += 1;
				if (h == type_table.max_entries) h = 0;
				if (types_equal(instance, type_table.entries[h].entry)) {
					if (hash) *hash = h;
					return true;
				}
			} while (type_table.entries[h].collided);
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Type_Table::insert_type(Type_Instance* instance, s64* hash)
{
	u32 h = type_table.type_hash(instance);

	while (type_table.entries[h].used) {
		type_table.entries[h].collided = true;
		h += 1;
		if (h == max_entries) h = 0;
	}
	instance->flags |= TYPE_FLAG_NOT_DELETE;
	type_table.entries[h].entry = instance;
	type_table.entries[h].used = true;

	DEBUG_type_entries[DEBUG_type_index++] = h;

	if (hash) *hash = h;

	return true;
}

Type_Instance* Type_Table::get_entry(s64 hash)
{
	return entries[hash].entry;
}

void delete_type(Type_Instance* instance) 
{
	if (instance->flags & TYPE_FLAG_NOT_DELETE) return;

	switch (instance->type) {
		case TYPE_PRIMITIVE: {
			delete instance;
		}break;
			// --
		case TYPE_POINTER: {
			delete_type(instance->pointer_to);
			delete instance;
		}break;
			// --
		case TYPE_ARRAY: {
			assert(0);	// @todo
		}break;
			// --
		case TYPE_STRUCT: {
			delete instance;
		}break;

		case TYPE_FUNCTION: {
			int num_args = instance->type_function.num_arguments;
			for (int i = 0; i < num_args; ++i) {
				delete_type(instance->type_function.arguments_type[i]);
			}
			delete_type(instance->type_function.return_type);
			delete instance;
		}break;
	}
}

s64 create_type(Type_Instance** instance, bool swap_and_delete)
{
	s64 index = 0;
	if (!type_table.entry_exist(*instance, &index)) {
		// if there is no entry for this type, then fill it
		type_table.insert_type(*instance, &index);
	} else {
		// if there is then swap it
		if (swap_and_delete && type_table.entries[index].entry != *instance) {
			delete_type(*instance);
			*instance = type_table.entries[index].entry;
		}
	}
	return index;
}
#include "ast.h"
void DEBUG_print_type_table()
{
	printf("The type table has %d entries.\n", DEBUG_type_index);
	for (int i = 0; i < DEBUG_type_index; ++i) {
		s64 hash = DEBUG_type_entries[i];
		printf("entry[%d/%u]: %p\n", i, hash, type_table.get_entry(hash));
		DEBUG_print_type(stdout, type_table.get_entry(hash));
		printf("\n");
	}
}