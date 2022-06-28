#include "type.h"
#include <assert.h>
#include "global_tables.h"
#include "light_array.h"

static u64 primitive_table_hash[TYPE_PRIMITIVE_COUNT] = {0};
static u64 struct_hash = 0;
static u64 union_hash = 0;
static u64 proc_hash = 0;
static u64 pointer_hash = 0;
static u64 enum_hash = 0;

static Light_Type* primitive_type_table[TYPE_PRIMITIVE_COUNT] = {0};
Light_Type* global_type_empty_struct;
Light_Type* global_type_empty_union;
Light_Type* global_type_empty_enum;

static Light_Type*
type_alloc() {
    return arena_alloc(global_type_arena, sizeof(Light_Type));
}

Light_Type*
type_alias_root(Light_Type* type) {
    while(type && type->kind == TYPE_KIND_ALIAS)
        type = type->alias.alias_to;
    return type;
}


// t1 is always the left type in an expression
bool
type_check_equality(Light_Type* t1, Light_Type* t2) {
    if(!(t1->flags & TYPE_FLAG_INTERNALIZED) || !(t2->flags & TYPE_FLAG_INTERNALIZED))
        return false;

    Light_Type* t1root = t1;
    Light_Type* t2root = t2;
    if(t1->kind == TYPE_KIND_ALIAS) {
        t1root = type_alias_root(t1);
    }
    if(t2->kind == TYPE_KIND_ALIAS) {
        t2root = type_alias_root(t2);
    }

    if(!t1root || !t2root) return false;

    if(t1root->kind == TYPE_KIND_PRIMITIVE && t2root->kind == TYPE_KIND_PRIMITIVE) {
        return t1root == t2root;
    }

    if(t1root->kind == TYPE_KIND_POINTER && t2root->kind == TYPE_KIND_POINTER) 
    {
        if(
            t2root->pointer_to == type_primitive_get(TYPE_PRIMITIVE_VOID) ||
            t1root->pointer_to == type_primitive_get(TYPE_PRIMITIVE_VOID))
        {
            return true;
        }
        Light_Type* ptr_to_root1 = type_alias_root(t1root->pointer_to);
        Light_Type* ptr_to_root2 = type_alias_root(t2root->pointer_to);
        if(ptr_to_root1 == ptr_to_root2) {
            if(ptr_to_root1 && ptr_to_root1->kind == TYPE_KIND_PRIMITIVE)
                return true;
        }
    }

    return t1 == t2;
}

bool type_primitive_sint(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive >= TYPE_PRIMITIVE_S8 && t->primitive <= TYPE_PRIMITIVE_S64);
}
bool type_primitive_uint(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive >= TYPE_PRIMITIVE_U8 && t->primitive <= TYPE_PRIMITIVE_U64);
}
bool type_primitive_int(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive >= TYPE_PRIMITIVE_S8 && t->primitive <= TYPE_PRIMITIVE_U64);
}
bool type_primitive_float(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive == TYPE_PRIMITIVE_R32 || t->primitive == TYPE_PRIMITIVE_R64);
}
bool type_primitive_float32(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return (t->kind == TYPE_KIND_PRIMITIVE) && (t->primitive == TYPE_PRIMITIVE_R32);
}
bool type_primitive_float64(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return (t->kind == TYPE_KIND_PRIMITIVE) && (t->primitive == TYPE_PRIMITIVE_R64);
}
bool type_primitive_numeric(Light_Type* t) {
    return (type_primitive_int(t) || type_primitive_float(t));
}
bool type_primitive_bool(Light_Type* t) {
    if(t->kind == TYPE_KIND_ALIAS) {
        t = type_alias_root(t);
    }
    return t == type_primitive_get(TYPE_PRIMITIVE_BOOL);
}

bool type_pointer(Light_Type* t) {
    return type_alias_root(t)->kind == TYPE_KIND_POINTER;
}

Light_Type* type_get_bigger(Light_Type* t1, Light_Type* t2) {
    assert(t1->flags & (TYPE_FLAG_INTERNALIZED|TYPE_FLAG_SIZE_RESOLVED));
    assert(t2->flags & (TYPE_FLAG_INTERNALIZED|TYPE_FLAG_SIZE_RESOLVED));

    return (t1->size_bits > t2->size_bits) ? t1 : t2;
}

