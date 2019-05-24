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


int main() {
    printf("int values: %llu\n", (u64)&((Light_VM_EXT_Stack*)0)->int_values);
    printf("int index:  %llu\n", (u64)&((Light_VM_EXT_Stack*)0)->int_index);
    printf("float values: %llu\n", (u64)&((Light_VM_EXT_Stack*)0)->float_values);
    printf("float index:  %llu\n", (u64)&((Light_VM_EXT_Stack*)0)->float_index);

    Light_VM_State* state = light_vm_init();

    void* dll = dlopen("libc.so.6", RTLD_NOW);
    s64(*write_proc)(int, const void *, size_t) = (s64(*)(int, const void *, size_t))dlsym(dll, "write");

    write_proc(1, "foo", 3);
    //printf("%p\n", write_proc);
    
    return 0;

    example4(state);

    //light_vm_debug_dump_code(stdout, state);
    light_vm_execute(state, 0);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
    //light_vm_debug_dump_registers_dec(stdout, state);

    return 0;   
}