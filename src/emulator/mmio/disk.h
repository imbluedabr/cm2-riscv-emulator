#include <stdint.h>
#include <stdbool.h>

#pragma once

bool Disk_OffsetInteraction(uint32_t offset);
uint32_t Disk_ByteLoad(uint32_t offset);
void Disk_ByteStore(uint32_t offset, uint32_t value);
void Disk_HalfStore(uint32_t offset, uint32_t value);