s64 type_struct_field_offset_bits(Light_Type* t, const char* field_name) {
    t = type_alias_root(t);
    for(int i = 0; i < t->struct_info.fields_count; ++i) {
        if(t->struct_info.fields[i]->decl_variable.name->data == field_name)
            return t->struct_info.offset_bits[i];
    }
    return -1;
}

static Light_Type*
type_new_primitive(Light_Type_Primitive p) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_PRIMITIVE;
    result->flags = TYPE_FLAG_SIZE_RESOLVED;
    switch(p) {
        case TYPE_PRIMITIVE_VOID:   result->size_bits = 0; break;
        case TYPE_PRIMITIVE_S8:     result->size_bits = 8; break;
        case TYPE_PRIMITIVE_S16:    result->size_bits = 16; break;
        case TYPE_PRIMITIVE_S32:    result->size_bits = 32; break;
        case TYPE_PRIMITIVE_S64:    result->size_bits = 64; break;
        case TYPE_PRIMITIVE_U8:     result->size_bits = 8; break;
        case TYPE_PRIMITIVE_U16:    result->size_bits = 16; break;
        case TYPE_PRIMITIVE_U32:    result->size_bits = 32; break;
        case TYPE_PRIMITIVE_U64:    result->size_bits = 64; break;
        case TYPE_PRIMITIVE_R32:    result->size_bits = 32; break;
        case TYPE_PRIMITIVE_R64:    result->size_bits = 64; break;
        case TYPE_PRIMITIVE_BOOL:   result->size_bits = 32; break;
        default: assert(0); break;
    }
    result->primitive = p;

    return type_internalize(result);
}

uint32_t
type_pointer_size_bits() {
    //return 32;
    return 64;
}

uint32_t
type_pointer_size_bytes() {
    return type_pointer_size_bits() >> 3;
}

Light_Type* 
type_new_pointer(Light_Type* pointer_to) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_POINTER;
    result->flags = TYPE_FLAG_SIZE_RESOLVED;
    result->size_bits = type_pointer_size_bits();
    result->pointer_to = pointer_to;

    if(pointer_to && pointer_to->flags & TYPE_FLAG_INTERNALIZED)
        result = type_internalize(result);

    return result;
}

Light_Type* 
type_new_directive(Light_Ast* expr) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_DIRECTIVE;
    result->flags = 0;
    result->size_bits = 0;
    result->directive = expr;

    return result;
}

Light_Type* 
type_new_alias(Light_Scope* scope, Light_Token* name, Light_Type* alias_to) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_ALIAS;
    result->flags = 0;
    result->size_bits = 0;
    result->alias.name = name;
    result->alias.alias_to = alias_to;
    result->alias.scope = scope;

    s32 index = -1;
    if(type_table_entry_exist(&global_type_table, result, &index, 0)) {
        assert(index >= 0);
        
        Light_Type* found_type = type_table_get(&global_type_table, index);
        if(found_type->flags & TYPE_FLAG_UNRESOLVED) {
            found_type->flags &= ~(TYPE_FLAG_UNRESOLVED);
            assert(alias_to->flags & TYPE_FLAG_INTERNALIZED);
            found_type->alias.alias_to = alias_to;
            found_type->alias.scope = scope;
        }
        result = found_type;
    }

    return result;
}

Light_Type* 
type_new_array(Light_Ast* dimension, Light_Type* type, Light_Token* token_array) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_ARRAY;
    result->size_bits = 0;
    if(!type || (type->flags & TYPE_FLAG_WEAK))
        result->flags |= TYPE_FLAG_WEAK;
    
    result->array_info.array_of = type;
    result->array_info.const_expr = dimension;
    result->array_info.token_array = token_array;

    return result;
}

Light_Type* 
type_new_function(Light_Type** arguments_types, Light_Type* return_type, s32 arguments_count, bool all_arguments_internalized, u32 flags) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_FUNCTION;
    result->size_bits = type_pointer_size_bits();
    result->flags = TYPE_FLAG_SIZE_RESOLVED;

    result->function.arguments_count = arguments_count;
    result->function.arguments_type = arguments_types;
    result->function.return_type = return_type;
    result->function.flags = flags;

    if(return_type->flags & TYPE_FLAG_INTERNALIZED && all_arguments_internalized) {
        result = type_internalize(result);
    }

    return result;
}

