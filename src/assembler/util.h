#include <assert.h>
#include "assembler.h"

#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define MIN(A, B) (((A) < (B)) ? (A) : (B))

// mod 2 bits 
// rm  3 bits	= right side = src
// reg 3 bits	= left  side = dest
static u8 
make_modrm(u8 mod, u8 rm, u8 reg) {
	assert(mod < 4);
	assert(rm  < 8);
	assert(reg < 8);
	return (mod << 6) | (rm << 3) | reg;
}

static u8
make_rex(int b, int x, int r, int w) {
	X64_REX res = {0};
	res.high = 4;
	res.B = b;
	res.X = x;
	res.R = r;
	res.W = w;
	return *(u8*)&res;
}

static u8
make_sib(u8 scale, u8 index, u8 base) {
	assert(scale < 4);
	assert(index < 8);
	assert(base  < 8);
	return (scale << 6) | (index << 3) | base;
}

static u8
register_representation(X64_Register r)
{
	if(r < R8) return r;
	if(r < EAX) return (r - R8);
	if(r < R8D) return (r - EAX);
	if(r < AX) return (r - R8D);
	if(r < R8W) return (r - AX);
	if(r < AL) return (r - R8W);
	if(r < R8B) return (r - AL);
	if(r < SPL) return (r - R8B);
	return (r - SPL + 4);
}

static bool
register_is_extended(X64_Register r)
{
	return (r >= R8 && r <= R15) ||
		(r >= R8D && r <= R15D) ||
		(r >= R8W && r <= R15W) ||
		(r >= R8B && r <= R15B);
}

static int
register_get_bitsize(X64_Register r)
{
    if(r >= RAX && r <= R15) return 64;
    if(r >= EAX && r <= R15D) return 32;
    if(r >= AX && r <= R15W) return 16;
    return 8;
}

static u8*
emit_int_value(u8* stream, int bitsize, Int_Value value)
{
	switch(bitsize)
    {
        case 8:  *(uint8_t*)stream = value.v8; break;
        case 16: *(uint16_t*)stream = value.v16; break;
        case 32: *(uint32_t*)stream = value.v32; break;
        case 64: *(uint64_t*)stream = value.v64; break;
    }
    stream += (bitsize / 8);
	return stream;
}

static u8*
emit_opcode(u8* stream, u8 opcode, int bitsize, X64_Register dest, X64_Register src)
{
    bool using_extended_register = register_is_extended(dest) || register_is_extended(src);

	if(bitsize == 16)
		*stream++ = 0x66; // operand size override
    if(bitsize == 64 || using_extended_register)
		*stream++ = make_rex(register_is_extended(dest), 0, register_is_extended(src), bitsize == 64);
    else if(bitsize == 8)
    {
        if( dest == SPL || dest == BPL || dest == SIL || dest == DIL ||
            src == SPL || src == BPL || src == SIL || src == DIL)
        {
            *stream++ = make_rex(0,0,0,0);
        }
    }
    *stream++ = opcode;
    return stream;
}