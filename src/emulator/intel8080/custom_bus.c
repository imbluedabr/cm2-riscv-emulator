#include "intel8080.h"

#include "../mmio/shared/tty.h"

uint32_t External_Intel8080_LoadHandlerU8(uint8_t *image, uint32_t offset) {
    if (Tty_OffsetInteraction(offset)) {
        return Tty_ByteLoad(offset);
    }
    return ((uint32_t)*(uint8_t *)(image + (offset & Intel8080_RAM_MASK)));
}


uint32_t External_Intel8080_LoadHandlerU16(uint8_t *image, uint32_t offset) {
    return ((uint32_t)*(uint16_t *)(image + (offset & Intel8080_RAM_MASK)));
}

void External_Intel8080_StoreHandlerU8(uint8_t *image, uint32_t offset, uint32_t value) {
    if (Tty_OffsetInteraction(offset)) {
        Tty_ByteStore(offset, value);
    }
    (*(uint8_t *)(image + (offset & Intel8080_RAM_MASK)) = value);
}

void External_Intel8080_StoreHandlerU16(uint8_t *image, uint32_t offset, uint32_t value) {
    (*(uint16_t *)(image + (offset & Intel8080_RAM_MASK)) = value);
}

