#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Configuration */
#define Intel8080_RAM_SIZE 65536
#define Intel8080_RAM_MASK (Intel8080_RAM_SIZE - 1)
#include "custom_bus.h"

#ifndef Intel8080_CUSTOM_BUS
   #define Intel8080_LoadU8(image, offset) ((uint32_t)*(uint8_t *)((image) + ((offset) & Intel8080_RAM_MASK)))
   #define Intel8080_LoadU16(image, offset) ((uint32_t)*(uint16_t *)((image) + ((offset) & Intel8080_RAM_MASK)))

   #define Intel8080_StoreU8(image, offset, value) (*(uint8_t *)((image) + ((offset) & Intel8080_RAM_MASK)) = (value))
   #define Intel8080_StoreU16(image, offset, value) (*(uint16_t *)((image) + ((offset) & Intel8080_RAM_MASK)) = (value))
#else
   #define Intel8080_LoadU8(image, offset) (External_Intel8080_LoadHandlerU8((image), (offset)))
   #define Intel8080_LoadU16(image, offset) (External_Intel8080_LoadHandlerU16((image), (offset)))

   #define Intel8080_StoreU8(image, offset, value) (External_Intel8080_StoreHandlerU8((image), (offset), (value)))
   #define Intel8080_StoreU16(image, offset, value) (External_Intel8080_StoreHandlerU16((image), (offset), (value)))
#endif

struct Intel8080_State {
   uint8_t a;
   uint8_t b;
   uint8_t c;
   uint8_t d;
   uint8_t e;
   uint8_t h;
   uint8_t l;

   uint16_t sp;
   uint16_t pc;

   bool  z,
         s,
         p,
         cy,
         ac;

   uint32_t ir, mar; // cosmetic
};

void Intel8080_InitState(struct Intel8080_State *state);
void Intel8080_Step(struct Intel8080_State *state, uint8_t *image);
bool Intel8080_ImageRead(void);
uint32_t Intel8080_GetImageReadAddr();
