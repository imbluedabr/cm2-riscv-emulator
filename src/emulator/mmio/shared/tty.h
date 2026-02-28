#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../../../console/console/console.h"

struct tty {
    uint32_t user_ready_offset;
    uint32_t user_ascii_offset;
    uint32_t tty_loc_offset;
    uint32_t tty_char_offset;
    uint32_t tty_write_offset;
    uint32_t tty_clear_offset;

    uint8_t tty_loc;
    uint8_t tty_char;
    uint8_t user_ascii;
    uint8_t user_ready;

    const char *window_name;
    enum focused_window focused_window;
};

void CreateTty(struct tty tty);
void Tty_Tick(void);

/* below are for custom_bus.c */

bool Tty_OffsetInteraction(uint32_t offset);
uint32_t Tty_ByteLoad(uint32_t offset);
void Tty_ByteStore(uint32_t offset, uint32_t value);