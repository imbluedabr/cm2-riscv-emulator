#pragma once

#include "../arch.h"
#include "intel8080.h"

void *intel8080_sys_init(void *args);
void *intel8080_init(void *args);
void *intel8080_step(void *args);
void *intel8080_sys_tick(void *args);
uint32_t intel8080_get_pc(void);
void intel8080_set_pc(uint32_t val);
uint32_t intel8080_get_ir(void);
uint32_t intel8080_get_mar(void);
arch_register_t intel8080_read_next_arch_register(void);
void intel8080_write_arch_register(int id, uint32_t val);

extern cpu_t intel8080;