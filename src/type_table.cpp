#include "type_table.h"
#include "util.h"
#include "memory.h"

#define ALLOC_TYPE(ARENA) (Type_Instance*)(ARENA).allocate(sizeof(Type_Instance))

static Memory_Arena types_internal(65536);
static Memory_Arena types_temporary(65536);

static Type_Instance* type_s8;
static Type_Instance* type_s16;
static Type_Instance* type_s32;
static Type_Instance* type_s64;
static Type_Instance* type_u8;
static Type_Instance* type_u16;
static Type_Instance* type_u32;
static Type_Instance* type_u64;
static Type_Instance* type_r32;
static Type_Instance* type_r64;
static Type_Instance* type_bool;
static Type_Instance* type_void;

inline Type_Instance* type_setup_primitive(Type_Primitive p);

constexpr u64 type_primitive_hash(Type_Primitive p) {
	switch (p) {
		case TYPE_PRIMITIVE_S64:
			return fnv_1_hash((const u8*)"s64", sizeof("s64") - 1);
		case TYPE_PRIMITIVE_S32:
			return fnv_1_hash((const u8*)"s32", sizeof("s32") - 1);
		case TYPE_PRIMITIVE_S16:
			return fnv_1_hash((const u8*)"s16", sizeof("s16") - 1);
		case TYPE_PRIMITIVE_S8:
			return fnv_1_hash((const u8*)"s8", sizeof("s8") - 1);
		case TYPE_PRIMITIVE_U64:
			return fnv_1_hash((const u8*)"u64", sizeof("u64") - 1);
		case TYPE_PRIMITIVE_U32:
			return fnv_1_hash((const u8*)"u32", sizeof("u32") - 1);
		case TYPE_PRIMITIVE_U16:
			return fnv_1_hash((const u8*)"u16", sizeof("u16") - 1);
		case TYPE_PRIMITIVE_U8:
			return fnv_1_hash((const u8*)"u8", sizeof("u8") - 1);
		case TYPE_PRIMITIVE_R32:
			return fnv_1_hash((const u8*)"r32", sizeof("r32") - 1);
		case TYPE_PRIMITIVE_R64:
			return fnv_1_hash((const u8*)"r64", sizeof("r64") - 1);
		case TYPE_PRIMITIVE_BOOL:
			return fnv_1_hash((const u8*)"bool", sizeof("bool") - 1);
		case TYPE_PRIMITIVE_VOID:
			return fnv_1_hash((const u8*)"void", sizeof("void") - 1);
	}
}

u64 type_hash(Type_Instance* type) {
	switch (type->kind) {
		case KIND_PRIMITIVE:
			return type_primitive_hash(type->primitive);
		case KIND_POINTER:
			return fnv_1_hash_from_start(type_hash(type->pointer_to), (const u8*)"pointer", sizeof("pointer") - 1);
		case KIND_STRUCT:
			return fnv_1_hash(type->struct_desc.name->value.data, type->struct_desc.name->value.length);
		case KIND_FUNCTION: {
			u64 return_type_hash = type_hash(type->function_desc.return_type);
			size_t n_args = array_get_length(type->function_desc.arguments_type);
			for (size_t i = 0; i < n_args; ++i) {
				return_type_hash = fnv_1_hash_combine(return_type_hash, type_hash(type->function_desc.arguments_type[i]));
			}
			return return_type_hash;
		} break;
		case KIND_ARRAY:
			return fnv_1_hash_combine(type_hash(type->array_desc.array_of), type->array_desc.dimension);
		default:
			assert(0);
	}
	return 0;
}

static Hash_Table type_table;

void type_table_init() {
	// TODO(psv): no compare function between types yet
	hash_table_init(&type_table, 1024 * 1024 * 4, (hash_function_type*)type_hash, (hash_entries_equal_type*)0);

	type_s64 = type_setup_primitive(TYPE_PRIMITIVE_S64);
	type_s32 = type_setup_primitive(TYPE_PRIMITIVE_S32);
	type_s16 = type_setup_primitive(TYPE_PRIMITIVE_S16);
	type_s8 = type_setup_primitive(TYPE_PRIMITIVE_S8);
	type_u64 = type_setup_primitive(TYPE_PRIMITIVE_U64);
	type_u32 = type_setup_primitive(TYPE_PRIMITIVE_U32);
	type_u16 = type_setup_primitive(TYPE_PRIMITIVE_U16);
	type_u8 = type_setup_primitive(TYPE_PRIMITIVE_U8);
	type_r32 = type_setup_primitive(TYPE_PRIMITIVE_R32);
	type_r64 = type_setup_primitive(TYPE_PRIMITIVE_R64);
	type_bool = type_setup_primitive(TYPE_PRIMITIVE_BOOL);
	type_void = type_setup_primitive(TYPE_PRIMITIVE_VOID);
}

