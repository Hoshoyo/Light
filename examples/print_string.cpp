#include <stdio.h>

extern "C" void print_string(long long length, char* data){
	printf("{ %lld, %s }\n", length, data);
}

typedef struct {
	long long length;
	char* data;
} string;

extern "C" long long print_s64(long long value) {
	return printf("%lld\n", value);
}

extern "C" void print_double(double value) {
	printf("%f\n", value);
}

extern "C" long long print_r32(float value) {
	return printf("%f\n", value);
}
