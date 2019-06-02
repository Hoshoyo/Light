#pragma once
#include "utils/hash.h"
#include "ast.h"

typedef struct {
    Light_Token* token;
    Light_Ast*   decl;
} Light_Symbol;

GENERATE_HASH_TABLE(Symbol, symbol, Light_Symbol)