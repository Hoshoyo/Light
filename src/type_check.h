#pragma once
#include "type.h"
#include "error.h"

Type_Error type_check(Scope* scope, Ast** ast);