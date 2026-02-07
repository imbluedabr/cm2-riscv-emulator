#pragma once

#include <stdint.h>

extern uint32_t protection_region_base;
extern uint32_t protection_region_top;
extern struct RV32IZicsr_State state;

void memprot_tick(void);
void memprot(char* cmd);
void memunprot(void);
