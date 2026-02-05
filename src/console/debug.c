#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "console.h"
#include "breakpoint.h"

int puts_y = 1;

void debug_console_puts(const char *s) {
    mvwprintw(dbg_console_window, puts_y++, 1, s);    
}

void handle_command(char *cmd) {
    if (!strcmp(cmd, "exit")) {
        endwin();
        exit(0);
    }
    else if (!strcmp(cmd, "clear")) {
        puts_y = 1;
        wclear(dbg_console_window);
    }
    else if (!strcmp(cmd, "break pop")) {
        breakpoint_pop_cmd();
    }
    else if (!strcmp(cmd, "break continue")) {
        break_continue_cmd();
    }
    else if (!strncmp(cmd, "break", 5)) {
        breakpoint_cmd(cmd);
    }
}

void debug_console_tick(void) {
    breakpoint_tick();

    char *cmd = console_window_getstr(FOCUSED_WINDOW_DBG_CONSOLE);
    if (!cmd) return;

    static int y = 1;
    debug_console_puts(cmd);

    handle_command(cmd);
}