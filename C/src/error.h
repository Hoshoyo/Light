#include "ast.h"

s32 type_error_location(Light_Token* t);
s32 type_error(u32* error, Light_Token* location, const char* fmt, ...);
s32 type_error_undeclared_identifier(u32* error, Light_Token* t);
s32 type_error_mismatch(u32* error, Light_Token* location, Light_Type* left, Light_Type* right);