#include <pthread.h>
#include <stdlib.h>
#include <string.h>

//#include "emulator/mmio/mmio_map.h"
#include "global.h"
#include "util.h"

#include "console/console/console.h"
#include "console/debug.h"
#include "console/breakpoint.h"

#include "emulator/arch.h"

#ifdef RAYLIB
#include "emulator/screen.h"
#endif

#include "cpulog.h"

static void load_bin_file(const char *filename, void *dest, size_t dest_size) {
    FILE *fptr = fopen(filename, "r");
    if (!fptr) {
        app_abort("load_bin_file()", "Unable to obtain file handle")
    }

    fseek(fptr, 0, SEEK_END);
    rewind(fptr);
    fread(dest, 1, dest_size - 1, fptr);
    fclose(fptr);
}

// struct RV32IZicsr_State state;
bool cpu_running = false;
int cpu_speed = 10000; // the default speed of taurus is 5 ?
uint8_t *image = NULL;

void cpu_step(void) {
    breakpoint_tick();
    selected_cpu->step(image);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        quick_abort("Usage: cm2-riscv-emulator <arch> <Filepath to initial .bin> <Filepath to tilegpu .bmp> <Filepath to disk .bin image>")
    }

    select_cpu(argv[1]);
    if (!selected_cpu) {
        quick_abort("Invalid CPU selection.")
    }

    /* Init Console */
    console_create_windows();

    /* Init selected CPU */
    image = scalloc(1, selected_cpu->ram_size);
    
    load_bin_file(argv[2], image, selected_cpu->ram_size);

    selected_cpu->init(image);
    selected_cpu->sys_init(&(void *[]){&argc, &argv});

    while (1) {
        console_tick();
        debug_console_tick();
        
        selected_cpu->sys_tick(NULL);

        if (cpu_running) {
            for (int i = 0; i < cpu_speed; i++)
            {
                cpu_step();
                cpu_logger_cycle();
                if (!cpu_running) break;
            }
        }
    }
}
