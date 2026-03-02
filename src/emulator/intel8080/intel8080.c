#include <string.h>

#include "intel8080.h"
#include "custom_bus.h"

void Intel8080_InitState(struct Intel8080_State *state) {
    memset(state, 0, sizeof(*state));
}

static uint8_t Intel8080_ReadWReg(struct Intel8080_State *state, uint8_t *image, uint8_t reg) {
    switch (reg) {
        case 0: return state->b;
        case 1: return state->c;
        case 2: return state->d;
        case 3: return state->e;
        case 4: return state->h;
        case 5: return state->l;
        case 6: return Intel8080_LoadU8(image, (state->mar = ((state->h << 8)) | state->l));
        case 7: return state->a;
        default: return 0;
    }
}

static void Intel8080_WriteWReg(struct Intel8080_State *state, uint8_t *image, uint8_t reg, uint8_t val) {
    switch (reg) {
        case 0: state->b = val; break;
        case 1: state->c = val; break;
        case 2: state->d = val; break;
        case 3: state->e = val; break;
        case 4: state->h = val; break;
        case 5: state->l = val; break;
        case 6: Intel8080_StoreU8(image, (state->mar = ((state->h << 8) | state->l)), val); break;
        case 7: state->a = val;
    }
}

static uint16_t Intel8080_ReadDReg(struct Intel8080_State *state, uint8_t reg, bool sp) {
    switch (reg) {
        case 0: return ((state->b << 8) | state->c); 
        case 1: return ((state->d << 8) | state->e); 
        case 2: return ((state->h << 8) | state->l);
        case 3: return sp ? state->sp : 
            ((state->cy) |
            (1 << 1) |
            (state->p << 2) |
            (0 << 3) | (state->ac << 4) |
            (0 << 5) |
            (state->z << 6) |
            (state->s << 7)) |
            (state->a << 8);
        default: return 0;
    }
}

static void Intel8080_WriteDReg(struct Intel8080_State *state, uint8_t reg, uint16_t val, bool sp) {
    switch (reg) {
        case 0: state->b = (val >> 8); state->c = val; break;
        case 1: state->d = (val >> 8); state->e = val; break;
        case 2: state->h = (val >> 8); state->l = val; break;
        case 3: {
            if (!sp) {
                state->cy = val & 0x1; 
                state->p = (val >> 2) & 0x1; 
                state->ac = (val >> 4) & 0x1; 
                state->z = (val >> 6) & 0x1; 
                state->s = (val >> 7) & 0x1;
                state->a = (val >> 8);
            } else state->sp = val;
            break;
        }
    }
}

#define RWREG(reg) (Intel8080_ReadWReg(state, image, reg))
#define WWREG(reg, val) (Intel8080_WriteWReg(state, image, reg, val))
#define RDREG(reg, sp) (Intel8080_ReadDReg(state, reg, sp))
#define WDREG(reg, val, sp) (Intel8080_WriteDReg(state, reg, val, sp))

static void Intel8080_Set_SZP(struct Intel8080_State *state, uint8_t x) {
    state->s = x & 0x80;
    state->z = (x == 0);
    state->p = !__builtin_parity(x);
}

static uint8_t Intel8080_Add(struct Intel8080_State *state, uint8_t x, uint8_t y) {
    uint16_t result = x + y;
    Intel8080_Set_SZP(state, result);
    state->cy = result & 0x100;
    state->ac = ((x & 0x0f) + (x & 0x0f)) > 0x0f;
    return result;
}

static uint8_t Intel8080_Subtract(struct Intel8080_State *state, uint8_t x, uint8_t y) {
    uint16_t result = x + (y ^ 0xff) + 1;
    Intel8080_Set_SZP(state, result);
    state->cy = !(result & 0x100);
    state->ac = ((x & 0x0f) + (x & 0x0f)) > 0x0f;
    return result;
}

