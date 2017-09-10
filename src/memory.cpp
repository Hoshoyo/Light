#include "memory.h"
#include <malloc.h>

#ifdef __linux__
void *_aligned_malloc(size_t size, size_t alignment)
{
	return aligned_alloc(alignment, size);
}
void _aligned_free(void* block)
{
	free(block);
}

#endif

Memory_Arena::Memory_Arena(s64 block_size)
{
	current_using = 0;
	memset(size, 0, sizeof(int) * MAX_ARENA);
	size[0] = block_size;
	memory[0] = _aligned_malloc(size[0], 16);							// SIZE THEN ALIGNMENT THANK YOU LINUX
	allocated[0] = 0;
	free_ptr[0] = memory[0];
}

void* Memory_Arena::allocate(int s)
{
	void* result = free_ptr[current_using];
	if (((char*)memory[current_using] + size[current_using]) < ((char*)free_ptr[current_using] + s))
	{
		current_using++;
		if (current_using == MAX_ARENA)
			return 0;	// error, full arena
		size[current_using] = block_size;
		memory[current_using] = _aligned_malloc(block_size, 16);		// SIZE THEN ALIGNMENT THANK YOU LINUX
		allocated[current_using] = 0;
		free_ptr[current_using] = memory[current_using];
	}
	free_ptr[current_using] = (char*)free_ptr[current_using] + s;
	allocated[current_using] += s;
	return result;
}

void Memory_Arena::release()
{

	for (int i = 0; i < MAX_ARENA; ++i)
	{
		if (size[i] != 0) {
			size[i] = 0;
			allocated[i] = 0;
			_aligned_free(memory[i]);
			memory[i] = 0;
			free_ptr[i] = 0;
		}
		else
		{
			break;
		}
	}
	current_using = 0;
}