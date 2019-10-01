typedef struct {
	struct User_Type_Info_t* array_of;
    u64 dimension;
} User_Type_Array;

typedef struct {
	struct User_Type_Info_t** fields_types;
	string* fields_names;
	s32 fields_count;
	s32 alignment;
} User_Type_Union;

typedef struct {
	struct User_Type_Info_t** fields_types;
	string* fields_names;
	s64* fields_offsets_bits;
	s32 fields_count;
	s32 alignment;
} User_Type_Struct;

typedef struct {
	struct User_Type_Info_t* return_type;
	struct User_Type_Info_t** arguments_type;
	string* arguments_name;
	s32 arguments_count;
} User_Type_Function;

typedef struct {
    string name;
    struct User_Type_Info_t*  alias_to;
} User_Type_Alias;

typedef union {
    u32 primitive; // User_Type_Primitive
    struct User_Type_Info_t* pointer_to;
    User_Type_Array array_desc;
    User_Type_Struct struct_desc;
    User_Type_Union union_desc;
    User_Type_Function function_desc;
    User_Type_Alias alias_desc;
} User_Type_Desc;

typedef struct User_Type_Info_t {
    u32 kind; // User_Type_Kind
	u32 flags;
	s64 type_size_bytes;
    User_Type_Desc description;
} User_Type_Info;