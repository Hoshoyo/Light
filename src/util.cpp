#include "util.h"
#include "string.h"
#include <stdarg.h>
#include <stdlib.h>

string string_new(const char* v, s64 length){
	string result = {0};
	if(length <= 0) return result;

	result.data     = (u8*)calloc(1, length * 2);
	result.length   = length;
	result.capacity = length * 2;
	memcpy((void*)result.data, v, length);
	return result;
}

void string_free(string* s) {
	free((void*)s->data);
	s->data = 0;
	s->length = 0;
	s->capacity = -1;
}

void string_append(string* s1, const char* s2){
	s64 s2_length  = str_length(s2);
	s64 res_length = s2_length + s1->length;

	if(res_length > s1->capacity){
		s1->capacity *= 2;
		s1->data = (u8*)realloc((void*)s1->data, s1->capacity);
	}
	memcpy((char*)s1->data + s1->length, s2, s2_length);
	s1->length    = res_length;
}

string string_make(const char* v) {
	string result;
	s64 len = str_length(v);
	result.data   = (const u8*)v;
	result.length = len;
	result.capacity = -1;
	return result;
}

string string_make(const char* v, s64 length) {
	string result;
	result.data = (const u8*)v;
	result.length = length;
	result.capacity = -1;
	return result;
}

s64 str_length(const char* str) {
	s64 length = 0;
	while (*str++) ++length;
	return length;
}

bool c_str_equal(const char* str1, const char* str2)
{
	const char* ptr = str1;
	const char* ptr2 = str2;
	while (*ptr) {
		if (*ptr++ != *ptr2++)
			return false;
	}

	if (*ptr2 == 0)
		return true;
	else
		return false;
}

bool str_equal(const char* str1, int str1_size, const char* str2, int str2_size)
{
	if (str1_size == str2_size) {
		for (int i = 0; i < str1_size; ++i)	{
			if (str1[i] != str2[i])
				return false;
		}
		return true;
	}
	return false;
}

bool str_equal(const string& s1, const string& s2)
{
	if (s1.length == s1.length) {
		for (int i = 0; i < s1.length; ++i) {
			if (s1.data[i] != s2.data[i])
				return false;
		}
		return true;
	}
	return false;
}

s64 align_delta(s64 offset, s64 align_to){
	s64 rest = offset % align_to;
	return((align_to - rest) % align_to);
}

//https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
s32 next_power2(s32 v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

bool is_white_space(char str)
{
	return (str == ' ' ||
		str == '\t' ||
		str == '\n' ||
		str == '\v' ||
		str == '\f' ||
		str == '\r');
}

bool is_hex_digit(char c) {
	return (is_number(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

bool is_number(char c) {
	return (c >= '0' && c <= '9');
}

bool is_letter(char c) {
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

u64 str_to_u64(char* text, int length)
{
	u64 result = 0;
	u64 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result;
}

s64 str_to_s64(char* text, int length)
{
	s64 result = 0;
	s64 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result;
}

s32 str_to_s32(char* text, int length)
{
	s32 result = 0;
	s32 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result;
}

r64 str_to_r64(char* text, int length)
{
	r64 result = 0.0;
	r64 tenths = 1.0;
	r64 frac_tenths = 0.1;
	int point_index = 0;

	while (text[point_index] != '.')
		++point_index;

	for (int i = point_index - 1; i >= 0; --i, tenths *= 10.0)
		result += (text[i] - 0x30) * tenths;
	for (int i = point_index + 1; i < length; ++i, frac_tenths *= 0.1)
		result += (text[i] - 0x30) * frac_tenths;
	return result;
}

r32 str_to_r32(char* text, int length)
{
	return (r32)str_to_r64(text, length);
}

u8 str_to_u8(char* text, int length)
{
	if (length == 2) {
		switch (text[1]) {
		case 'a': return 0x07;	// alert
		case 'b': return 0x08;	// backspace
		case 'f': return 0x0c;	// formfeed
		case 'n': return 0x0a;	// new line
		case 'r': return 0x0d;	// carriage return
		case 't': return 0x09;	// horizontal tab
		case 'v': return 0x0b;	// vertical tab
		case '\\': return '\\';
		case '\'': return '\'';
		case '"': return '"';
		}
	}
	else if (length == 1) {
		return text[0];
	}
	else if (length == 3 || length == 4) {
		// hex escaped
		if (text[0] != '\\') return 0;
		if (text[1] != 'x')	return 0;

		if (length == 3) {
			if (is_number(text[2])) return text[2] - 0x30;
		} else {
			if (is_number(text[2]) && is_number(text[3])) {
				int num = (text[2] - 0x30) * 10 + (text[3] - 0x30);
				if (num >= 0 && num <= 0xff) return (char)num;
			}
		}
	}
	return 0;
}

int s32_to_str(s32 val, char* buffer) {
	char b[16] = {0};
	int sum = 0;
	if(val < 0) {
		val = -val;
		buffer[0] = '-';
		sum = 1;
	}
    char* auxbuffer = b + 16;
    char* start = auxbuffer;
    do {
        int rem = val % 10;
        val = val / 10;
        *auxbuffer-- = '0' + rem;
    } while(val != 0);

    int size = start - auxbuffer;
	memcpy(buffer + sum, auxbuffer + 1, size);
	return size;
}

int s64_to_str(s64 val, char* buffer) {
    char b[32] = {0};
	int sum = 0;
	if(val < 0) {
		val = -val;
		buffer[0] = '-';
		sum = 1;
	}
    char* auxbuffer = b + 32;
    char* start = auxbuffer;
    do {
        int rem = val % 10;
        val = val / 10;
        *auxbuffer-- = '0' + rem;
    } while(val != 0);

    int size = start - auxbuffer;
	memcpy(buffer + sum, auxbuffer + 1, size);
	return size;
}

u32 djb2_hash(u8 *str, int size)
{
	u32 hash = 5381;
	int c;
	int i = 0;
	while (i < size) {
		c = *str++;
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		++i;
	}
	return hash;
}

u32 djb2_hash(u32 starting_hash, u8 *str, int size)
{
	u32 hash = starting_hash;
	int c;
	int i = 0;
	while (i < size) {
		c = *str++;
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		++i;
	}
	return hash;
}

u32 djb2_hash(u32 hash1, u32 hash2) {
	u32 hash = ((hash1 << 5) + hash1) + hash2;
	return hash;
}

char* make_c_string(string& s)
{
	if (s.length == 0) return 0;
	char* mem = (char*)malloc(s.length + 1);
	mem[s.length] = 0;
	memcpy(mem, s.data, s.length);
	return mem;
}

u64 fnv_1_hash_combine(u64 hash1, u64 hash2) {
	u64 fnv_prime = 1099511628211;
	u64 hash = hash1;
	hash = hash * fnv_prime;
	hash = hash ^ hash2;
	return hash;
}

u64 fnv_1_hash_from_start(u64 hash, const u8* s, u64 length) {
	u64 fnv_prime = 1099511628211;
	for (u64 i = 0; i < length; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}

u64 fnv_1_hash(const u8* s, u64 length) {
	u64 hash = 14695981039346656037;
	u64 fnv_prime = 1099511628211;
	for (u64 i = 0; i < length; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}

s32 system_exit(s32 ret) {
	exit(ret);
}

void report_internal_compiler_error(char* filename, int line, char* msg, ...) {
	va_list args;
	va_start(args, msg);
	fprintf(stderr, "Internal compiler Error: ", filename, line);
	vfprintf(stderr, msg, args);
	va_end(args);
	system_exit(-1);
}