Light_Type* 
type_new_enum(Light_Ast** fields, s32 fields_count, Light_Type* type_hint, Light_Scope* scope) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_ENUM;
    result->size_bits = 0;

    result->enumerator.field_count = fields_count;
    result->enumerator.fields = fields;
    result->enumerator.type_hint = type_hint;
    result->enumerator.enum_scope = scope;

    if(type_hint) {
        if(type_hint->flags & TYPE_FLAG_WEAK) {
            result->flags |= TYPE_FLAG_WEAK;
        }
        if(type_hint->flags & TYPE_FLAG_SIZE_RESOLVED) {
            result->flags |= TYPE_FLAG_SIZE_RESOLVED;
        }
    }
    
    return result;
}

Light_Type*
type_new_struct(Light_Ast** fields, s32 fields_count, Light_Scope* struct_scope) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_STRUCT;
    result->size_bits = 0;
    result->flags = 0;

    result->struct_info.fields = fields;
    result->struct_info.fields_count = fields_count;
    result->struct_info.flags = 0;
    result->struct_info.struct_scope = struct_scope;

    return result;
}

Light_Type* 
type_new_union(Light_Ast** fields, s32 fields_count, Light_Scope* union_scope) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_UNION;
    result->size_bits = 0;
    result->flags = 0;

    result->union_info.fields = fields;
    result->union_info.fields_count = fields_count;
    result->union_info.flags = 0;
    result->union_info.union_scope = union_scope;

    return result;
}

static u64 
type_primitive_hash(Light_Type_Primitive p) {
	return primitive_table_hash[p];
}

u64 
type_hash(Light_Type* type) {
	u64 hash = 0;
	if(!type) return 0;
	switch (type->kind) {
		case TYPE_KIND_PRIMITIVE:
			hash = type_primitive_hash(type->primitive); 
            break;
		case TYPE_KIND_POINTER:
			hash = fnv_1_hash_combine(type_hash(type->pointer_to), pointer_hash); 
            break;
		case TYPE_KIND_STRUCT:
            hash = struct_hash;
            if(type->struct_info.fields_count > 0) {
                for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                    Light_Ast* field_node = type->struct_info.fields[i];
                    u64 rhash = type_hash(field_node->decl_variable.type);
                    u64 nhash = fnv_1_hash_from_start(rhash, 
                        field_node->decl_variable.name->data, field_node->decl_variable.name->length);
                    hash = fnv_1_hash_combine(hash, nhash);
                }
            }
            break;
		case TYPE_KIND_UNION:
            hash = union_hash;
            if(type->union_info.fields_count > 0) {
                for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                    Light_Ast* field_node = type->union_info.fields[i];
                    u64 rhash = type_hash(field_node->decl_variable.type);
                    u64 nhash = fnv_1_hash_from_start(rhash, 
                        field_node->decl_variable.name->data, field_node->decl_variable.name->length);
                    hash = fnv_1_hash_combine(hash, nhash);
                    
                }
            }
			break;
		case TYPE_KIND_FUNCTION: {
			u64 return_type_hash = type_hash(type->function.return_type);
            if(type->function.arguments_count > 0) {
                for (size_t i = 0; i < type->function.arguments_count; ++i) {
                    return_type_hash = fnv_1_hash_combine(return_type_hash, type_hash(type->function.arguments_type[i]));
                }
            }
			hash = fnv_1_hash_combine(return_type_hash, proc_hash);
		} break;
		case TYPE_KIND_ARRAY:
			hash = fnv_1_hash_combine(type_hash(type->array_info.array_of), type->array_info.dimension); break;
            break;
        case TYPE_KIND_ALIAS:
            hash = fnv_1_hash(type->alias.name->data, type->alias.name->length);
            hash = fnv_1_hash_combine(hash, (u64)type->alias.scope);
            break;
        case TYPE_KIND_ENUM: 
            hash = enum_hash;
            // TODO(psv): type hint
            //type_hash(type->enumerator.type_hint);
            if(type->enumerator.field_count > 0) {
                for(s32 i = 0; i < type->enumerator.field_count; ++i) {
                    // TODO(psv): field value be part of the hash
                    Light_Token* field_name = type->enumerator.fields[i]->decl_constant.name;
                    hash = fnv_1_hash_from_start(hash, field_name->data, field_name->length);
                }
            }
            break;
		default:
			assert(0);
	}
	return hash;
}

