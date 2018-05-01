#include <stdio.h>

extern "C" long long println() {
	return printf("\n");
}

extern "C" long long print_string(long long length, char* data){
	return printf("{ %lld, %s }\n", length, data);
}

extern "C" long long print_s64(long long value) {
	return printf("%lld", value);
}

extern "C" long long print_s32(int value) {
	return printf("%d", value);
}

extern "C" long long print_r64(double value) {
	return printf("%f", value);
}

extern "C" long long print_r32(float value) {
	return printf("%f", value);
}
