# HoAssembler

## What it is and what it's for

This a simple single header library containing functions that generate x86_64 assembly instructions.
It is not a complete set, but it contains the most important instructions, including the core set and
the most used SSE instructions. It is also not a complete assembler, rather an auxiliary library to make
one, or to generate code for a compiler backend.

## How to build

- Include in your c or cpp unit with the following definition in order to build with the implementation:

```c
#define HO_ASSEMBLER_IMPLEMENTATION
#include "hoasm.h"

// Use it
```