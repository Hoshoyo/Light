#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "lightvm.h"
#include <stdint.h>
#include <assert.h>

#if defined(__linux__)
#include <dlfcn.h>
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

void example1(Light_VM_State* state) {
    // branch test
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r0, 0x5");
    light_vm_push(state, "mov r1, 0x5");
    light_vm_push(state, "cmp r0, r1");
    Light_VM_Instruction_Info b = light_vm_push_fmt(state, "beq 0xff");
    light_vm_push(state, "mov r2, 0x42");
    Light_VM_Instruction_Info hlt = light_vm_push(state, "hlt");
    light_vm_patch_immediate_distance(b, hlt);

    light_vm_execute(state, entry.absolute_address, 0);
    assert(state->registers[R0] == 5 && state->registers[R1] == 5);
}

void example2(Light_VM_State* state) {
    // push and pop test
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r0, 0x5");
    light_vm_push(state, "push r0");
    light_vm_push(state, "mov r0, 0x19");
    light_vm_push(state, "pop r1");
    light_vm_push(state, "hlt");

    light_vm_execute(state, entry.absolute_address, 0);
    assert(state->registers[R0] == 25 && state->registers[R1] == 5);
}

void example3(Light_VM_State* state) {
    Light_VM_Instruction_Info call = light_vm_push(state, "call 0xff");
    light_vm_push(state, "hlt");

    // proc
    Light_VM_Instruction_Info proc = light_vm_push(state, "mov r0, 0x69");
    light_vm_push(state, "ret");

    light_vm_patch_immediate_distance(call, proc);

    light_vm_execute(state, call.absolute_address, 0);
    assert(state->registers[R0] == 0x69);
}

void example4(Light_VM_State* state) {
    // factorial recursive
    Light_VM_Instruction_Info entry = 
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

    light_vm_debug_dump_code(stdout, state);

    light_vm_execute(state, entry.absolute_address, 0);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
    assert(state->registers[R0] == 120 && state->registers[R1] == 5);

}

void example5(Light_VM_State* state) {
    // factorial procedural
    Light_VM_Instruction_Info entry = 
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

    light_vm_execute(state, entry.absolute_address, 0);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
    assert(state->registers[R1] == 120);
}

r32 addproc(int x, int y, float z) {
    printf("%d %d %f\n", x, y, z);
    return 1.544f;
}

void example6(Light_VM_State* state) {
    // external call test
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r1, 0x2"); // r1 = 2
    light_vm_push(state, "mov r0, 0x5"); // r0 = 5
    light_vm_push(state, "mov r3, 0x4131999a");
    light_vm_push(state, "push r3");
    light_vm_push(state, "fmov fr0, [rsp - 0x8]");
    light_vm_push(state, "expushi r1");
    light_vm_push(state, "expushi r0");
    light_vm_push(state, "expushf fr0");
    light_vm_push_fmt(state, "mov r2, 0x%llx", (void*)addproc);
    light_vm_push(state, "extcall r2");
    light_vm_push(state, "expop");
    light_vm_push(state, "hlt");

    // Should print 2 5 11.100000
	light_vm_debug_dump_code(stdout, state);
    light_vm_execute(state, entry.absolute_address, 1);
    assert(state->f32registers[FR0] == 1.544f);
}

#if defined(__linux__)
void example7(Light_VM_State* state) {
    char str[] = "Hello World!\n";
    void* addr = light_vm_push_bytes_data_segment(state, (u8*)str, sizeof(str) - 1);
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r0, 1");
    light_vm_push_fmt(state, "mov r1, %p", addr);
    //light_vm_push(state, "mov r1, rdp");
    light_vm_push_fmt(state, "mov r2, %d", sizeof(str) - 1);
    light_vm_push(state, "expushi r0");
    light_vm_push(state, "expushi r1");
    light_vm_push(state, "expushi r2");
    light_vm_push_fmt(state, "mov r3, %p", write);
    light_vm_push(state, "extcall r3");
    light_vm_push(state, "expop");
    light_vm_push(state, "hlt");

    // Should print Hello World!\n
    light_vm_execute(state, entry.absolute_address, 0);
}
#elif defined(_WIN32) || defined(_WIN64)
void example7(Light_VM_State* state) {
    char str[] = "Hello World!\n";
    void* addr = light_vm_push_bytes_data_segment(state, (u8*)str, sizeof(str) - 1);
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r0, 0");
    light_vm_push_fmt(state, "mov r1, 0x%llx", addr);
    light_vm_push_fmt(state, "mov r2, 0x%llx", addr);
    light_vm_push(state, "mov r3, 0");
    light_vm_push(state, "expushi r0");
    light_vm_push(state, "expushi r1");
    light_vm_push(state, "expushi r2");
    light_vm_push(state, "expushi r3");
    light_vm_push_fmt(state, "mov r4, 0x%llx", MessageBoxA);
    light_vm_push(state, "extcall r4");
    light_vm_push(state, "expop");
    light_vm_push(state, "hlt");

    // Should print Hello World!\n
    light_vm_execute(state, entry.absolute_address, 1);
}
#endif

