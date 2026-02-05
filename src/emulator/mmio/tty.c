#include <ncurses.h>

#include "tty.h"
#include "mmio_map.h"

#include "../../console/console.h"

void Tty_Init(void) {
    // initscr();
    // cbreak();
    // noecho();
    // nodelay(stdscr, TRUE);
}

static uint8_t tty_loc;
static uint8_t tty_char;
static uint8_t user_ascii;
static uint8_t user_ready;

static void draw(void) {
    wmove(tty_window, ((tty_loc >> 5) & 0x07) + 1, (tty_loc & 0x1f) + 1);
    waddch(tty_window, tty_char);
    tty_window_dirty = true;
}

void Tty_Tick(void) {
    if (console_window_kbhit(FOCUSED_WINDOW_TTY)) {
        user_ascii = console_window_getch(FOCUSED_WINDOW_TTY);
        user_ready = true;
    }
}

bool Tty_OffsetInteraction(uint32_t offset) {
    if (offset == USER_READY) return true;
    if (offset == USER_ASCII) return true;
    if (offset == TTY_LOC) return true;
    if (offset == TTY_CHAR) return true;
    if (offset == TTY_WRITE) return true;
    if (offset == TTY_CLEAR) return true;
    return false;
}

uint32_t Tty_ByteLoad(uint32_t offset) {
    switch (offset) {
        case USER_READY:
            if (user_ready == false) 
                return user_ready;
            if (user_ready == true) {
                user_ready = false;
                return !user_ready;
            }
            break;
        case USER_ASCII: return user_ascii; break;
        case TTY_LOC: return tty_loc;
    }
    return 0;
}

void Tty_ByteStore(uint32_t offset, uint32_t value) {
    switch (offset) {
        case TTY_LOC: tty_loc = value; break;
        case TTY_CHAR: tty_char = value; break;
        case TTY_WRITE:
            draw();
            break;
        case TTY_CLEAR:
            wclear(tty_window);
    }
}
