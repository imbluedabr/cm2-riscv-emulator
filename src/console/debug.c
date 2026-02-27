#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../util.h"
#include "console/window.h"
#include "console/console.h"

#include "../emulator/arch.h"

#include "breakpoint.h"
#include "cpu.h"
#include "logger.h"

#include "../emulator/screen.h"

#define CMD_SIZE 128

extern int cpu_speed;

void handle_command(char *cmd) {
    if (!strcmp(cmd, "exit")) {
        endwin();
        exit(0);
    }
    else if (!strncmp(cmd, "logger", 6)) {
        handle_logger_command(cmd);
    }
    else if (!strcmp(cmd, "clear")) {
        find_window("debug")->ch_y = 1;
        find_window("debug")->ch_x= 1;
        wclear(get_window("debug"));
        update_window("debug");
    }
    else if (!strncmp(cmd, "scale", 5)) {
        char *end;
        scale = strtof(strtok(NULL, " "), &end);
    }
    else if (!strncmp(cmd, "cpu", 3)) {
        handle_cpu_command(cmd);
    }
    else if (!strncmp(cmd, "break", 5)) {
        handle_break_command(cmd);
    }
}

static void registers_window_tick(void) {
    char buf[128];
    arch_register_t arch_register;
    for (; (arch_register = selected_cpu->read_next_arch_register()).name;) {
        snprintf(buf, sizeof(buf), "%s\t0x%08x", arch_register.name, arch_register.value);
        window_puts("registers", buf);
    }
    snprintf(buf, sizeof(buf), "pc\t0x%08x", selected_cpu->get_pc());
    window_puts("registers", buf);
    find_window("registers")->ch_y = 1;
}

static void favmem_window_tick(void) {
    int i = 0;
    for (int y = 0; y < find_window("favmem")->height; y++) {
        for (int x = 0; x < find_window("favmem")->width; x++) {
            mvwaddch(get_window("favmem"), y, x, ((uint8_t *)selected_cpu->image)[(selected_cpu->get_mar() + (i++)) & 0xFFFF]); // ffff won't work on 32 bit
        }
    }
}

void debug_console_tick(void) {
    registers_window_tick();
    favmem_window_tick();

    static char buf[CMD_SIZE];
    static int i = 0;

    char ch = console_window_getch(FOCUSED_WINDOW_DBG_CONSOLE);
    if (ch != ERR) {
        if (ch == '\r' || ch == '\n') {
            buf[i] = '\0';
            i = 0;
            window_putc("debug", ch);
            handle_command(buf);
        }
        else if (ch == 127 || ch == 8) {
            if (i != 0) {
                buf[--i] = '\0';
                find_window("debug")->ch_x--;
                window_putc("debug", ' ');
                find_window("debug")->ch_x--;
            }
        }
        else {
            buf[i++] = ch;
            window_putc("debug", ch);
        }
    }
}
