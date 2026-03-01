#pragma once

#define Intel8080_CUSTOM_BUS

#include "intel8080.h"

#include <stdint.h>

uint32_t External_Intel8080_LoadHandlerU8(uint8_t *image, uint32_t offset);
uint32_t External_Intel8080_LoadHandlerU16(uint8_t *image, uint32_t offset);
void External_Intel8080_StoreHandlerU8(uint8_t *image, uint32_t offset, uint32_t value);
void External_Intel8080_StoreHandlerU16(uint8_t *image, uint32_t offset, uint32_t value);