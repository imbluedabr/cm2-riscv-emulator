#include <string.h>

#include "../util.h"
#include "console/console.h"
#include "../emulator/rv32izicsr.h"

extern struct RV32IZicsr_State state;
extern int cpu_speed;

static void cpu_speed_cmd(char *arg) {
    cpu_speed = str_literal_to_ul(arg);
}

static void cpu_jmp_cmd(char *arg) {
    state.pc = str_literal_to_ul(arg);
}

void handle_cpu_command(char *cmd) {
    strtok(cmd, " ");
    char *subcommand = strtok(NULL, " ");

    if (!strncmp(subcommand, "speed", 3)) {
        cpu_speed_cmd(strtok(NULL, " "));
    }
    else if (!strncmp(subcommand, "jmp", 3)) {
        cpu_jmp_cmd(strtok(NULL, " "));
    }    
}
