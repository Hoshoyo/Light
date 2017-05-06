#pragma once
#include <ho_system.h>

struct Memory_Arena
{
#define MAX_ARENA 1024
	s64 block_size;
	int current_using;
	int size[MAX_ARENA];
	int allocated[MAX_ARENA];
	void* memory[MAX_ARENA];
	void* free_ptr[MAX_ARENA];

	Memory_Arena(s64 block_size);
	void* allocate(int s);
	void release();
};