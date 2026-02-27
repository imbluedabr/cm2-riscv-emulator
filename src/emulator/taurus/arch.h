#pragma once

#include "../arch.h"
#include "rv32izicsr.h"

void *taurus_sys_init(void *args);
void *taurus_init(void *args);
void *taurus_step(void *args);
void *taurus_sys_tick(void *args);
uint32_t taurus_get_pc(void);
void taurus_set_pc(uint32_t val);
uint32_t taurus_get_ir(void);
uint32_t taurus_get_mar(void);
arch_register_t taurus_read_next_arch_register(void);
void taurus_write_arch_register(int id, uint32_t val);

extern cpu_t taurus;