#include <ncurses.h>
#include <string.h>

#include "../../../util.h"

#include "tty.h"

#define TTY_AMOUNT 2

struct tty *ttys = NULL;
size_t ttys_size;

void CreateTty(struct tty tty) {
    ttys = srealloc(ttys, ++ttys_size * sizeof(struct tty));
    ttys[ttys_size - 1] = tty;
}

static void draw(struct tty *tty) {
    wmove(find_window(tty->window_name)->window, ((tty->tty_loc >> 5) & 0x07) + 1, (tty->tty_loc & 0x1f) + 1);
    waddch(find_window(tty->window_name)->window, tty->tty_char);
    find_window(tty->window_name)->dirty = true;
}

void Tty_Tick(void) {
    for (int i = 0; i < TTY_AMOUNT; i++) {
        if (console_window_kbhit(ttys[i].focused_window)) {
            ttys[i].user_ascii = console_window_getch(ttys[i].focused_window);
            ttys[i].user_ready = true;
        }
    }
}

bool Tty_OffsetInteraction(uint32_t offset) {
    for (int i = 0; i < TTY_AMOUNT; i++) {
        if (offset == ttys[i].user_ready_offset) return true;
        if (offset == ttys[i].user_ascii_offset) return true;
        if (offset == ttys[i].tty_loc_offset) return true;
        if (offset == ttys[i].tty_char_offset) return true;
        if (offset == ttys[i].tty_write_offset) return true;
        if (offset == ttys[i].tty_clear_offset) return true;
    }
    return false;
}

uint32_t Tty_ByteLoad(uint32_t offset) {
    for (int i = 0; i < TTY_AMOUNT; i++) {
        if (offset == ttys[i].user_ready_offset) {
            if (ttys[i].user_ready == false) {
                return ttys[i].user_ready;
            }
            if (ttys[i].user_ready == true) {
                ttys[i].user_ready = false;
                return !ttys[i].user_ready;
            }
        }
        else if (offset == ttys[i].user_ascii_offset) {
            return ttys[i].user_ascii;
        }
        else if (offset == ttys[i].tty_loc_offset) {
            return ttys[i].tty_loc;
        }
    }
    return 0;
}

void Tty_ByteStore(uint32_t offset, uint32_t value) {
    for (int i = 0; i < TTY_AMOUNT; i++) {
        if (offset == ttys[i].tty_loc_offset) ttys[i].tty_loc = value;
        else if (offset == ttys[i].tty_char_offset) ttys[i].tty_char = value;
        else if (offset == ttys[i].tty_write_offset) draw(&ttys[i]);
        else if (offset == ttys[i].tty_clear_offset) wclear(find_window(ttys[i].window_name)->window);
    }
}
