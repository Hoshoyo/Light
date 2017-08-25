#pragma once
#include "ho_system.h"

void init_interpreter(s64 stack_size = 1024 * 1024, s64 heap_size = 1024 * 1024);
int run_interpreter();