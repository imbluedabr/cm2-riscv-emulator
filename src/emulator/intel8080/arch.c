#include <stddef.h>
#include <stdlib.h>

#include "../../util.h"

#include "arch.h"
#include "intel8080.h"

cpu_t intel8080 = {
    .id = INTEL8080,
    .name = "8080",
    .sys_init = intel8080_sys_init,
    .init = intel8080_init,
    .step = intel8080_step,
    .sys_tick = intel8080_sys_tick,
    .get_pc = intel8080_get_pc,
    .set_pc = intel8080_set_pc,
    .get_ir = intel8080_get_ir,
    .get_mar = intel8080_get_mar,
    .read_next_arch_register = intel8080_read_next_arch_register,
    .write_arch_register = intel8080_write_arch_register,
    .ram_size = Intel8080_RAM_SIZE,
    .register_amount = 13,
    .image = NULL,
};

void *intel8080_sys_init(void *args) {
    int argc = *(int *)(((void **)args)[0]);
    char **argv = *(char ***)(((void **)args)[1]);

    return NULL;
}

void *intel8080_init(void *args) {
    intel8080.state = smalloc(sizeof(struct Intel8080_State));
    Intel8080_InitState(intel8080.state);
    intel8080.image = args;
    return NULL;
}

void *intel8080_step(void *args) {
    Intel8080_Step(intel8080.state, intel8080.image);
    return NULL;
}

void *intel8080_sys_tick(void *args) {
    return NULL;
}

uint32_t intel8080_get_pc(void) {
    return ((struct Intel8080_State *)intel8080.state)->pc;
}

void intel8080_set_pc(uint32_t val) {
    ((struct Intel8080_State *)intel8080.state)->pc = val;
}

uint32_t intel8080_get_ir(void) {
    return ((struct Intel8080_State *)intel8080.state)->ir;
}

uint32_t intel8080_get_mar(void) {
    return ((struct Intel8080_State *)intel8080.state)->mar;
}

arch_register_t intel8080_read_next_arch_register(void) {
    struct Intel8080_State *state = intel8080.state;

    static int i = 0;
    static arch_register_t arch_register;

    switch (i) {
        case 0: arch_register.name = "b"; arch_register.value = state->b; break;
        case 1: arch_register.name = "c"; arch_register.value = state->c; break;
        case 2: arch_register.name = "d"; arch_register.value = state->d; break;
        case 3: arch_register.name = "e"; arch_register.value = state->e; break;
        case 4: arch_register.name = "h"; arch_register.value = state->h; break;
        case 5: arch_register.name = "l"; arch_register.value = state->l; break;
        case 6: arch_register.name = "a"; arch_register.value = state->a; break;
        case 7: arch_register.name = "sp"; arch_register.value = state->sp; break;
        case 8: arch_register.name = "zf"; arch_register.value = state->z; break;
        case 9: arch_register.name = "sf"; arch_register.value = state->s; break;
        case 10: arch_register.name = "pf"; arch_register.value = state->p; break;
        case 11: arch_register.name = "cf"; arch_register.value = state->cy; break;
        case 12: arch_register.name = "acf"; arch_register.value = state->ac; break;
        default: arch_register.name = NULL; i = 0; return arch_register;
    }

    arch_register.id = i;
    i++;
    return arch_register;
}

void intel8080_write_arch_register(int id, uint32_t val) {
    struct Intel8080_State *state = intel8080.state;

    switch (id) {
        case 0: state->b = val; break;
        case 1: state->c = val; break;
        case 2: state->d = val; break;
        case 3: state->e = val; break;
        case 4: state->h = val; break;
        case 5: state->l = val; break;
        case 6: state->a = val; break;
        case 7: state->sp = val; break;
        case 9: state->s = val; break;
        case 8: state->z = val; break;
        case 10: state->p = val; break;
        case 11: state->cy = val; break;
        case 12: state->ac = val; break;
    }
}