#include <stdio.h>

extern "C" long long println() {
	int r = printf("\n");
	fflush(stdout);
	return r;
}

extern "C" long long print_string(long long length, char* data){
	int r = printf("%.*s", (int)length, data);
	fflush(stdout);
	return r;
}

extern "C" long long print_s64(long long value) {
	int r = printf("%lld", value);
	fflush(stdout);
	return r;
}

extern "C" long long print_s32(int value) {
	int r = printf("%d", value);
	fflush(stdout);
	return r;
}

extern "C" long long print_r64(double value) {
	int r = printf("%f", value);
	fflush(stdout);
	return r;
}

extern "C" long long print_r32(float value) {
	int r = printf("%f", value);
	fflush(stdout);
	return r;
}
