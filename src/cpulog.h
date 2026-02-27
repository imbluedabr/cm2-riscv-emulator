#pragma once

#include <stdint.h>

/* should work for RV32I and 8080 */
#define MAX_REGS_SIZE 32

struct cycle_dump {
  uint32_t pc;
  uint32_t regs[MAX_REGS_SIZE];
};

void init_cpu_logger(const char *filename);
void toggle_cpu_logger(void);
void flush_cpu_logger(void);
void cpu_logger_cycle(void);
void cpu_logger_save_snapshot(
    const char *state_bin_filename,
    const char *memory_bin_filename
);
void cpu_logger_load_snapshot(
    const char *state_bin_filename,
    const char *memory_bin_filename
);