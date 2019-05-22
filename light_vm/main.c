#include <stdio.h>
#include "ast.h"
#include "lightvm.h"
#include "light_array.h"

int main() {
    Light_VM_State* state = light_vm_init();

    light_vm_push(state, "mov r0, [r1 + 0x123]");
    light_vm_push(state, "adds [r0], r1");

    light_vm_debug_dump_code(stdout, state);

    return 0;   
}