void
type_tables_initialize() {
    // Initialize hashes
    pointer_hash = fnv_1_hash((const u8*)"pointer", sizeof("pointer") - 1);
    struct_hash = fnv_1_hash((const u8*)"struct", sizeof("struct") - 1);
    union_hash = fnv_1_hash((const u8*)"union", sizeof("union") - 1);
    proc_hash = fnv_1_hash((const u8*)"proc", sizeof("proc") - 1);
    enum_hash = fnv_1_hash((const u8*)"enum", sizeof("enum") - 1);

    primitive_table_hash[TYPE_PRIMITIVE_VOID] = fnv_1_hash((const u8*)"void", sizeof("void") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S8]   = fnv_1_hash((const u8*)"s8", sizeof("s8") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S16]  = fnv_1_hash((const u8*)"s16", sizeof("s16") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S32]  = fnv_1_hash((const u8*)"s32", sizeof("s32") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S64]  = fnv_1_hash((const u8*)"s64", sizeof("s64") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U8]   = fnv_1_hash((const u8*)"u8", sizeof("u8") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U16]  = fnv_1_hash((const u8*)"u16", sizeof("u16") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U32]  = fnv_1_hash((const u8*)"u32", sizeof("u32") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U64]  = fnv_1_hash((const u8*)"u64", sizeof("u64") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_R32]  = fnv_1_hash((const u8*)"r32", sizeof("r32") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_R64]  = fnv_1_hash((const u8*)"r64", sizeof("r64") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_BOOL] = fnv_1_hash((const u8*)"bool", sizeof("bool") - 1);

    // Initialize primitive types
    primitive_type_table[TYPE_PRIMITIVE_R32]   = type_new_primitive(TYPE_PRIMITIVE_R32);
    primitive_type_table[TYPE_PRIMITIVE_R64]   = type_new_primitive(TYPE_PRIMITIVE_R64);
    primitive_type_table[TYPE_PRIMITIVE_S8]    = type_new_primitive(TYPE_PRIMITIVE_S8);
    primitive_type_table[TYPE_PRIMITIVE_S16]   = type_new_primitive(TYPE_PRIMITIVE_S16);
    primitive_type_table[TYPE_PRIMITIVE_S32]   = type_new_primitive(TYPE_PRIMITIVE_S32);
    primitive_type_table[TYPE_PRIMITIVE_S64]   = type_new_primitive(TYPE_PRIMITIVE_S64);
    primitive_type_table[TYPE_PRIMITIVE_U8]    = type_new_primitive(TYPE_PRIMITIVE_U8);
    primitive_type_table[TYPE_PRIMITIVE_U16]   = type_new_primitive(TYPE_PRIMITIVE_U16);
    primitive_type_table[TYPE_PRIMITIVE_U32]   = type_new_primitive(TYPE_PRIMITIVE_U32);
    primitive_type_table[TYPE_PRIMITIVE_U64]   = type_new_primitive(TYPE_PRIMITIVE_U64);
    primitive_type_table[TYPE_PRIMITIVE_VOID]  = type_new_primitive(TYPE_PRIMITIVE_VOID);
    primitive_type_table[TYPE_PRIMITIVE_BOOL]  = type_new_primitive(TYPE_PRIMITIVE_BOOL);

    Light_Type* empty_struct_type = type_new_struct(0, 0, 0);
    empty_struct_type->size_bits = 0;
    empty_struct_type->flags = TYPE_FLAG_SIZE_RESOLVED;
    
    Light_Type* empty_union_type = type_new_union(0, 0, 0);
    empty_union_type->size_bits = 0;
    empty_union_type->flags = TYPE_FLAG_SIZE_RESOLVED;

    Light_Type* empty_enum_type = type_new_enum(0, 0, 0, 0);
    empty_enum_type->size_bits = 0;
    empty_enum_type->flags = TYPE_FLAG_SIZE_RESOLVED;

    global_type_empty_struct = type_internalize(empty_struct_type);
    global_type_empty_union = type_internalize(empty_union_type);
    global_type_empty_enum = type_internalize(empty_enum_type);
}

