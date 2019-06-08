#include "type_table.h"
#include "utils/allocator.h"
#include "type.h"
#include <assert.h>

int 
type_equal(Light_Type* t1, Light_Type* t2) {
    if(t1->kind != t2->kind) return 0;
    switch(t1->kind)  {
        case TYPE_KIND_PRIMITIVE:
            return (t1->primitive == t2->primitive);
        case TYPE_KIND_POINTER:
            return type_equal(t1->pointer_to, t2->pointer_to);
        case TYPE_KIND_STRUCT:{
            if(t1->struct_info.fields_count != t2->struct_info.fields_count)
                return 0;
            for(u64 i = 0; i < t1->struct_info.fields_count; ++i) {
                if(!type_equal(
                    t1->struct_info.fields[i]->decl_variable.type,
                    t2->struct_info.fields[i]->decl_variable.type))
                {
                    return 0;
                }
            }
            return 1;
        } break;
        case TYPE_KIND_UNION: {
            if(t1->union_info.fields_count != t2->union_info.fields_count)
                return 0;
            for(u64 i = 0; i < t1->union_info.fields_count; ++i) {
                if(!type_equal(
                    t1->union_info.fields[i]->decl_variable.type,
                    t2->union_info.fields[i]->decl_variable.type))
                {
                    return 0;
                }
            }
            return 1;
        } break;
        case TYPE_KIND_ARRAY:{
            assert(0);
            // TODO(psv):
        } break;
        case TYPE_KIND_FUNCTION:{
            if(!type_equal(t1->function.return_type, t1->function.return_type))
                return 0;
            if(t1->function.arguments_count != t2->function.arguments_count)
                return 0;
            for(s32 i = 0; i < t1->function.arguments_count; ++i) {
                if(!type_equal(t1->function.arguments_type[i], t2->function.arguments_type[i]))
                    return 0;
            }
            return 1;
        } break;
        case TYPE_KIND_ENUM:{
            if(t1->enumerator.type_hint && t2->enumerator.type_hint) {
                return type_equal(t1->enumerator.type_hint, t2->enumerator.type_hint);
            } else {
                if(t1->enumerator.field_count != t2->enumerator.field_count)
                    return 0;
                // TODO(psv): consider enum values
                for(s32 i = 0; i < t1->enumerator.field_count; ++i) {
                    if(t1->enumerator.fields_names[i]->data != t2->enumerator.fields_names[i]->data)
                        return 0;
                }
                return 1;
            }
        } break;
        case TYPE_KIND_ALIAS: {
            return (type_equal(t1->alias.alias_to, t2->alias.alias_to));
        } break;
    }
    return 0;
}

GENERATE_HASH_TABLE_IMPLEMENTATION(Type, type, Light_Type*, type_hash, light_alloc, light_free, type_equal)