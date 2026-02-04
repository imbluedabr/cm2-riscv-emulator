#include <ncurses.h>

#include "console.h"

enum focused_window current_focused_window;

WINDOW *tty_window;
WINDOW *dbg_console_window;

void console_create_windows(void) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    tty_window = newwin(8 + 2, 32 + 2, 0, 0);
    dbg_console_window = newwin(512, 512, 0, 32 + 2);
    
    current_focused_window = FOCUSED_WINDOW_TTY;
}

static int kbhit(void) {
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
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
    box(tty_window, 0, 0);
    wrefresh(tty_window);
    box(dbg_console_window, 0, 0);
    wrefresh(dbg_console_window);
    if ((iskbhit = kbhit())) {
        ch = getch();
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
        return ch;
    }
    return ERR;
}

const char *console_window_getstr(enum focused_window window) {
    if (current_focused_window != window) return NULL;

    static char buf[1024];
    static int i = 0;

    while (1) {
        console_tick();
        if (!iskbhit) {
            if (i != 0 && ch == ERR) return NULL;
            continue;
        }

        if (ch == '\n') {
            buf[i] = '\0';
            i = 0;
            return buf;
        }
        buf[i++] = ch;
    }
}