void example8(Light_VM_State* state) {
    // copy test
    char str[] = "Hello World";
    light_vm_push_bytes_data_segment(state, (u8*)str, sizeof(str) - 1);
    
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r0, rdp");
    light_vm_push_fmt(state, "adds r0, %d", sizeof(str) - 1);
    light_vm_push(state, "mov r1, rdp");
    light_vm_push_fmt(state, "mov r2, %d", sizeof(str) - 1);
    light_vm_push(state, "copy r0, r1, r2");
    light_vm_push(state, "hlt");

    light_vm_execute(state, entry.absolute_address, 0);
    assert(memcmp((void*)state->registers[R0], (void*)state->registers[R1], sizeof(str) - 1) == 0);
}

void example9(Light_VM_State* state) {
    // copy and alloc test
    char str[] = "Hello World";
    light_vm_push_bytes_data_segment(state, (u8*)str, sizeof(str) - 1);
    
    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "mov r1, rdp");
    light_vm_push_fmt(state, "mov r2, %d", sizeof(str) - 1);
    light_vm_push(state, "alloc r0, r2");
    light_vm_push(state, "copy r0, r1, r2");
    light_vm_push(state, "hlt");

    light_vm_execute(state, entry.absolute_address, 0);
    assert(memcmp((void*)state->registers[R0], (void*)state->registers[RDP], sizeof(str) - 1) == 0);
}

s32 func_add(int a, int b) {
    return a + b;
}

void example10(Light_VM_State* state) {
    // test call of immediate absolute
    light_vm_push(state, "mov r0d, 2");
    light_vm_push(state, "mov r1d, 3");
    light_vm_push(state, "expushi r0d");
    light_vm_push(state, "expushi r1d");

    Light_VM_Instruction entry = {
        .type = LVM_EXTCALL,
        .imm_size_bytes = 8,
        .branch = {
            .addr_mode = BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE,
        },
    };
    Light_VM_Instruction_Info entry_info = light_vm_push_instruction(state, entry, 0);

    light_vm_push(state, "hlt");

    *(void**)(((Light_VM_Instruction*)entry_info.absolute_address) + 1) = func_add;

    light_vm_execute(state, entry_info.absolute_address, 0);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
}

void example11(Light_VM_State* state) {
    // put garbage in r1 and r2
	Light_VM_Instruction_Info entry =
    light_vm_push(state, "mov r1, 0xff");
    light_vm_push(state, "mov r2, 0xff");

    light_vm_push(state, "mov r0, 0x1");
    light_vm_push(state, "cmp r0, 0x0");
    light_vm_push(state, "moveq r1");
    light_vm_push(state, "movne r2");

    light_vm_push(state, "hlt");

    light_vm_execute(state, entry.absolute_address, 0);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
    assert(state->registers[R1] == 0 && state->registers[R2] == 1);
}

int main() {
    Light_VM_State* state = light_vm_init();

    example1(state);
    example2(state);
    example3(state);
    example4(state);
    example5(state);
    example6(state);
    example7(state);
    example8(state);
    example9(state);
    example10(state);
    example11(state);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_FLAGS_REGISTER|LVM_PRINT_DECIMAL);

    //Light_VM_Instruction_Info from = {0};
    //from.absolute_address = state->code.block - 0x02;
    //printf("0x%lx\n", light_vm_offset_from_current_instruction(state, from));
    //Light_VM_Instruction_Info info = light_vm_push_fmt(state, "beq 0x%lx", light_vm_offset_from_current_instruction(state, from));

    //light_vm_debug_dump_code(stdout, state);
    //light_vm_execute(state, 0);
    //light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);
    //light_vm_debug_dump_registers_dec(stdout, state);

    return 0;   
}

#if 0
    void* dll = dlopen("libc.so.6", RTLD_NOW);
    s64(*write_proc)(int, const void *, size_t) = (s64(*)(int, const void *, size_t))dlsym(dll, "write");

    write_proc(1, "foo", 3);
    //printf("%p\n", write_proc);
#endif