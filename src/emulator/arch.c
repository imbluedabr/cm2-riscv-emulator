#include <string.h>
#include <stdlib.h>

#include "arch.h"

#include "taurus/arch.h"
#include "intel8080/arch.h"

#include "../riscv.h"

cpu_t *selected_cpu = NULL;

void select_cpu(const char *name) {
    if (!strcmp(name, taurus.name)) {
        selected_cpu = &taurus;
    }
    else if (!strcmp(name, intel8080.name)) {
        selected_cpu = &intel8080;
    }
}

void write_named_arch_register(const char *name, uint32_t val) {
    arch_register_t arch_register;
    while (1) {
        arch_register = selected_cpu->read_next_arch_register();
        if (!arch_register.name) return;
        if (!strcmp(arch_register.name, name)) {
            selected_cpu->write_arch_register(arch_register.id, val);
            break;
        }
    }
}

const char *instruction_to_str(uint32_t ir) {
    switch (selected_cpu->id) {
        default:
        case TAURUS: return rv32i_instruction_to_str(ir);
    }
}
