#include <stdio.h>
#include <stdbool.h>

#include "global.h"
#include "util.h"
#include "cpulog.h"
#include "emulator/arch.h"

bool enabled = false;
struct cycle_dump *cpu_log = NULL;
size_t cpu_log_size = 0;
FILE *log_file = NULL;

void init_cpu_logger(const char *filename) {
    log_file = fopen(filename, "wb");
    if (!log_file) {
        app_abort("init_cpu_logger()", "Unable to obtain file handle")
    }
}

void toggle_cpu_logger(void) {
    enabled = !enabled;
}

void flush_cpu_logger(void) {
    fwrite(cpu_log, sizeof(*cpu_log), cpu_log_size, log_file);
}

void cpu_logger_cycle(void) {
    if (!enabled) return;

    cpu_log = srealloc(cpu_log, ++cpu_log_size * sizeof(struct cycle_dump));
    cpu_log[cpu_log_size - 1].pc = selected_cpu->get_pc();

    arch_register_t arch_register;
    for (int i = 0; (arch_register = selected_cpu->read_next_arch_register()).name; i++) {
        cpu_log[cpu_log_size - 1].regs[i] = arch_register.value;
    }
}

void cpu_logger_save_snapshot(
    const char *state_bin_filename,
    const char *memory_bin_filename
) {
    FILE *state_bin = fopen(state_bin_filename, "wb");
    if (!state_bin) {
        app_abort("cpu_logger_save_snapshot()", "Failed to open file handle")
    }

    struct cycle_dump state_dump;
    state_dump.pc = selected_cpu->get_pc();
    
    arch_register_t arch_register;
    for (; (arch_register = selected_cpu->read_next_arch_register()).name;) {
        state_dump.regs[arch_register.id] = arch_register.value;
    }

    fwrite(&state_dump, sizeof(state_dump), 1, state_bin);
    fclose(state_bin);

    FILE *memory_bin = fopen(memory_bin_filename, "wb");
    if (!memory_bin) {
        app_abort("cpu_logger_save_snapshot()", "Failed to open file handle")
    }

    fwrite(selected_cpu->image, selected_cpu->ram_size, sizeof(*selected_cpu->image), memory_bin);
    fclose(memory_bin);
}

void cpu_logger_load_snapshot(
    const char *state_bin_filename,
    const char *memory_bin_filename
) {
    FILE *state_bin = fopen(state_bin_filename, "rb");
    if (!state_bin) {
        app_abort("cpu_logger_save_snapshot()", "Failed to open file handle")
    }

    struct cycle_dump *state_dump = smalloc(sizeof(struct cycle_dump));
    fread(state_dump, sizeof(struct cycle_dump), 1, state_bin);

    selected_cpu->set_pc(state_dump->pc);
    for (int i = 0; i < selected_cpu->register_amount; i++) {
        selected_cpu->write_arch_register(i, state_dump->regs[i]);
    }

    free(state_dump);
    fclose(state_bin);

    FILE *memory_bin = fopen(memory_bin_filename, "rb");
    if (!memory_bin) {
        app_abort("cpu_logger_save_snapshot()", "Failed to open file handle");
    }

    fread(selected_cpu->image, selected_cpu->ram_size, sizeof(*selected_cpu->image), memory_bin);
    fclose(memory_bin);
}