Light_Type*
type_internalize(Light_Type* type) {
    if(type->kind != TYPE_KIND_ALIAS)
        assert(type->flags & TYPE_FLAG_SIZE_RESOLVED);

    s32 index = 0;
    bool added = type_table_add(&global_type_table, type, &index);
    // Works even if the type already exist in the table.
    Light_Type* internalized = type_table_get(&global_type_table, index);
    internalized->flags |= TYPE_FLAG_INTERNALIZED;
    internalized->flags &= ~(TYPE_FLAG_WEAK);

    if(added && !(internalized->flags & TYPE_FLAG_UNRESOLVED)) {
        internalized->flags |= TYPE_FLAG_IN_TYPE_ARRAY;
        array_push(global_type_array, internalized);
    } else if(
        !(internalized->flags & TYPE_FLAG_IN_TYPE_ARRAY) && 
        !(internalized->flags & TYPE_FLAG_UNRESOLVED)) 
    {
        internalized->flags |= TYPE_FLAG_IN_TYPE_ARRAY;
        array_push(global_type_array, internalized);
    }

    return internalized;
}

Light_Type*
type_primitive_get(Light_Type_Primitive t) {
    return primitive_type_table[t];
}

Light_Type* 
type_primitive_from_token(Light_Token_Type token) {
    switch(token) {
        case TOKEN_REAL32:  return primitive_type_table[TYPE_PRIMITIVE_R32];
        case TOKEN_REAL64:  return primitive_type_table[TYPE_PRIMITIVE_R64];
        case TOKEN_SINT8:   return primitive_type_table[TYPE_PRIMITIVE_S8];
        case TOKEN_SINT16:  return primitive_type_table[TYPE_PRIMITIVE_S16];
        case TOKEN_SINT32:  return primitive_type_table[TYPE_PRIMITIVE_S32];
        case TOKEN_SINT64:  return primitive_type_table[TYPE_PRIMITIVE_S64];
        case TOKEN_UINT8:   return primitive_type_table[TYPE_PRIMITIVE_U8];
        case TOKEN_UINT16:  return primitive_type_table[TYPE_PRIMITIVE_U16];
        case TOKEN_UINT32:  return primitive_type_table[TYPE_PRIMITIVE_U32];
        case TOKEN_UINT64:  return primitive_type_table[TYPE_PRIMITIVE_U64];
        case TOKEN_VOID:    return primitive_type_table[TYPE_PRIMITIVE_VOID];
        case TOKEN_BOOL:    return primitive_type_table[TYPE_PRIMITIVE_BOOL];
        default: assert(0); break;
    }
    return 0;
}

Light_Type* 
type_weak_primitive_from_literal(Light_Literal_Type literal) {
    Light_Type* type = type_alloc();
    type->kind = TYPE_KIND_PRIMITIVE;
    type->flags = TYPE_FLAG_WEAK;
    
    switch(literal) {
        case LITERAL_CHAR:          type->primitive = TYPE_PRIMITIVE_U32; break;
        case LITERAL_FLOAT:         type->primitive = TYPE_PRIMITIVE_R32; break;
        case LITERAL_DEC_SINT:      type->primitive = TYPE_PRIMITIVE_S32; break;
        case LITERAL_BIN_INT:       type->primitive = TYPE_PRIMITIVE_U32; break;
        case LITERAL_HEX_INT:       type->primitive = TYPE_PRIMITIVE_U32; break;
        case LITERAL_DEC_UINT:      type->primitive = TYPE_PRIMITIVE_S32; break;
        default: assert(0); break;
    }
    return type;
}

// Could be a WEAK type
Light_Type* 
type_from_token(Light_Token* t) {
    // TODO(psv): implement
    return 0;
}

Light_Type*
type_alias_by_name_str(Light_Scope* decl_scope, const char* name, int length) {
    Light_Type t = {0};
    t.kind = TYPE_KIND_ALIAS;
    const char* str = lexer_internalize_identifier(name, length);
    t.alias.name = token_new_identifier_from_string(str, length);
    t.alias.scope = decl_scope;

    s32 index = -1;
    if(type_table_entry_exist(&global_type_table, &t, &index, 0)) {
        assert(index >= 0);
        return type_table_get(&global_type_table, index);
    }
    return 0;
}

Light_Type* 
type_alias_by_name(Light_Scope* decl_scope, Light_Token* name) {
    Light_Type t = {0};
    t.kind = TYPE_KIND_ALIAS;
    t.alias.name = name;
    t.alias.scope = decl_scope;

    s32 index = -1;
    if(type_table_entry_exist(&global_type_table, &t, &index, 0)) {
        assert(index >= 0);
        return type_table_get(&global_type_table, index);
    }
    return 0;
}

