#include "utils.h"

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

s64 str_to_s64(char* text, int length)
{
    bool negative = false;
    if(*text == '-') {
        text++;
        negative = true;
    }
	s64 result = 0;
	s64 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result * ((negative) ? -1 : 1);
}

u64 str_to_u64(char* text, int length)
{
	u64 result = 0;
	u64 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result;
}

static int
is_number(char c) {
    return (c >= '0' && c <= '9');
}

u8 str_to_u8(char* text, int length) {
	if (length == 2) {
		switch (text[1]) {
			case 'a': return 0x07;  // alert
			case 'b': return 0x08;  // backspace
			case 'f': return 0x0c;  // formfeed
			case 'n': return 0x0a;  // new line
			case 'r': return 0x0d;  // carriage return
			case 't': return 0x09;  // horizontal tab
			case 'v': return 0x0b;  // vertical tab
			case '\\': return '\\';
			case '\'': return '\'';
			case '"': return '"';
		}
	} else if (length == 1) {
		return text[0];
	} else if (length == 3 || length == 4) {
		// hex escaped
		if (text[0] != '\\') return 0;
		if (text[1] != 'x')     return 0;

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

static u8 
hexdigit_to_u8(u8 d) {
    if (d >= 'A' && d <= 'F')
        return d - 'A' + 10;
    if (d >= 'a' && d <= 'f')
        return d - 'a' + 10;
    return d - '0';
}

u64 
str_hex_to_u64(char* text, int length) {
    u64 res = 0;
    u64 count = 0;
    for (s64 i = length - 1; i >= 0; --i, ++count) {
		if (text[i] == 'x') break;
		char c = hexdigit_to_u8(text[i]);
		res += (u64)c << (count * 4);
    }
    return res;
}

#if 0
catstring 
path_from_fullpath(catstring s) {
	s64 index = 0;
    for (s64 i = s.length - 1; i >= 0; --i){
		if (s.data[i] == '/'
#if defined (_WIN32) || defined(_WIN64)
			|| s.data[i] == '\\'
#endif
			){
            index = i + 1;
			break;
        }
    }
    catstring result = catstring_copy(&s);
	result.length = index;

    return result;
}

catstring 
filename_from_path(catstring s) {
    s64 index = 0;
    for (s64 i = s.length - 1; i >= 0; --i){
        if(s.data[i] == '/'){
            index = i + 1;
			break;
        }
    }
    catstring result = catstring_new(s.data, s.length - index);
    return result;
}

bool
file_exists(const char* filename) {
	int r = access(filename, F_OK);
	return r == 0;
}

static char* 
file_realpath(const char* path, size_t* size)
{
	// this function uses malloc, should free after
	char* result = realpath(path, 0);
	if(result)
		*size = strlen(result);
	else
		*size = 0;
	return result;
}

catstring
fullpath_from_filename(const char* filename) {
	size_t size = 0;
	char* fullpath = file_realpath(filename, &size);

	catstring result = {0};
	result.capacity = (int)size;
	result.data = fullpath;
	result.length = (int)size;

	return result;
}
#endif