// TODO: remove //default:;
void Intel8080_Step(struct Intel8080_State *state, uint8_t *image) {
    uint32_t pc = state->pc;
    uint32_t ir;

    ir = Intel8080_LoadU8(image, pc);
    state->ir = ir; // cosmetic

    uint32_t mar = state->mar; // cosmetic

    uint8_t limm = Intel8080_LoadU8(image, pc + 1),
            himm = Intel8080_LoadU8(image, pc + 2);
    uint16_t imm = Intel8080_LoadU16(image, pc + 1);

    uint8_t wdest = (ir & 0b00111000) >> 3,
            wsrc = (ir & 0b00000111);

    uint8_t wdest_opcode = 0b11000111;
    uint8_t wsrc_opcode = 0b11000000;

    uint8_t tmp;
    uint16_t tmp2;

    switch (ir & wdest_opcode) {
        case 0b00000100: WWREG(wdest, RWREG(wdest)+1); Intel8080_Set_SZP(state, RWREG(wdest)); pc += 1; break; // INR
        case 0b00000101: WWREG(wdest, RWREG(wdest)-1); Intel8080_Set_SZP(state, RWREG(wdest)); pc += 1; break; // DCR
        case 0b00000110: WWREG(wdest, limm); pc += 2; break; // MVI
        //default:;
    }

    uint8_t alu_opcode = (ir >> 3) & 0x7;
    uint8_t s2 = RWREG(wsrc);
    
    if ((ir & 0b11000111) == 0b11000110) { // ALUI
        s2 = imm;
        pc += 1;
        goto alu;
    }
    switch (ir & wsrc_opcode) {
        case 0b01000000: WWREG(wdest, RWREG(wsrc)); pc += 1; break; // MOV
        alu:
        case 0b10000000: { // ALU
            switch (alu_opcode) {
                case 0: state->a = Intel8080_Add(state, state->a, s2); pc += 1; break; // ADD
                case 1: state->a = Intel8080_Add(state, state->a, s2 + state->cy); pc += 1; break; // ADC
                case 2: state->a = Intel8080_Subtract(state, state->a, s2); pc += 1; break; // SUB
                case 3: state->a = Intel8080_Subtract(state, state->a, s2 + state->cy); pc += 1; break; // SBB
                case 4: state->a &= s2; Intel8080_Set_SZP(state, state->a); state->cy = 0; state->ac = 0; pc += 1; break; // ANA
                case 5: state->a ^= s2; Intel8080_Set_SZP(state, state->a); state->cy = 0; state->ac = 0; pc += 1; break; // XRA
                case 6: state->a |= s2; Intel8080_Set_SZP(state, state->a); state->cy = 0; state->ac = 0; pc += 1; break; // ORA
                case 7: Intel8080_Subtract(state, state->a, s2); pc += 1; break; // CMP
            }
        }
    }
    
    uint8_t dsrc = (ir & 0b00110000) >> 4;

    uint8_t dsrc_opcode = 0b11001111;

    switch (ir & dsrc_opcode) {
        case 0b11000101: mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, RDREG(dsrc, false)); pc += 1; break; // PUSH
        case 0b11000001: WDREG(dsrc, Intel8080_LoadU16(image, state->sp), false); mar = (state->sp += 2); pc += 1; break; // POP
        case 0b00001001: { // DAD
            uint32_t result = RDREG(2, false) + RDREG(dsrc, true);
            state->cy = result & 0x10000;
            WDREG(2, result, false);
            pc += 1;
            break;
        }
        case 0b00000001: WDREG(dsrc, imm,  true); pc += 3; break; // LXI
        case 0b00000011: WDREG(dsrc, RDREG(dsrc, true) + 1, true); pc += 1; break; // INX
        case 0b00001011: WDREG(dsrc, RDREG(dsrc, true) - 1, true); pc += 1; break; // DCX
        //default:;
    }

    uint8_t rst_vector = 0b00111000;
    switch (ir & rst_vector) {
        case 0b11000111: (mar = state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc + 1); pc = ir & rst_vector; break; // RST
        //default:;
    }

    switch (ir) {
        case 0: pc += 1; break; // NOP
        case 0b00111111: state->cy = !state->cy; pc += 1; break; // CMC
        case 0b00110111: state->cy = 1; pc += 1; break; // STC
        case 0b00101111: state->a = ~(state->a); pc += 1; break; // CMA
        case 0b00100111: { // DAA
            uint8_t correction = 0;
            if ((state->a & 0x0f) > 9 || state->ac)
                correction |= 0x06;
            if ((state->a >> 4) > 9 || state->cy || (((state->a >> 4) >= 9) && (state->a & 0x0f) > 9)) {
                correction |= 0x60;
                state->cy = 1;
            }
            state->a = Intel8080_Add(state, state->a, correction);
            pc += 1;
            break;
        }
        case 0b00000010: Intel8080_StoreU8(image, (mar = (state->b << 8) | state->c), state->a); pc += 1; break; // STAX B
        case 0b00010010: Intel8080_StoreU8(image, (mar = (state->d << 8) | state->e), state->a); pc += 1; break; // STAX D
        case 0b00001010: state->a = Intel8080_LoadU8(image, (mar = (state->b << 8) | state->c)); pc += 1; break; // LDAX B
        case 0b00011010: state->a = Intel8080_LoadU8(image, (mar = (state->d << 8) | state->e)); pc += 1; break; // LDAX D
        case 0b00000111: state->cy = ((state->a & 0x80) >> 7); state->a = ((state->a << 1) | (state->a & 0x80) >> 7); pc += 1; break; // RLC
        case 0b00001111: state->cy = state->a & 0x1; state->a = ((state->a >> 1) | (state->a & 0x1) << 7); pc += 1; break; // RRC
        case 0b00010111: tmp = state->a; state->a = (state->a << 1 | state->cy); state->cy = ((tmp & 0x80) >> 7); pc += 1; break; // RAL
        case 0b00011111: tmp = state->a; state->a = (state->a >> 1 | state->cy << 7); state->cy = tmp & 0x1; pc += 1; break; // RAR
        case 0b11101011: tmp = state->h; state->h = state->d; state->d = tmp; tmp = state->l; state->l = state->e; state->e = tmp; pc += 1; break; // XCHG
        case 0b11111001: state->sp = ((state->h << 8) | state->l); pc += 1; break; // SPHL
        case 0b11100011: tmp2 = Intel8080_LoadU16(image, state->sp); Intel8080_StoreU16(image, state->sp, ((state->h << 8) | state->l)); state->h = tmp2 >> 8; state->l = tmp2; pc += 1; break; // XTHL
        case 0b00110010: Intel8080_StoreU8(image, (mar = imm), state->a); pc += 3; break; // STA
        case 0b00111010: state->a = Intel8080_LoadU8(image, (mar = imm)); pc += 3; break; // LDA
        case 0b00100010: Intel8080_StoreU16(image, (mar = imm), (state->h << 8) | state->l); pc += 3; break; // SHLD
        case 0b00101010: state->h = Intel8080_LoadU8(image, (mar = (imm+1))); state->l = Intel8080_LoadU8(image, (mar = imm)); pc += 3; break; // LHLD
        case 0b11101001: pc = ((state->h << 8) | state->l); break; // PCHL
        case 0b11000011: pc = imm; break; // JMP
        case 0b11011010: if (state->cy) pc = imm; else pc += 3; break; // JC
        case 0b11010010: if (!state->cy) pc = imm; else pc += 3; break; // JNC
        case 0b11001010: if (state->z) pc = imm; else pc += 3; break; // JZ
        case 0b11000010: if (!state->z) pc = imm; else pc += 3; break; // JNZ
        case 0b11111010: if (state->s) pc = imm; else pc += 3; break; // JM
        case 0b11110010: if (!state->s) pc = imm; else pc += 3; break; // JP
        case 0b11101010: if (state->p) pc = imm; else pc += 3; break; // JPE
        case 0b11100010: if (!state->p) pc = imm; else pc += 3; break; // JPO
        case 0b11001101: mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; break; // CALL
        case 0b11011100: if (state->cy) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CC
        case 0b11010100: if (!state->cy) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CNC
        case 0b11001100: if (state->z) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CZ
        case 0b11000100: if (!state->z) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CNZ
        case 0b11111100: if (state->s) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CM
        case 0b11110100: if (!state->s) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CP
        case 0b11101100: if (state->p) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CPE
        case 0b11100100: if (!state->p) { mar = (state->sp -= 2); Intel8080_StoreU16(image, state->sp, pc+3); pc = imm; } else pc += 3; break; // CPO
        case 0b11001001: pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; break; // RET
        case 0b11011000: if (state->cy) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RC
        case 0b11010000: if (!state->cy) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RNC
        case 0b11001000: if (state->z) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RZ
        case 0b11000000: if (!state->z) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RNZ
        case 0b11111000: if (state->s) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RM
        case 0b11110000: if (!state->s) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RP
        case 0b11101000: if (state->p) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RPE
        case 0b11100000: if (!state->p) { pc = Intel8080_LoadU16(image, state->sp); state->sp += 2; } else pc += 1; break; // RPO
        //default:;
    }

    state->mar = mar; // cosmetic
    state->pc = pc;
}