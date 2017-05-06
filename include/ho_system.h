#if !defined(HO_SYSTEM_H)
#define HO_SYSTEM_H
#include <stdint.h>

typedef int32_t b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float r32;
typedef double r64;

#if defined(_WIN32) || defined(_WIN64)
#include "ho_windows.h"
#endif

struct array;

#define create_array(type, num) (type*)_create_array(num, sizeof(type))
#define push_array(arr, data) _push_array((void**)&arr, data)
#define push_array_no_copy(arr) _push_array_no_copy((void**)&arr)

void* HO_API _create_array(int num_elements, size_t size);
void HO_API _push_array(void** array_, void* data);
size_t HO_API _push_array_no_copy(void** array_);
size_t HO_API get_arr_capacity(void* array_);
size_t HO_API get_arr_length(void* array_);
size_t HO_API get_arr_size_element(void* array_);
void HO_API array_pop(void* array);
void HO_API array_release(void* array);
void HO_API array_remove(void* array_, size_t index);

#if defined(HO_SYSTEM_IMPLEMENT)

struct array
{
	size_t capacity;
	size_t length;
	size_t size_element;
};

void HO_API array_pop(void* array_)
{
	array* base = (array*)((char*)array_ - sizeof(array));
	if(base->length > 0)
		base->length--;
}

void HO_API array_release(void* array_)
{
	array* base = (array*)((char*)array_ - sizeof(array));
	free(base);
}

void HO_API array_remove(void* array_, size_t index)
{
	array* base = (array*)((char*)array_ - sizeof(array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	void* dst = (char*)array_ + index * size_element;
	void* from = (char*)array_ + ((length - 1) * size_element);
	memcpy(dst, from, size_element);
	base->length--;
}

void* HO_API _create_array(int num_elements, size_t size)
{
	void* arr = malloc(sizeof(array) + num_elements * size);
	array* aux = (array*)arr;
	aux->capacity = num_elements;
	aux->length = 0;
	aux->size_element = size;
	return (char*)arr + sizeof(array);
}

void HO_API _push_array(void** array_, void* data)
{
	array* base = (array*)((char*)*array_ - sizeof(array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	if (length == capacity)
	{
		capacity *= 2;
		base->capacity = capacity;

		void* new_mem = realloc(base, capacity * size_element + sizeof(array));
		base = (array*)new_mem;
		*array_ = (char*)new_mem + sizeof(array);
	}

	void* dst = (char*)*array_ + length * size_element;
	base->length++;
	memcpy(dst, data, size_element);
}

size_t HO_API _push_array_no_copy(void** array_)
{
	array* base = (array*)((char*)*array_ - sizeof(array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	if (length == capacity)
	{
		capacity *= 2;
		base->capacity = capacity;
		void* new_mem = realloc(base, capacity * size_element + sizeof(array));
		base = (array*)new_mem;
		*array_ = (char*)new_mem + sizeof(array);
	}

	void* dst = (char*)*array_ + length * size_element;
	base->length++;

	return length;
}

size_t HO_API get_arr_capacity(void* array_)
{
	array* base = (array*)((char*)array_ - sizeof(array));
	size_t capacity = base->capacity;
	return capacity;
}

size_t HO_API get_arr_length(void* array_)
{
	array* base = (array*)((char*)array_ - sizeof(array));
	size_t length = base->length;
	return length;
}

size_t HO_API get_arr_size_element(void* array_)
{
	array* base = (array*)((char*)array_ - sizeof(array));
	size_t size_element = base->size_element;
	return size_element;
}
#undef HO_SYSTEM_IMPLEMENT
#endif	// HO_SYSTEM_IMPLEMENT

#endif	// HO_SYSTEM_H