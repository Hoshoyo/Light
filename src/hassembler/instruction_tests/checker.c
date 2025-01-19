#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_entire_file(const char* filename, size_t* size)
{
    FILE* file = fopen(filename, "rb");

    if(!file) {
        fprintf(stderr, "Could not load file %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t length_bytes = ftell(file);
	if(size) *size = length_bytes;
    fseek(file, 0, SEEK_SET);
    
    char* stream = malloc(length_bytes + 1);

    fread(stream, length_bytes, 1, file);
    fclose(file);

	return stream;
}

int
check_file(const char* exe, const char* result, const char* test)
{
    char buffer[1024] = {0};

    size_t arith_result_size = 0;
#if defined(_WIN32) || defined(_WIN64)
    sprintf(buffer, ".\\bin\\%s", result);
    char* res_data = read_entire_file(buffer, &arith_result_size);

    sprintf(buffer, "bin\\%s > bin\\%s", exe, test);

    system(buffer);
    sprintf(buffer, "objdump -D -Mintel,x86-64 -b binary -m i386 -w %s", test);

    size_t arith_test_size = 0;
    sprintf(buffer, ".\\bin\\%s", test);
    char* test_data = read_entire_file(buffer, &arith_test_size);
#else
    sprintf(buffer, "./bin/%s", result);
    char* res_data = read_entire_file(buffer, &arith_result_size);

    sprintf(buffer, "bin/%s > bin/%s", exe, test);

    system(buffer);
    sprintf(buffer, "objdump -D -Mintel,x86-64 -b binary -m i386 -w %s", test);

    size_t arith_test_size = 0;
    sprintf(buffer, "./bin/%s", test);
    char* test_data = read_entire_file(buffer, &arith_test_size);
#endif

    if(arith_test_size != arith_result_size)
    {
        fprintf(stdout, "Test failed: %s Different file sizes(Result x Test): %llu vs %llu\n", test, arith_result_size, arith_test_size);
        free(res_data);
        free(test_data);
        return 1;
    }
    if(memcmp(res_data, test_data, arith_result_size) != 0)
    {
        fprintf(stdout, "Test failed: Different file content\n");
        free(res_data);
        free(test_data);
        return 1;
    }

    free(res_data);
    free(test_data);

    fprintf(stdout, "Test Succeeded (%s)\n", test);

    return 0;
}

int main()
{
#if defined(_WIN32) || defined(_WIN64)
    check_file("test_arith.exe", "arith_result.res", "arith.test");
    check_file("test_mov.exe", "mov_result.res", "mov.test");
    check_file("test_mul.exe", "mul_result.res", "mul.test");
    check_file("test_jmp.exe", "jmp_result.res", "jmp.test");
    check_file("test_shift.exe", "shift_result.res", "shift.test");
    check_file("test_bit.exe", "bit_result.res", "bit.test");
    check_file("test_sse.exe", "sse_result.res", "sse.test");
    check_file("test_cvt.exe", "cvt_result.res", "cvt.test");
    check_file("test_str.exe", "str_result.res", "str.test");
#else
    check_file("test_arith", "arith_result.res", "arith.test");
    check_file("test_mov", "mov_result.res", "mov.test");
    check_file("test_mul", "mul_result.res", "mul.test");
    check_file("test_jmp", "jmp_result.res", "jmp.test");
    check_file("test_shift", "shift_result.res", "shift.test");
    check_file("test_bit", "bit_result.res", "bit.test");
    check_file("test_sse", "sse_result.res", "sse.test");
    check_file("test_cvt", "cvt_result.res", "cvt.test");
    check_file("test_str", "str_result.res", "str.test");
#endif

    return 0;
}