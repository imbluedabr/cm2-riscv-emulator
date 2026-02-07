#include <ncurses.h>

#include "../../util.h"
#include "console.h"

#include "window.h"

enum focused_window current_focused_window;

void console_create_windows(void) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    create_window("tty", 8 + 2, 32 + 2, 0, 0);
    create_window("debug", 32, 32, 0, 32 + 2);
    create_window("registers", 33 + 2, 17 + 2, 0, 64 + 2);
    
    current_focused_window = FOCUSED_WINDOW_TTY;
}

static bool iskbhit;
static char ch;

#define ctrl(x) ((x) & 0x1f)

static void console_process_char(void) {
    switch (ch) {
        case ctrl('w'):
            iskbhit = false;
            current_focused_window++;
            if (current_focused_window == FOCUSED_WINDOW_MAX) current_focused_window = 0;
            ch = ERR;
            break;
    }
}

void console_tick(void) {
    find_window("registers")->dirty = true;
    update_windows();
    
    ch = getch();
    if (ch != ERR) {
        iskbhit = true;
        console_process_char();
    }
}

int console_window_kbhit(enum focused_window window) {
    if (iskbhit && current_focused_window == window) return 1;
    return 0;
}

int console_window_getch(enum focused_window window) {
    if (!iskbhit) return ERR;
    if (current_focused_window == window) {
        iskbhit = false;
        return ch;
    }
    return ERR;
}