// Creates a struct literal of type User_Type_Info representing the type
Light_Ast* 
type_value_expression(Light_Scope* scope, Light_Type* type) {
    assert(type->flags & TYPE_FLAG_INTERNALIZED);
    
    // User_Type_Info:{u32, u32, u64, User_Type_Description}
    // User_Type_Info:{kind, flags, type_size_bytes, description}

    Light_Ast** user_type_info_struct_exprs = array_new(Light_Ast*);
    Light_Ast* user_type_info_kind = ast_new_expr_literal_primitive_u32(scope, TYPE_KIND_PRIMITIVE, (Lexical_Range){0});
    Light_Ast* user_type_info_flags = ast_new_expr_literal_primitive_u32(scope, 0, (Lexical_Range){0});
    Light_Ast* user_type_info_size_bytes = ast_new_expr_literal_primitive_u64(scope, type->size_bits / 8, (Lexical_Range){0});
    user_type_info_kind->flags |= AST_FLAG_COMPILER_GENERATED;
    user_type_info_flags->flags |= AST_FLAG_COMPILER_GENERATED;
    user_type_info_size_bytes->flags |= AST_FLAG_COMPILER_GENERATED;
    array_push(user_type_info_struct_exprs, user_type_info_kind);
    array_push(user_type_info_struct_exprs, user_type_info_flags);
    array_push(user_type_info_struct_exprs, user_type_info_size_bytes);
    
    //Light_Ast* user_type_info_description 

    int user_type_info_string_length = sizeof("User_Type_Info") - 1;
    const char* user_type_info_string = lexer_internalize_identifier("User_Type_Info", user_type_info_string_length);
    Light_Token* user_type_info_token = token_new_identifier_from_string(user_type_info_string, user_type_info_string_length);
    Light_Ast* type_info_struct_literal = ast_new_expr_literal_struct(
        scope, 
        user_type_info_token,
        user_type_info_token,
        0, false, 0, (Lexical_Range){0});
    type_info_struct_literal->flags |= AST_FLAG_COMPILER_GENERATED;

    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE:{
            // {}
        } break;
        case TYPE_KIND_ALIAS:
        case TYPE_KIND_ARRAY:
        case TYPE_KIND_DIRECTIVE:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
        default: break;
    }
    return 0;
}

s32 type_alignment_get(Light_Type* type){
    type = type_alias_root(type);
	switch(type->kind) {
		case TYPE_KIND_PRIMITIVE: return type->size_bits / 8;
		case TYPE_KIND_POINTER:   return type_pointer_size_bits() / 8;
		case TYPE_KIND_FUNCTION:  return type_pointer_size_bits() / 8;
		case TYPE_KIND_ARRAY:     return type_alignment_get(type->array_info.array_of);
		case TYPE_KIND_STRUCT:{
			if(type->struct_info.fields_count > 0){
				return type_alignment_get(type->struct_info.fields[0]->decl_variable.type);
			} else {
				return 0;
			}
		} break;
		case TYPE_KIND_UNION: {
			// TODO(psv): maybe do this process earlier?
			// calculate union alignment is the alignment of the biggest type
			Light_Type* biggest_type = 0;
			for (size_t i = 0; i < type->union_info.fields_count; ++i) {
				size_t type_size = type->union_info.fields[i]->decl_variable.type->size_bits;
				assert(type_size > 0); // this needs to be calculated first
				if (biggest_type && type->union_info.fields[i]->decl_variable.type->size_bits > biggest_type->size_bits) {
					biggest_type = type->union_info.fields[i]->decl_variable.type;
				} else {
					biggest_type = type->union_info.fields[i]->decl_variable.type;
				}
			}
			return type_alignment_get(biggest_type);
		}break;
        default: 
        assert(0); break;// invalid node 
	}
	return 0;
}

#include <stdio.h>
void
type_table_print() {
    FILE* out = stdout;
    for(int i = 0; i < global_type_table.entries_capacity; ++i) {
        Type_Table_Entry* entry = &global_type_table.entries[i];
        if(entry->flags & HASH_TABLE_OCCUPIED){
            ast_print_type(entry->data, LIGHT_AST_PRINT_STDOUT, 0);
            fprintf(out, "\n");
        }
    }
}