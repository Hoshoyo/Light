typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 bool;
typedef float r32;
typedef double r64;
#define true 1
#define false 0


// Forward declarations
u32 ExitProcess(u32 ret);
s64 print_s64(s64 value);
void quicksort(s32* values, s64 start, s64 end);
void print_array(s32* values, s64 length);
s32 __main();

void quicksort(s32* values, s64 start, s64 end){
s64 i = start;
s64 j = (end-1);
s32 pivo = *(s32*)(((char*)values)+ 4 * (((start+end)/2)));
s32 aux = 0;
while((i<=j)){
while(((*(s32*)(((char*)values)+ 4 * (i))<pivo)&&(i<end))){
i = (i+1);
}
loop_1:;

while(((*(s32*)(((char*)values)+ 4 * (j))>pivo)&&(j>start))){
j = (j-1);
}
loop_2:;

if((i<=j)){
aux = *(s32*)(((char*)values)+ 4 * (i));
*(s32*)(((char*)values)+ 4 * (i)) = *(s32*)(((char*)values)+ 4 * (j));
*(s32*)(((char*)values)+ 4 * (j)) = aux;
i = (i+1);
j = (j-1);
}
}
loop_0:;

if((j>start))quicksort(values,start,(j+1));
if((i<end))quicksort(values,i,end);
}

void print_array(s32* values, s64 length){
{
s64 i = 0;
while((i<length)){
print_s64(((s64)*(s32*)(((char*)values)+ 4 * (i))));
i = (i+1);
}
loop_3:;

}
}

s32 __main(){
char arr[40] = {0};
*(s32*)(((char*)arr)+ 4 * (0)) = 5;
*(s32*)(((char*)arr)+ 4 * (1)) = 8;
*(s32*)(((char*)arr)+ 4 * (2)) = 1;
*(s32*)(((char*)arr)+ 4 * (3)) = 2;
*(s32*)(((char*)arr)+ 4 * (4)) = 7;
*(s32*)(((char*)arr)+ 4 * (5)) = 3;
*(s32*)(((char*)arr)+ 4 * (6)) = 6;
*(s32*)(((char*)arr)+ 4 * (7)) = 9;
*(s32*)(((char*)arr)+ 4 * (8)) = 4;
*(s32*)(((char*)arr)+ 4 * (9)) = 10;
print_array(((s32*)arr),10);
quicksort(((s32*)arr),0,10);
print_array(((s32*)arr),10);
return 0;
}


void __entry() {
	ExitProcess(__main());
}