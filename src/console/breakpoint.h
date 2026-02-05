#pragma once

#include <stdbool.h>

extern bool canrun;

void breakpoint_tick(void);
void breakpoint_cmd(char *cmd);
void breakpoint_pop_cmd(void);
void break_continue_cmd(void);