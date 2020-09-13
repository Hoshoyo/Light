#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

static u8 
make_modrm2(u8 mod, u8 rm, u8 reg) {
	return (mod << 6) | (rm << 3) | reg;
}

int main(int argc, char** argv)
{
	FILE* out = fopen("test.bin", "wb");
	u8* code = (u8*)calloc(1, 1024*1024);
	u8* stream = code;
	
	{
		//stream = emit_mov_test(stream);
		stream = emit_add_test(stream);
		stream = emit_and_test(stream);
	}

	fwrite(code, 1, stream - code, out);
	fclose(out);

	system("ndisasm -b 64 test.bin");
    return 0;
}