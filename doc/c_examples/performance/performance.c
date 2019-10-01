#include <stdio.h>
#include <stdlib.h>

char* read_entire_file (char* filename, int* out_file_size) {
    FILE* fd = fopen(filename, "r");

    fseek(fd, 0, SEEK_END);
    int size = ftell(fd);
    if(out_file_size) *out_file_size = size;
    fseek(fd, 0, SEEK_SET);

    char* memory = (char*)malloc(size + 1);
    memory[size] = 0;

    fread(memory, size, 1, fd);

    fclose(fd);

    return memory;
}

unsigned long hash_func(char* s, unsigned long secret) {
	unsigned long hash = secret;
	unsigned long fnv_prime = 1099511628211;
	for (unsigned long i = 0; s[i] != 0; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}

int main(int argc, char** argv) {
    char* data = read_entire_file("secret.txt", 0);
    unsigned long secret;
    sscanf(data, "%lu", &secret);

    FILE* out = fopen("table_hash.h", "w");

    fprintf(out, "unsigned long table[] = {\n");
    fprintf(out, "%lu, \n", hash_func("foo", secret));
    fprintf(out, "%lu, \n", hash_func("bar", secret));
    fprintf(out, "%lu, \n", hash_func("fizzle", secret));
    fprintf(out, "%lu, \n", hash_func("barlze", secret));
    fprintf(out, "%lu, \n", hash_func("hello", secret));
    fprintf(out, "%lu, \n", hash_func("int", secret));
    fprintf(out, "%lu, \n", hash_func("u32", secret));
    fprintf(out, "%lu, \n", hash_func("string", secret));
    fprintf(out, "%lu \n", hash_func("value", secret));
    fprintf(out, "};\n");

    fclose(out);
    return 0;
}