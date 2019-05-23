#include <stdio.h>
#include "ast.h"
#include "lightvm.h"
#include "light_array.h"

void* push_float(Light_VM_State* state, r32 f) {
    Light_VM_Data data = {0};
    data.byte_size = 4;
    data.float32 = f;
    return light_vm_push_data_segment(state, data);
}
void* push_double(Light_VM_State* state, r64 f) {
    Light_VM_Data data = {0};
    data.byte_size = 8;
    data.float64 = f;
    return light_vm_push_data_segment(state, data);
}

int main() {
    Light_VM_State* state = light_vm_init();

    void* addr1 = push_float(state, 1.34f);
    void* addr2 = push_float(state, 3.14f);

    light_vm_push(state, "mov r0, 0x5");
    light_vm_push(state, "mov r1, 0x5");
    light_vm_push(state, "cmp r0, r1");
    Light_VM_Instruction_Info b = light_vm_push(state, "beq 0x09");
    light_vm_push(state, "hlt");

    //light_vm_debug_dump_code(stdout, state);
    light_vm_execute(state);
    light_vm_debug_dump_registers(stdout, state);
    //light_vm_debug_dump_registers_dec(stdout, state);

    return 0;   
}