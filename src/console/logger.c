#include <string.h>

#include "../cpulog.h"

void handle_logger_command(char *cmd) {
    strtok(cmd, " ");
    char *subcommand = strtok(NULL, " ");

    if (!strcmp(subcommand, "init")) {
        init_cpu_logger(strtok(NULL, " "));
    }
    else if (!strcmp(subcommand, "toggle")) {
        toggle_cpu_logger();
    }
    else if (!strcmp(subcommand, "flush")) {
        flush_cpu_logger();
    }
    else if (!strcmp(subcommand, "save")) {
        cpu_logger_save_snapshot(strtok(NULL, " "), strtok(NULL, " "));
    }
    else if (!strcmp(subcommand, "load")) {
        cpu_logger_load_snapshot(strtok(NULL, " "), strtok(NULL, " "));
    }
}