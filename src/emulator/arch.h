#pragma once

#include <stddef.h>
#include <stdint.h>

enum cpu_id {
    TAURUS,
    INTEL8080
};

typedef struct {
    const char *name;
    int id;
    uint32_t value;
} arch_register_t;

typedef struct {
    enum cpu_id id;
    const char *name;

    void *(*sys_init)(void *);
    void *(*init)(void *);
    void *(*step)(void *);
    void *(*sys_tick)(void *);

    uint32_t (*get_pc)(void);
    void (*set_pc)(uint32_t val);
    uint32_t (*get_ir)(void);
    uint32_t (*get_mar)(void);

    arch_register_t (*read_next_arch_register)(void);
    void (*write_arch_register)(int id, uint32_t val);

    void *state;

    void *image;
    size_t ram_size;
    int register_amount;
} cpu_t;

extern cpu_t *selected_cpu;

void select_cpu(const char *name);
void write_named_arch_register(const char *name, uint32_t val);
uint32_t get_pc(void);
uint32_t set_pc(uint32_t val);
uint32_t get_ir(void);
uint32_t get_mar(void);
const char *instruction_to_str(uint32_t ir);