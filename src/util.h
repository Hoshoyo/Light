#pragma once
#include <ho_system.h>

#define FLAG(A) (1 << A)
#define MAX(X, Y) ((X > Y) ? (X) : (Y))
#define MIN(X, Y) ((X < Y) ? (X) : (Y))

#define MAKE_STRING(X) { sizeof(X) - 1, (u8*)(X) }

#define ARRAY_COUNT(X) (sizeof(X) / sizeof((X)[0]))


struct string {
	s64 length;
	const u8* data;
	s64 capacity;
};

string string_new(const char* v, s64 length);
void   string_free(string* s);
void   string_append(string* s1, const char* s2);

string string_make(const char* v);
string string_make(const char* v, s64 length);

bool is_white_space(char str);
bool is_number(char c);
bool is_letter(char c);
bool is_hex_digit(char c);

s64  str_length(const char* str);
bool c_str_equal(const char* s1, const char* s2);
bool str_equal(const char* s1, int s1_len, const char* s2, int s2_len);
bool str_equal(const string& s1, const string& s2);
u64  str_to_u64(char* text, int length);
s64  str_to_s64(char* text, int length);
s32  str_to_s32(char* text, int length);
r64  str_to_r64(char* text, int length);
r32  str_to_r32(char* text, int length);
u8   str_to_u8(char* text, int length);

int s32_to_str(s32 val, char* buffer);
int s64_to_str(s64 val, char* buffer);

u32 djb2_hash(u8 *str, int size);
u32 djb2_hash(u32 starting_hash, u8 *str, int size);
u32 djb2_hash(u32 hash1, u32 hash2);
u64 fnv_1_hash(const u8* s, u64 length);
u64 fnv_1_hash_combine(u64 hash1, u64 hash2);
u64 fnv_1_hash_from_start(u64 hash, const u8* s, u64 length);

s32 system_exit(s32 ret);

void report_internal_compiler_error(char* filename, int line, char* msg, ...);