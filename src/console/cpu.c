#include <string.h>

#include "../util.h"
#include "console/console.h"

#include "../emulator/arch.h"
#include "../emulator/taurus/rv32izicsr.h"

extern int cpu_speed;

static void cpu_jmp_cmd(char *arg) {
    selected_cpu->set_pc(str_literal_to_ul(arg));
}

void handle_cpu_command(char *cmd) {
    strtok(cmd, " ");
    char *subcommand = strtok(NULL, " ");

    if (!strcmp(subcommand, "speed")) {
        cpu_speed = str_literal_to_ul(strtok(NULL, " "));
    }
    else if (!strcmp(subcommand, "jmp")) {
        selected_cpu->set_pc(str_literal_to_ul(strtok(NULL, " ")));
    }
    else if (!strcmp(subcommand, "rw")) {
        write_named_arch_register(strtok(NULL, " "), str_literal_to_ul(strtok(NULL, " ")));
    }
}
