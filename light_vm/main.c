#include <stdio.h>
#include "ast.h"
#include "lightvm.h"
#include "light_array.h"
#include <dlfcn.h>

void example1(Light_VM_State* state) {
    // branch test
    light_vm_push(state, "mov r0, 0x5");
    light_vm_push(state, "mov r1, 0x5");
    light_vm_push(state, "cmp r0, r1");
    Light_VM_Instruction_Info b = light_vm_push_fmt(state, "beq 0xff");
    light_vm_push(state, "mov r2, 0x42");
    Light_VM_Instruction_Info hlt = light_vm_push(state, "hlt");
    light_vm_patch_immediate_distance(b, hlt);
}

void example2(Light_VM_State* state) {
    // push and pop test
    light_vm_push(state, "mov r0, 0x5");
    light_vm_push(state, "push r0");
    light_vm_push(state, "mov r0, 0x19");
    light_vm_push(state, "pop r1");
    light_vm_push(state, "hlt");
}

void example3(Light_VM_State* state) {
    Light_VM_Instruction_Info call = light_vm_push(state, "call 0xff");
    light_vm_push(state, "hlt");

    // proc
    Light_VM_Instruction_Info proc = light_vm_push(state, "mov r0, 0x69");
    light_vm_push(state, "ret");

    light_vm_patch_immediate_distance(call, proc);
}

void example4(Light_VM_State* state) {
    // factorial recursive
    light_vm_push(state, "mov r0, 0x5");
    Light_VM_Instruction_Info call = 
        light_vm_push(state, "call 0xff");
    light_vm_push(state, "hlt");

    // factorial
    Light_VM_Instruction_Info start = 
    light_vm_push(state, "cmp r0, 0x1");
    Light_VM_Instruction_Info trivial_branch = 
    light_vm_push(state, "bne 0xff"); // jump over ret
    light_vm_push(state, "ret");
    Light_VM_Instruction_Info over_ret = 
    light_vm_push(state, "push r0");
    light_vm_push(state, "subs r0, 0x1");
    Light_VM_Instruction_Info recursive_call = 
    light_vm_push(state, "call 0xff"); //  call factorial
    light_vm_push(state, "pop r1");
    light_vm_push(state, "muls r0, r1");
    light_vm_push(state, "ret");

    light_vm_patch_immediate_distance(call, start);
    light_vm_patch_immediate_distance(trivial_branch, over_ret);
    light_vm_patch_immediate_distance(recursive_call, start);
}

void example5(Light_VM_State* state) {
    // factorial procedural
    light_vm_push(state, "mov r1, 0x1"); // r1 = 1
    light_vm_push(state, "mov r0, 0x5"); // r0 = 5
    
    Light_VM_Instruction_Info start = 
        light_vm_push(state, "muls r1, r0");  // r1 *= r0
    light_vm_push(state, "subs r0, 0x1"); // r0--

    light_vm_push(state, "cmp r0, 0x1"); // if(r0 == 1)
    Light_VM_Instruction_Info branch = 
        light_vm_push(state, "bne 0xff");    // goto start
    light_vm_push(state, "hlt");

    light_vm_patch_immediate_distance(branch, start);
}

void foozle(int a, float b, int c, int d, int e, int f, int g, int h, int i, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8) {
    printf("%d %f %d %d %d %d %d %d %d %f %f %f %f %f %f %f %f\n", a, b, c, d, e, f, g, h, i, f1, f2, f3, f4, f5, f6, f7, f8);
}

int testextcall() {
    Light_VM_EXT_Stack stack = {0};
    stack.int_arg_count = 8;
    stack.float_arg_count = 9;

    stack.int_values[0] = 1;
    stack.int_values[1] = 2;
    stack.int_values[2] = 3;
    stack.int_values[3] = 4;
    stack.int_values[4] = 5;
    stack.int_values[5] = 6;
    stack.int_values[6] = 7;
    stack.int_values[7] = 8;

    stack.int_index[0] = 0;
    stack.int_index[1] = 2;
    stack.int_index[2] = 3;
    stack.int_index[3] = 4;
    stack.int_index[4] = 5;
    stack.int_index[5] = 6;
    stack.int_index[6] = 7;
    stack.int_index[7] = 8;

    stack.float_values[0] = 0x3f800000;
    stack.float_values[1] = 0x40000000;
    stack.float_values[2] = 0x40400000;
    stack.float_values[3] = 0x40800000;
    stack.float_values[4] = 0x40a00000;
    stack.float_values[5] = 0x40c00000;
    stack.float_values[6] = 0x40e00000;
    stack.float_values[7] = 0x41000000;
    stack.float_values[8] = 0x41100000;

    stack.float_index[0] = 1;
    stack.float_index[1] = 9;
    stack.float_index[2] = 10;
    stack.float_index[3] = 11;
    stack.float_index[4] = 12;
    stack.float_index[5] = 13;
    stack.float_index[6] = 14;
    stack.float_index[7] = 15;
    stack.float_index[8] = 16;

    lvm_ext_call(&stack, foozle);

    return 0;
}

void addproc(int x, int y, float z) {
    printf("%d %d %f\n", x, y, z);
}

void example6(Light_VM_State* state) {
    // factorial procedural
    light_vm_push(state, "mov r1, 0x2"); // r1 = 2
    light_vm_push(state, "mov r0, 0x5"); // r0 = 5
    light_vm_push(state, "mov r3, 0x4131999a");
    light_vm_push(state, "push r3");
    light_vm_push(state, "fmov fr0, [rsp - 0x8]");
    light_vm_push(state, "expushi r1");
    light_vm_push(state, "expushi r0");
    light_vm_push(state, "expushf fr0");
    light_vm_push_fmt(state, "mov r2, %p", (void*)addproc);
    light_vm_push(state, "extcall r2");
    light_vm_push(state, "hlt");
}

int main() {
    Light_VM_State* state = light_vm_init();

    example6(state);

    //light_vm_debug_dump_code(stdout, state);
    light_vm_execute(state, 1);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
    //light_vm_debug_dump_registers_dec(stdout, state);

    return 0;   
}

#if 0    
    void* dll = dlopen("libc.so.6", RTLD_NOW);
    s64(*write_proc)(int, const void *, size_t) = (s64(*)(int, const void *, size_t))dlsym(dll, "write");

    write_proc(1, "foo", 3);
    //printf("%p\n", write_proc);
#endif