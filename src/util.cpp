#include "util.h"

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

bool is_white_space(char str)
{
	if (str == ' ' ||
		str == '\t' ||
		str == '\n' ||
		str == '\v' ||
		str == '\f' ||
		str == '\r')
	{
		return true;
	}
	return false;
}

bool is_number(char c)
{
	if (c >= '0' && c <= '9')
		return true;
	return false;
}

bool is_letter(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return true;
	return false;
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

void s32_to_str(s32 val, char* buffer)
{
	s32 aux = val;
	s32 divisor = 10;
	while (aux) {
		s32 v = aux % divisor;
		*buffer++ = v + 0x30;
		aux -= v;
		divisor *= 10;
	}
}

void s64_to_str(s64 val, char* buffer)
{
	s64 aux = val;
	s64 divisor = 10;
	while (aux) {
		s64 v = aux % divisor;
		*buffer++ = v + 0x30;
		aux -= v;
		divisor *= 10;
	}
}

string::string()
{
	capacity = 0;
	length = 0;
	data = 0;
}

string::string(s64 capac, size_t strlen, char* str)
{
	capacity = capac;
	length = strlen;
	data = (char*)malloc(capacity);
	if (strlen) strncpy(data, str, strlen);
}

string::~string()
{
	capacity = 0;
	length = 0;
	if(capacity > 0) free(data);
	data = 0;
}

void string::cat(string& r)
{
	if (length + r.length >= capacity)
	{
		size_t newcap = capacity * 2 + r.length;
		capacity = newcap;
		data = (char*)realloc(data, newcap);
	}
	memcpy(&data[length], r.data, r.length);
	length += r.length;
}

void string::cat(char* str, size_t len)
{
	if (length + len >= capacity)
	{
		size_t newcap = capacity * 2 + len;
		capacity = newcap;
		data = (char*)realloc(data, newcap);
	}
	memcpy(&data[length], str, len);
	length += len;
}

void string::cat(s64 num)
{
	char buffer[128];
	s64 m = 10;
	int i = 0;
	for (; num; ++i)
	{
		s64 res = num % m;
		s64 nres = res / (m / 10);
		buffer[64 - i] = nres + 0x30;
		num -= res;
		m *= 10;
	}
	int len = 64 - i;

	if (length + len >= capacity)
	{
		size_t newcap = capacity * 2 + len;
		capacity = newcap;
		data = (char*)realloc(data, newcap);
	}
	memcpy(&data[length], buffer, len);
	length += len;
}

bool string::is_mutable()
{
	if (capacity == -1) return false;
	return true;
}

string make_new_string(s64 capacity)
{
	string res;
	res.capacity = capacity;
	res.length = 0;
	res.data = (char*)malloc(res.capacity);
	return res;
}

string make_new_string(const char* val)
{
	string res;
	size_t len = strlen(val);
	res.capacity = len + 1;
	res.length = len;
	if (len) {
		res.data = (char*)malloc(res.capacity);
		strncpy(res.data, val, len);
	}
	return res;
}

void make_immutable_string(string& s, const char* val, s64 length)
{
	s.capacity = -1;
	// yes i dont give a damn I say when it is or it is'nt const
	s.data = (char*)val;
	s.length = length;
}

void make_immutable_string(string& s, const char* val)
{
	s.capacity = -1;
	// yes i dont give a damn I say when it is or it is'nt const
	s.data = (char*)val;
	s.length = strlen(val);
}

void make_immutable_string(string& dest, string& src)
{
	dest.capacity = -1;
	dest.data = src.data;
	dest.length = src.length;
}