#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../util.h"
#include "console/window.h"
#include "console/console.h"
#include "breakpoint.h"
#include "cpu.h"

#include "../emulator/rv32izicsr.h"
#include "../emulator/mmio/tilegpu.h"

#define CMD_SIZE 128

extern uint8_t *image;
extern struct RV32IZicsr_State state;
extern int cpu_speed;

void handle_command(char *cmd) {
    if (!strcmp(cmd, "exit")) {
        endwin();
        exit(0);
    }
    else if (!strcmp(cmd, "clear")) {
        find_window("debug")->ch_y = 1;
        find_window("debug")->ch_x= 1;
        wclear(get_window("debug"));
        update_window("debug");
    }
    else if (!strncmp(cmd, "cpu", 3)) {
        handle_cpu_command(cmd);
    }
    else if (!strncmp(cmd, "break", 5)) {
        handle_break_command(cmd);
    }
}

/*
    Making a new file for the registers window is overkill,
    so settle with this
*/
static void registers_window_tick(void) {
    char buf[128];
    for (unsigned int i = 0; i < sizeof(state.regs)/sizeof(state.regs[0]); i++) {
        snprintf(buf, sizeof(buf), "x%d\t0x%08x", i, state.regs[i]);
        window_puts("registers", buf);
    }
    snprintf(buf, sizeof(buf), "pc\t0x%08x", state.pc);
    window_puts("registers", buf);
    find_window("registers")->ch_y = 1;
}

static void favmem_window_tick(void) {
    int i = 0;
    for (int y = 0; y < find_window("favmem")->height; y++) {
        for (int x = 0; x < find_window("favmem")->width; x++) {
            mvwaddch(get_window("favmem"), y, x, image[(interacted_address + (i++)) & RV32IZicsr_RAM_MASK]);
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

    breakpoint_tick();
}