// Deep copy of types, this function follow the pointers of all Type_Instance* in the type description
Type_Instance* type_copy_internal(Type_Instance* type) {
	Type_Instance* result = ALLOC_TYPE(types_internal);
	memcpy(result, type, sizeof(Type_Instance));

	switch (type->kind) {
	case KIND_PRIMITIVE: break;
	case KIND_POINTER:
		result->pointer_to = type_copy_internal(type->pointer_to);
		break;
	case KIND_STRUCT: {
		size_t num_args = array_get_length(type->struct_desc.fields_types);
		result->struct_desc.fields_types = (Type_Instance**)types_internal.allocate(array_get_header_size() + num_args * sizeof(Type_Instance*));
		result->struct_desc.fields_types = (Type_Instance**)((u8*)result->struct_desc.fields_types + array_get_header_size());
		array_set_capacity(result->struct_desc.fields_types, num_args);
		array_set_length(result->struct_desc.fields_types, num_args);
		array_set_element_size(result->struct_desc.fields_types, sizeof(Type_Instance*));
		for (size_t i = 0; i < num_args; ++i) {
			result->struct_desc.fields_types[i] = type_copy_internal(type->struct_desc.fields_types[i]);
		}
	}break;
	case KIND_FUNCTION: {
		result->function_desc.return_type = type_copy_internal(type->function_desc.return_type);
		size_t num_args = type->function_desc.num_arguments;
		result->function_desc.arguments_type = (Type_Instance**)types_internal.allocate(array_get_header_size() + num_args * sizeof(Type_Instance*));
		result->function_desc.arguments_type = (Type_Instance**)((u8*)result->function_desc.arguments_type + array_get_header_size());
		array_set_capacity(result->function_desc.arguments_type, num_args);
		array_set_length(result->function_desc.arguments_type, num_args);
		array_set_element_size(result->function_desc.arguments_type, sizeof(Type_Instance*));
		for (size_t i = 0; i < num_args; ++i) {
			result->function_desc.arguments_type[i] = type_copy_internal(type->function_desc.arguments_type[i]);
		}
	}break;
	case KIND_ARRAY:
		result->array_desc.array_of = type_copy_internal(type->array_desc.array_of);
		break;
	default:
		assert(0);
	}
}

void internalize_type(Type_Instance** type, bool copy) {
	assert((*type)->flags & TYPE_INSTANCE_RESOLVED);
	u64 hash = type_hash(*type);

	s64 index = hash_table_entry_exist(&type_table, *type, hash);

	if (index == -1) {
		if (copy) {
			// copy into internal mem space
			Type_Instance* copied = type_copy_internal(*type);
			index = hash_table_add(&type_table, copied, sizeof(copied), hash);
		} else {
			index = hash_table_add(&type_table, *type, sizeof(*type), hash);
		}
	}
	*type = (Type_Instance*)type_table.entries[index].data;
	(*type)->flags |= TYPE_INSTANCE_INTERNALIZED;
}

inline Type_Instance* type_setup_primitive(Type_Primitive p) {
	Type_Instance* res = ALLOC_TYPE(types_internal);
	res->kind = KIND_PRIMITIVE;
	res->flags = TYPE_INSTANCE_RESOLVED | TYPE_INSTANCE_INTERNALIZED | TYPE_INSTANCE_SIZE_RESOLVED;
	res->primitive = p;
	switch (p) {
		case TYPE_PRIMITIVE_S64:  res->type_size_bits = 64; break;
		case TYPE_PRIMITIVE_S32:  res->type_size_bits = 32; break;
		case TYPE_PRIMITIVE_S16:  res->type_size_bits = 16; break;
		case TYPE_PRIMITIVE_S8:   res->type_size_bits = 8; break;
		case TYPE_PRIMITIVE_U64:  res->type_size_bits = 64; break;
		case TYPE_PRIMITIVE_U32:  res->type_size_bits = 32; break;
		case TYPE_PRIMITIVE_U16:  res->type_size_bits = 16; break;
		case TYPE_PRIMITIVE_U8:   res->type_size_bits = 8; break;
		case TYPE_PRIMITIVE_R32:  res->type_size_bits = 32; break;
		case TYPE_PRIMITIVE_R64:  res->type_size_bits = 64; break;
		case TYPE_PRIMITIVE_BOOL: res->type_size_bits = 8; break;
		case TYPE_PRIMITIVE_VOID: res->type_size_bits = 0; break;
	}
	internalize_type(&res, false);
	return res;
}

Type_Instance* type_primitive_get(Type_Primitive p) {
	switch (p) {
	case TYPE_PRIMITIVE_S64:  return type_s64;
	case TYPE_PRIMITIVE_S32:  return type_s32;
	case TYPE_PRIMITIVE_S16:  return type_s16;
	case TYPE_PRIMITIVE_S8:   return type_s8;
	case TYPE_PRIMITIVE_U64:  return type_u64;
	case TYPE_PRIMITIVE_U32:  return type_u32;
	case TYPE_PRIMITIVE_U16:  return type_u16;
	case TYPE_PRIMITIVE_U8:   return type_u8;
	case TYPE_PRIMITIVE_R32:  return type_r32;
	case TYPE_PRIMITIVE_R64:  return type_r64;
	case TYPE_PRIMITIVE_BOOL: return type_bool;
	case TYPE_PRIMITIVE_VOID: return type_void;
	default: report_internal_compiler_error(__FILE__, __LINE__, "tried to get unknown primitive type from type table\n"); break;
	}
}

Type_Instance* type_new_temporary() {
	return ALLOC_TYPE(types_temporary);
}

s64 type_pointer_size() {
	return 8;
}