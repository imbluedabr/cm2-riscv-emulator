#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../util.h"

#include "arch.h"
#include "rv32izicsr.h"

#include "../mmio/taurus/tty.h"
#include "../mmio/taurus/tilegpu.h"
#include "../mmio/taurus/disk.h"
#include "../mmio/taurus/spritegpu.h"

cpu_t taurus = {
    .id = TAURUS,
    .name = "taurus",
    .sys_init = taurus_sys_init,
    .init = taurus_init,
    .step = taurus_step,
    .sys_tick = taurus_sys_tick,
    .get_pc = taurus_get_pc,
    .set_pc = taurus_set_pc,
    .get_ir = taurus_get_ir,
    .get_mar = taurus_get_mar,
    .read_next_arch_register = taurus_read_next_arch_register,
    .write_arch_register = taurus_write_arch_register,
    .ram_size = RV32IZicsr_RAM_SIZE,
    .register_amount = 32,
    .image = NULL,
};

void *taurus_sys_init(void *args) {
    int argc = *(int *)(((void **)args)[0]);
    char **argv = *(char ***)(((void **)args)[1]);

    Tty_Init();

    /* Init TileGPU and Disk */
    #ifdef RAYLIB
    if (argc == 6) {
        TileGpu_Init(argv[3]);
        Disk_LoadBin(argv[4]);
        SpriteGpu_Init(argv[5]);
    }
    #endif
    if (argc == 5)
        Disk_LoadBin(argv[3]);
    return NULL;
}

void *taurus_init(void *args) {
    taurus.state = smalloc(sizeof(struct RV32IZicsr_State));
    RV32IZicsr_InitState(taurus.state);
    taurus.image = args;
    return NULL;
}

void *taurus_step(void *args) {
    RV32IZicsr_Step(taurus.state, taurus.image);
    return NULL;
}

void *taurus_sys_tick(void *args) {
    Tty_Tick();
    #ifdef RAYLIB
    TileGpu_Tick();
    SpriteGpu_Tick();
    #endif
    return NULL;
}

uint32_t taurus_get_pc(void) {
    return ((struct RV32IZicsr_State *)taurus.state)->pc;
}

void taurus_set_pc(uint32_t val) {
    ((struct RV32IZicsr_State *)taurus.state)->pc = val;
}

uint32_t taurus_get_ir(void) {
    return ((struct RV32IZicsr_State *)taurus.state)->ir;
}

uint32_t taurus_get_mar(void) {
    return ((struct RV32IZicsr_State *)taurus.state)->mar;
}

arch_register_t taurus_read_next_arch_register(void) {
    struct RV32IZicsr_State *state = taurus.state;

    static int i = 0;
    static arch_register_t arch_register;

    static char buf[4];
    
    if (i > 31) {
        i = 0;
        arch_register.name = NULL;
        return arch_register;
    }
    snprintf(buf, sizeof(buf), "x%d", i);
    
    arch_register.name = buf;
    arch_register.value = state->regs[i];
    arch_register.id = i;
    i++;
    return arch_register;
}

void taurus_write_arch_register(int id, uint32_t val) {
    struct RV32IZicsr_State *state = taurus.state;

    state->regs[id] = val;
}
