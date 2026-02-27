#include <string.h>

#include "intel8080.h"

void Intel8080_InitState(struct Intel8080_State *state) {
    memset(state, 0, sizeof(*state));
}

enum word_register {
    B_REG,
    C_REG,
    D_REG,
    E_REG,
    H_REG,
    L_REG,
    MEM,
    A_REG
};

static uint8_t Intel8080_ReadWReg(struct Intel8080_State *state, uint8_t *image, enum word_register reg) {
    switch (reg) {
        default:
        case B_REG: return state->b; break;
        case C_REG: return state->c; break;
        case D_REG: return state->d; break;
        case E_REG: return state->e; break;
        case H_REG: return state->h; break;
        case L_REG: return state->l; break;
        case MEM: return Intel8080_LoadU8(image, (state->h << 8) | state->l); break;
        case A_REG: return state->a;
    }
}

static void Intel8080_WriteWReg(struct Intel8080_State *state, uint8_t *image, enum word_register reg, uint8_t val) {
    switch (reg) {
        default:
        case B_REG: state->b = val; break;
        case C_REG: state->c = val; break;
        case D_REG: state->d = val; break;
        case E_REG: state->e = val; break;
        case H_REG: state->h = val; break;
        case L_REG: state->l = val; break;
        case MEM: Intel8080_StoreU8(image, (state->h << 8) | state->l, val); break;
        case A_REG: state->a = val;
    }
}

#define RWREG(reg) (Intel8080_ReadWReg(state, image, reg))
#define WWREG(reg, val) (Intel8080_WriteWReg(state, image, reg, val))

static void Intel8080_Set_SZP(struct Intel8080_State *state, uint8_t x) {
    if (x & 0x80) {
        state->s = 1;
    }
    if (x == 0) {
        state->z = 0;
    }
    if (__builtin_parity(x)) {
        state->p = 1;
    }
}

static uint8_t Intel8080_Add(struct Intel8080_State *state, uint8_t x, uint8_t y) {
    uint8_t result = x + y;
    Intel8080_Set_SZP(state, result);
    state->ac = ((x & 0x0f) + (x & 0x0f)) > 0x0f;
    return result;
}

void Intel8080_Step(struct Intel8080_State *state, uint8_t *image) {
    uint32_t pc = state->pc;
    uint32_t ir;

    ir = Intel8080_LoadU8(image, pc);
    state->ir = ir; // cosmetic

    uint8_t limm = Intel8080_LoadU8(image, pc + 1),
            himm = Intel8080_LoadU8(image, pc + 2),
            imm = Intel8080_LoadU16(image, pc + 1);

    uint8_t wdest = (ir & 0b00111000) >> 3,
            wsrc = (ir & 0b00000111);

    uint8_t wdest_opcode = 0b11000111;
    uint8_t wsrcdest_opcode = 0b11000000;

    // Instructions operating on one register dest
    switch (ir & wdest_opcode) {
        case 0b00000100: WWREG(wdest, RWREG(wdest)+1); state->pc += 1; break; // INR
        case 0b00000101: WWREG(wdest, RWREG(wdest)-1); state->pc += 1; break; // DCR
    }
    // Instructions operating on register dest + src
    switch (ir & wsrcdest_opcode) {
        case 0b01000000: WWREG(wdest, RWREG(wsrc)); state->pc += 1; break; // MOV
    }
    // Basic instructions
    switch (ir) {
        case 0: state->pc += 1; break; // NOP
        case 0b00111111: state->cy = !state->cy; state->pc += 1; break; // CMC
        case 0b00110111: state->cy = 1; state->pc += 1; break; // STC
        case 0b00101111: state->a = ~(state->a); state->pc += 1; break; // CMA
        case 0b00100111: { // DAA
            uint8_t correction = 0;

            if ((state->a & 0x0f) > 9 || state->ac)
                correction |= 0x06;
            if ((state->a >> 4) > 9 || state->cy)
                correction |= 0x60;
            state->a = Intel8080_Add(state, state->a, correction);
            state->pc += 1; 
            break;
        }
        case 0b11000011: state->pc = imm; break; // JMP
        default: state->pc += 1; break;
    }   
}