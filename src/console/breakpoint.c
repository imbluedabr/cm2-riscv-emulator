#include <string.h>

#include "../util.h"
#include "console.h"
#include "debug.h"
#include "../emulator/rv32izicsr.h"

extern struct RV32IZicsr_State state;

bool canrun;

uint32_t *breakpoints = NULL;
size_t breakpoints_size = 0;

void breakpoint_tick(void) {
    if (!canrun) return;
    for (size_t i = 0; i < breakpoints_size; i++) {
        if (state.pc > breakpoints[i]) {
            state.running = false;
            debug_console_puts("Breakpoint hit");
        }
    }
}

void breakpoint_cmd(char *cmd) {
    cmd = strtok(cmd, " ");
    breakpoints = srealloc(breakpoints, ++breakpoints_size * sizeof(uint32_t));
    breakpoints[breakpoints_size - 1] = str_literal_to_ul(strtok(NULL, " "));
    debug_console_puts("Breakpoint added");
}

void breakpoint_pop_cmd(void) {
    breakpoints = srealloc(breakpoints, --breakpoints_size * sizeof(uint32_t));
}

void break_continue_cmd(void) {
    state.running = true;
}
