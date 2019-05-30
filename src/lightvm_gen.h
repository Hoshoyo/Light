#include <lightvm.h>
#include "ast.h"

const u32 LVM_REGISTER_ALLOCATED = FLAG(0);

struct LVM_Register_Info {
    u32 flags;
};

struct LVM_Gen_Register_Allocation {
    LVM_Register_Info int_registers[R7 + 1];
    LVM_Register_Info f32_registers[FREG_COUNT];
    LVM_Register_Info f64_registers[FREG_COUNT];
};

Light_VM_State* lvm_generate(Ast** ast);