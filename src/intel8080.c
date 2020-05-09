#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/intel8080.h"

#define MERGE_16BIT(h, l) ((h<<8 | l) & 0xffff)

int main(int argc, char **argv){
    //Initialise
    puts("Loading Intel8080 CPU Emulator...");
    i8080_state_t *cpu = malloc(sizeof(*cpu));
    // memset(cpu, 0, sizeof(i8080_state_t));

    // i8080_rom_t *rom = malloc(sizeof(i8080_rom_t));
    if(cpu == NULL){
        fprintf(stderr, "[ERROR]: Could not intialise CPU\n");
        return 1;
    }

    //Get ROM filename to load
    if(argc < 2){
        fprintf(stderr, "[ERROR]: No input file provided\n");
        return 1;
    }

    char *rom_filename = *(argv+1);
    printf("Loading ROM File: %s\n", rom_filename);

    // if(load_rom(cpu, rom_filename) != I8080_OK){
    //     fprintf(stderr, "[ERROR]: did not load ROM\n");
    //     return 1;
    // }

    cpu->pc = 0;
    cpu->flags.c = 0;
    cpu->flags.ac = 0;
    cpu->flags.z = 0;
    cpu->flags.s = 0;
    cpu->flags.p = 0;

    // //TEST RUN
    // while(cpu->pc < cpu->loaded_rom_size){
    //     run_instruction(cpu);
    // }
    // run_instruction(cpu);

    // test_inr(cpu);
    // test_dcr(cpu);
    // test_mvi(cpu);
    test_ldax(cpu);

    if (cpu)
        free(cpu);

    return 0;
}

void display_flags(i8080_state_t *cpu){
    printf("C:  %d\n", cpu->flags.c);
    printf("AC: %d\n", cpu->flags.ac);
    printf("S:  %d\n", cpu->flags.s);
    printf("P:  %d\n", cpu->flags.p);
    printf("Z:  %d\n", cpu->flags.z);
}

void check_flags(i8080_state_t *cpu, uint16_t result, uint8_t mask){
    if (mask & FLAG_Z)
        cpu->flags.z = ((result & 0xff) == 0); //Zero flag
    if (mask & FLAG_C)
        cpu->flags.c = (result > 0xff); //Carry flag
    if (mask & FLAG_AC)
        cpu->flags.ac = (result & (1<<3)) != 0; //Check state of 4th bit
    if (mask & FLAG_S) //Sign
        cpu->flags.s = ((result & 0x80) != 0); //Check HSB
    if (mask & FLAG_P){ //Parity: 1=even, 0=odd
        uint16_t x = result & 0xff;
        x ^= x >> 4;
        x ^= x >> 2;
        x ^= x >> 1;
        cpu->flags.p = ~(x & 1);
    }
}

void clear_flags(i8080_state_t *cpu){
    cpu->flags.c = 0;
    cpu->flags.ac = 0;
    cpu->flags.p = 0;
    cpu->flags.s = 0;
    cpu->flags.z = 0;
}

/* Increase value in register */
void inr(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = *reg + 1;
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_AC);
    *reg = result & 0xff;
}

/* Increase 16-bit regsiter pair */
void inx(uint8_t *reg_hi, uint8_t *reg_lo){
    uint16_t result = (((*reg_hi << 8) | *reg_lo) + 1);
    *reg_hi = result >> 8;
    *reg_lo = result & 0xff;  
}

/* Decrease value in register */
void dcr(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = *reg - 1;
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_AC);
    *reg = result & 0xff;
}

void dcx(uint8_t *reg_hi, uint8_t *reg_lo){
    uint16_t result = (((*reg_hi << 8) | *reg_lo) - 1);
    *reg_hi = result >> 8;
    *reg_lo = result & 0xff;
}

/* Move a given value into a register */
void mvi(uint8_t *reg, uint8_t value){
    *reg = value;
}

/* LDAX [reg], mem[addr] -- Load data from memory address into a register */
void ldax(i8080_state_t *cpu, uint8_t *reg, uint16_t addr){
    if(addr <= I8080_MAX_ADDRESS){
        *reg = cpu->memory[addr];
    }else{
        fprintf(stderr, "Address exceeds CPU memory\n");
    }
}

/* STAX [addr]. Store accumulator in address from register pair */
void stax(i8080_state_t *cpu, uint16_t addr){
    cpu->memory[addr] = cpu->a;
}

/* ADD [reg] - add value in register to current value in accumulator */
void add(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = cpu->a + *reg;
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_C|FLAG_AC);
    cpu->a = result & 0xff;
}

/* ADC [reg] - add register value to A + carry bit */
void adc(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = (cpu->a + *reg + cpu->flags.c);
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_C|FLAG_AC);
    cpu->a = result & 0xff;
}

/* SUB [reg] - subtract value in register to current value in accumulator */
void sub(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = cpu->a - *reg;
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_C|FLAG_AC);
    cpu->a = result & 0xff;
}

/* SBB [reg] - subtract register value to A - carry bit */
void sbb(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = (cpu->a - *reg - cpu->flags.c);
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_C|FLAG_AC);
    cpu->a = result & 0xff;
}

/* ANA [reg] - Logical AND of register and accumulator. Carry is reset to 0 */
void ana(i8080_state_t *cpu, uint8_t *reg){
    cpu->a = cpu->a & *reg;
    check_flags(cpu, (uint16_t)cpu->a, FLAG_S|FLAG_Z|FLAG_P|FLAG_C);
}

/* XRA [reg] - Logical XOR of register and accumulator. Carry is reset to 0 */
void xra(i8080_state_t *cpu, uint8_t *reg){
    cpu->a = cpu->a ^ *reg;
    check_flags(cpu, (uint16_t)cpu->a, FLAG_S|FLAG_Z|FLAG_P|FLAG_C|FLAG_AC); 
}

/* ORA [reg] - Logical OR of register and accumulator. Carry is reset to 0 */
void ora(i8080_state_t *cpu, uint8_t *reg){
    cpu->a = cpu->a | *reg;
    check_flags(cpu, (uint16_t)cpu->a, FLAG_S|FLAG_Z|FLAG_P|FLAG_C);
}

/* CMP [reg] - Set Z flag = 1 if [reg] == A, else set Z=0 */
void cmp(i8080_state_t *cpu, uint8_t *reg){
    uint16_t result = cpu->a - *reg;
    check_flags(cpu, result, FLAG_Z|FLAG_S|FLAG_P|FLAG_AC|FLAG_C);
    cpu->flags.z = (cpu->a == *reg) ? 1 : 0;
}

/* RET - Replace program-counter by value addressed by stack pointer */
void ret(i8080_state_t *cpu){
    // PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
    uint8_t d16_l = cpu->memory[cpu->sp];
    uint8_t d16_h = cpu->memory[cpu->sp+1];
    cpu->pc = MERGE_16BIT(d16_h, d16_l);
    cpu->sp += 2;
}

/* POP - Replace value in register pair with that help at data addressed by stack pointer */
void pop(i8080_state_t *cpu, uint8_t *reg_hi, uint8_t *reg_lo){
    *reg_hi = cpu->memory[cpu->sp+1];
    *reg_lo = cpu->memory[cpu->sp];
    cpu->sp += 2;
}

/* JMP - Program counter is moved to address given */
void jmp(i8080_state_t *cpu, uint16_t addr){
    cpu->pc = addr;
}

/* CALL - Push Return pos onto stack. Move PC to target address */
void call(i8080_state_t *cpu, uint16_t addr){
    uint16_t ret = cpu->pc + 2; //We want to return to just after this instruction
    cpu->memory[cpu->sp - 1] = ret & 0xff; // Push current position onto the stack
    cpu->memory[cpu->sp - 2] = (ret >> 8) & 0xff;
    cpu->sp -= 2; // Reset stack pointer to the address we just pushed
    cpu->pc = addr; // Move PC to target
}

/* PUSH - Push register pair data onto stack */
void push(i8080_state_t *cpu, uint8_t *reg_hi, uint8_t *reg_lo){
    cpu->memory[cpu->sp - 2] = *reg_lo;
    cpu->memory[cpu->sp - 1] = *reg_hi;
    cpu->sp -= 2;
}

void test_ldax(i8080_state_t *cpu){
    cpu->b = 0x00;
    cpu->memory[0xffff] = 0x34;
    ldax(cpu, &cpu->b, 0xffff);
    printf("Reg = %02X\n", cpu->b);
}

void test_mvi(i8080_state_t *cpu){
    uint8_t val = 0xf0;
    cpu->c = 0x00;
    mvi(&cpu->b, val);
    printf("Result: %02X\n", cpu->b);
}

void test_inr(i8080_state_t *cpu){
    cpu->c = 0xaa;
    // inr(cpu);
    printf("Result: c=0x%02x\n", cpu->c);
    display_flags(cpu);
}

void test_dcr(i8080_state_t *cpu){
    uint8_t val = 0x01;
    cpu->c = val;
    dcr(cpu, &cpu->c);
    printf("Orig: %02X, Result: %02X\n", val, cpu->c);
    display_flags(cpu);
}

int run_instruction(i8080_state_t *cpu){
    unsigned char *op = &cpu->memory[cpu->pc]; // Get op-code at program counter position
    unsigned char d16_l = cpu->memory[cpu->pc + 1];
    unsigned char d16_h = cpu->memory[cpu->pc + 2];
    cpu->pc++; //Increment Program Counter - some instructions will apply extra increments to PC

    //Parse for OP-Code
    switch(*op){
        case 0x00: break; //NOP
        case 0x01: //LXI BC,D16
            cpu->b = d16_h;
            cpu->c = d16_l;
            cpu->pc += 2;
            printf("0x%04X written to BC\n", (cpu->b<<8 | cpu->c));
            break;
        case 0x02: //STAX BC
            cpu->memory[(cpu->b <<8) | cpu->c] = cpu->a;
            break;
        case 0x03: //INX BC
            inx(&(cpu->b), &(cpu->c));
            break;
        case 0x04: //INR B
            inr(cpu, &(cpu->b));
            break;
        case 0x05: //DCR B
            dcr(cpu, &(cpu->b));
            break;
        case 0x06: //MVI B
            mvi(&(cpu->b), d16_l);
            cpu->pc++;
            break;
        case 0x07: //RLC - Rotate accumulator left
            cpu->flags.c = cpu->a & 0x40;
            cpu->a = ((cpu->a << 1) | (cpu->a >> 7)) & 0xff;
            break;
        case 0x08: break; //NOP
        case 0x09: //DAD BC (Add BC reg to HL reg)
            {
                uint16_t result = ((cpu->h << 8) | cpu->l) + ((cpu->b << 8) | cpu->c);
                check_flags(cpu, result, FLAG_C);
                cpu->h = ((result>>8) & 0xff);
                cpu->l = (result & 0xff);
            }
        case 0x0A: //LDAX BC (Load BC into A)
            ldax(cpu, &(cpu->a), ((cpu->b <<8) | cpu->c));
            break;
        case 0x0B: //DCX BC
            dcx(&(cpu->b), &(cpu->c));
            break;
        case 0x0C: //INR C
            inr(cpu, &(cpu->c));
            break;
        case 0x0D: //DCR C
            dcr(cpu, &(cpu->c));
            break;
        case 0x0E: //MVI C,D8 (Move 8-bit value into C)
            mvi(&(cpu->c), d16_l);
            cpu->pc++;
            break;
        case 0x0F: //RRC (Rotate accumulator right)
            cpu->flags.c = cpu->a & 0x1; //Carry bit = current A[0]
            cpu->a = ((cpu->a >> 1) | (cpu->a << 7)) & 0xff;
            break;
        case 0x10: //NOP
            break;
        case 0x11: //LXI D, D16 (Load value in DE)
            cpu->d = d16_h;
            cpu->e = d16_l;
            cpu->pc += 2;
            break;
        case 0x12: //STAX D (Load A into memory addressed by DE)
            stax(cpu, ((cpu->d << 8) | cpu->e));
            break;
        case 0x13: //INX DE
            inx(&(cpu->d), &(cpu->e));
        case 0x14: //INR D
            inr(cpu, &(cpu->d));
            break;
        case 0x15: //DCR D
            dcr(cpu, &(cpu->d));
            break;
        case 0x16: //MVI D,D8
            mvi(&(cpu->d), d16_l);
            cpu->pc++;
            break;
        case 0x17: // RAL (Rotate accumulator Left, through carry)
            {
                uint8_t prev_carry = cpu->flags.c;
                cpu->flags.c = cpu->a & 0x1; //CY = A[0]
                cpu->a = ((cpu->a >> 1) | (prev_carry << 7)); //Shift A left, A[7]=prev carry bit
                break;
            }
        case 0x18: //NOP
            break;
        case 0x19: //DAD D
            {
                uint16_t result = ((cpu->h << 8) | cpu->l) + ((cpu->d << 8) | cpu->e);
                check_flags(cpu, result, FLAG_C);
                cpu->h = ((result>>8) & 0xff);
                cpu->l = (result & 0xff);
            }
        case 0x1A: //LDAX D
            ldax(cpu, &(cpu->a), ((cpu->d <<8) | cpu->e));
            break;
        case 0x1B: //DCX D
            dcx(&(cpu->d), &(cpu->e));
            break;
        case 0x1C: //INR E
            inr(cpu, &(cpu->e));
            break;
        case 0x1D: //DCR E
            dcr(cpu, &(cpu->e));
            break;
        case 0x1E: //MVI E, D8
            mvi((&cpu->e), d16_l);
            cpu->pc++;
            break;
        case 0x1F: //RAR (Rotate A right through carry)
            not_implemented(*op);
            break; //TO IMPLEMENT
        case 0x20: //NOP
            break;
        case 0x21: //LXI H,D16
            cpu->h = d16_h;
            cpu->l = d16_l;
            cpu->pc += 2;
            break;
        case 0x22: //SHLD addr
            {
                uint16_t addr = ((d16_h << 8) | d16_l);
                cpu->memory[addr] = cpu->l;
                cpu->memory[addr+1] = cpu->h;
                cpu->pc += 2;
                break;
            }
        case 0x23: //INX H
            inx(&(cpu->h), &(cpu->l));
            break;
        case 0x24: // INR H
            inr(cpu, &(cpu->h));
            break;
        case 0x25: // DCR H
            dcr(cpu, &(cpu->h));
            break;
        case 0x26: // MVI H,D8
            mvi(&(cpu->h), d16_l);
            cpu->pc++;
            break;
        case 0x27: // DAA
            not_implemented(*op);
            break;
        case 0x28: // NOP
            break;
        case 0x29: // DAD H (HL *= 2)
            {
                uint16_t result = (((cpu->h << 8) | cpu->l) << 1);
                check_flags(cpu, result, FLAG_C);
                cpu->h = result >> 8;
                cpu->l = result & 0xff;
                break;
            }
        case 0x2A: // LHLD addr
            {
                uint16_t addr = MERGE_16BIT(d16_h, d16_l);
                cpu->l = cpu->memory[addr];
                cpu->h = cpu->memory[addr+1];
                cpu->pc += 2;
                break;
            }
        case 0x2B: // DCH HL
            dcx(&(cpu->h), &(cpu->l));
            break;
        case 0x2C: //INR L
            inr(cpu, &(cpu->l));
            break;
        case 0x2D: // DCR L
            dcr(cpu, &(cpu->l));
            break;
        case 0x2E: // MVI L, D8
            mvi(&(cpu->l), d16_l);
            cpu->pc++;
            break;
        case 0x2F: // CMA (A = !A)
            cpu->a = ~(cpu->a) & 0xff;
            break;
        case 0x30: // NOP
            break;
        case 0x31: //LXI SP,D16 (update stack pointer)
            cpu->sp = MERGE_16BIT(d16_h, d16_l);
            cpu->pc += 2;
            break;
        case 0x32: //STA addr
            cpu->memory[MERGE_16BIT(d16_h, d16_l)] = cpu->a;
            cpu->pc += 2;
            break;
        case 0x33: // INX SP
            cpu->sp += 1;
            break; 
        case 0x34: // INR M (Increment data at memory addressed by HL)
            inr(cpu, &cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]);
            break;
        case 0x35: // DCR M (Deccrement data at memory addressed by HL)
            dcr(cpu, &cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]);
            break;
        case 0x36: // MVI M,D8 (Move val into memory addresse dy HL)
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = d16_l;
            cpu->pc++;
            break;
        case 0x37: // STC
            cpu->c = 0x1;
            break;
        case 0x38: //NOP
            break;
        case 0x39: //DAD SP (Add stackpointer to HL)
            {
                uint16_t result = MERGE_16BIT(cpu->h, cpu->l) + cpu->sp;
                check_flags(cpu, result, FLAG_C);
                cpu->h = result >> 8;
                cpu->l = result & 0xff;
                break;
            }
        case 0x3A: // LDA addr
            cpu->a = cpu->memory[MERGE_16BIT(d16_h, d16_l)];
            cpu->pc += 2;
            break;
        case 0x3B: // DCX SP
            cpu->sp -= 1;
            break;
        case 0x3C: // INR A
            inr(cpu, &(cpu->a));
            break;
        case 0x3D: // DCR A
            dcr(cpu, &(cpu->a));
            break;
        case 0x3E: // MVI A,D8
            mvi(&(cpu->a), d16_l);
            cpu->pc++;
            break;
        case 0x3F: // CMC
            cpu->c = !(cpu->c);
        case 0x40: // MOV B,B
            break;
        case 0x41: // MOV B,C
            cpu->b = cpu->c;
            break;
        case 0x42: // MOV B, D
            cpu->b = cpu->d;
            break;
        case 0x43: // MOV B,E
            cpu->b = cpu->e;
            break;
        case 0x44: // MOV B,H
            cpu->b = cpu->h;
            break;
        case 0x45: // MOV B,L
            cpu->b = cpu->l;
        case 0x46: // MOV B,M
            cpu->b = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x47: // MOV B,A
            cpu->b = cpu->a;
            break;
        case 0x48: // MOV C,B
            cpu->c = cpu->b;
            break;
        case 0x49: // MOV C,C
            break;
        case 0x4A: // MOV C,D
            cpu->c = cpu->d;
            break;
        case 0x4B: // MOV C,E
            cpu->c = cpu->e;
            break;
        case 0x4C: // MOV C,H
            cpu->c = cpu->h;
            break;
        case 0x4D: // MOV C,L
            cpu->c = cpu->l;
            break;
        case 0x4E: // MOV C,M
            cpu->c = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x4F: // MOV C,A
            cpu->c = cpu->a;
            break;
        case 0x50: // MOV D,B
            cpu->d = cpu->b;
            break;
        case 0x51: // MOV D,C
            cpu->d = cpu->c;
            break;
        case 0x52: // MOV D, D
            break;
        case 0x53: // MOV D,E
            cpu->d = cpu->e;
            break;
        case 0x54: // MOV D,H
            cpu->d = cpu->h;
            break;
        case 0x55: // MOV D,L
            cpu->d = cpu->l;
            break;
        case 0x56: // MOV D,M
            cpu->d = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x57: // MOV D,A
            cpu->d = cpu->a;
            break;
        case 0x58: // MOV E,B
            cpu->e = cpu->b;
            break;
        case 0x59: // MOV E,C
            cpu->e = cpu->c;
            break;
        case 0x5A: // MOV E,D
            cpu->e = cpu->d;
            break;
        case 0x5B: // MOV E,E
            break;
        case 0x5C: // MOV E,H
            cpu->e = cpu->h;
            break;
        case 0x5D: // MOV E,L
            cpu->e = cpu->l;
        case 0x5E: // MOV E,M
            cpu->e = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x5F: // MOV E,A
            cpu->e = cpu->a;
            break;
        case 0x60: // MOV H,B
            cpu->h = cpu->b;
            break;
        case 0x61: // MOV H,C
            cpu->h = cpu->c;
            break;
        case 0x62: // MOV H, D
            cpu->h = cpu->d;
            break;
        case 0x63: // MOV H,E
            cpu->h = cpu->e;
            break;
        case 0x64: // MOV H,H
            break;
        case 0x65: // MOV H,L
            cpu->h = cpu->l;
            break;
        case 0x66: // MOV H,M
            cpu->h = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x67: // MOV H,A
            cpu->h = cpu->a;
            break;
        case 0x68: // MOV L,B
            cpu->l = cpu->b;
        case 0x69: // MOV L,C
            cpu->l = cpu->c;
            break;
        case 0x6A: // MOV L,D
            cpu->l = cpu->d;
            break;
        case 0x6B: // MOV L,E
            cpu->l = cpu->e;
            break;
        case 0x6C: // MOV L,H
            cpu->l = cpu->h;
            break;
        case 0x6D: // MOV L,L
            break;
        case 0x6E: // MOV L,M
            cpu->l = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x6F: // MOV L,A
            cpu->l = cpu->a;
            break;
        case 0x70: // MOV M,B
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->b;
            break;
        case 0x71: // MOV M,C
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->c;
            break;
        case 0x72: // MOV M,D
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->d;
            break;
        case 0x73: // MOV M,E
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->e;
            break;
        case 0x74: // MOV M,H
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->h;
            break;
        case 0x75: // MOV M,L
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->l;
            break;
        case 0x76: // HLT (HALT - increment pc and wait for interrupt)
            not_implemented(*op);
            break;
        case 0x77: // MOV M,A
            cpu->memory[MERGE_16BIT(cpu->h, cpu->l)] = cpu->a;
            break;
        case 0x78: // MOV A,B
            cpu->a = cpu->b;
        case 0x79: // MOV A,C
            cpu->a = cpu->c;
            break;
        case 0x7A: // MOV A,D
            cpu->a = cpu->d;
            break;
        case 0x7B: // MOV A,E
            cpu->a = cpu->e;
            break;
        case 0x7C: // MOV A,H
            cpu->a = cpu->h;
            break;
        case 0x7D: // MOV A,L
            cpu->a = cpu->l;
            break;
        case 0x7E: // MOV A,M
            cpu->a = cpu->memory[MERGE_16BIT(cpu->h, cpu->l)];
            break;
        case 0x7F: // MOV A,A
            break;
        case 0x80: // ADD B
            add(cpu, &(cpu->b));
            break;
        case 0x81: // ADD C
            add(cpu, &(cpu->c));
            break;
        case 0x82: // ADD D
            add(cpu, &(cpu->d));
            break;
        case 0x83: // ADD E
            add(cpu, &(cpu->e));
            break;
        case 0x84: // ADD H
            add(cpu, &(cpu->h));
            break;
        case 0x85: // ADD L
            add(cpu, &(cpu->l));
            break;
        case 0x86: // ADD M
            // not_implemented(*op);
            // // cpu->a = (cpu->a + cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]);
            add(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0x87: // ADD A
            add(cpu, &(cpu->a));
            break;
        case 0x88: // ADC B
            adc(cpu, &(cpu->b));
            break;
        case 0x89: // ADC C
            adc(cpu, &(cpu->c));
            break;
        case 0x8A: // ADC D
            adc(cpu, &(cpu->d));
            break;
        case 0x8B: // ADC E
            adc(cpu, &(cpu->e));
            break;
        case 0x8C: // ADC H
            adc(cpu, &(cpu->h));
            break;
        case 0x8D: // ADC L
            adc(cpu, &(cpu->l));
            break;
        case 0x8E: // ADC M
            adc(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0x8F: // ADC A
            adc(cpu, &(cpu->a));
            break;
        case 0x90: // SUB B
            sub(cpu, &(cpu->b));
            break;
        case 0x91: // SUB C
            sub(cpu, &(cpu->c));
            break;
        case 0x92: // SUB D
            sub(cpu, &(cpu->d));
            break;
        case 0x93: // SUB E
            sub(cpu, &(cpu->e));
            break;
        case 0x94: //SUB H
            sub(cpu, &(cpu->h));
            break;
        case 0x95: // SUB L
            sub(cpu, &(cpu->l));
            break;
        case 0x96: // SUB M
            sub(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0x97: // SUB A
            sub(cpu, &(cpu->a));
            break;
        case 0x98: // SBB B
            sbb(cpu, &(cpu->b));
            break;
        case 0x99: // SBB C
            sbb(cpu, &(cpu->c));
            break;
        case 0x9A: // SBB D
            sbb(cpu, &(cpu->d));
            break;
        case 0x9B: // SBB E
            sbb(cpu, &(cpu->e));
            break;
        case 0x9C: // SBB H
            sbb(cpu, &(cpu->h));
            break;
        case 0x9D: // SBB L
            sbb(cpu, &(cpu->l));
            break;
        case 0x9E: // SBB M
            sbb(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0x9F: // SBB A
            sbb(cpu, &(cpu->a));
            break;
        case 0xA0: // ANA B
            ana(cpu, &(cpu->b));
            break;
        case 0xA1: // ANA C
            ana(cpu, &(cpu->c));
            break;
        case 0xA2: // ANA D
            ana(cpu, &(cpu->d));
            break;
        case 0xA3: // ANA E
            ana(cpu, &(cpu->e));
            break;
        case 0xA4: // ANA H
            ana(cpu, &(cpu->h));
            break;
        case 0xA5: // ANA L
            ana(cpu, &(cpu->l));
            break;
        case 0xA6: // ANA M
            ana(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0xA7: // ANA A
            cpu->flags.c = 0;
            break;
        case 0xA8: // XRA B
            xra(cpu, &(cpu->b));
            break;
        case 0xA9: // XRA C
            xra(cpu, &(cpu->c));
            break;
        case 0xAA: // XRA D
            xra(cpu, &(cpu->d));
            break;
        case 0xAB: // XRA E
            xra(cpu, &(cpu->e));
            break;
        case 0xAC: // XRA H
            xra(cpu, &(cpu->h));
            break;
        case 0xAD: // XRA L
            xra(cpu, &(cpu->l));
            break;
        case 0xAE: // XRA M
            xra(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0xAF: // XRA A
            cpu->flags.c = 0;
            break;
        case 0xB0: // ORA B
            ora(cpu, &(cpu->b));
            break;
        case 0xB1: // ORA C
            ora(cpu, &(cpu->c));
            break;
        case 0xB2: // ORA D
            ora(cpu, &(cpu->d));
            break;
        case 0xB3: // ORA E
            ora(cpu, &(cpu->e));
            break;
        case 0xB4: // ORA H
            ora(cpu, &(cpu->h));
            break;
        case 0xB5: // ORA L
            ora(cpu, &(cpu->l));
            break;
        case 0xB6: // ORA M
            ora(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0xB7: // ORA A
            cpu->flags.c = 0;
            break;
        case 0xB8: // CMP B
            cmp(cpu, &(cpu->b));
            break;
        case 0xB9: // CMP C
            cmp(cpu, &(cpu->c));
            break;
        case 0xBA: // CMP D
            cmp(cpu, &(cpu->d));
            break;
        case 0xBB: // CMP E
            cmp(cpu, &(cpu->e));
            break;
        case 0xBC: // CMP H
            cmp(cpu, &(cpu->h));
            break;
        case 0xBD: // CMP L
            cmp(cpu, &(cpu->l));
            break;
        case 0xBE: // CMP M
            cmp(cpu, &(cpu->memory[MERGE_16BIT(cpu->h, cpu->l)]));
            break;
        case 0xBF: // CMP A
            cmp(cpu, &(cpu->a));
            break;
        case 0xC0: // RNZ
            if(!cpu->flags.z)
                ret(cpu);
            break;
        case 0xC1: // POP BC
            pop(cpu, &(cpu->b), &(cpu->c));
            break;
        case 0xC2: // JNZ
            if(!cpu->flags.z){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xC3: // JMP
            jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            cpu->pc += 2;
            break;
        case 0xC4: // CNZ
            if(!cpu->flags.z){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xC5: // PUSH BC
            push(cpu, &(cpu->b), &(cpu->c));
            break;
        case 0xC6: // ADI D8
            {
                uint16_t result = cpu->a + d16_l;
                check_flags(cpu, result, FLAG_ALL);
                cpu->a = result & 0xff;
                cpu->pc++;
                break;
            }
        case 0xC7: // RST 0
            not_implemented(*op);
            break;
        case 0xC8: // RZ
            if(cpu->flags.z)
                ret(cpu);
            break;
        case 0xC9: // RET
            ret(cpu);
            break;
        case 0xCA: // JZ adr
            if(cpu->flags.z){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xCB: // NOP
            break;
        case 0xCC: // CZ addr
            if(cpu->flags.c){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xCD: // CALL addr
            call(cpu, MERGE_16BIT(d16_h, d16_l));
            break;
        case 0xCE: // ACI D8
            {
                uint16_t result = cpu->a + d16_l + cpu->flags.c;
                check_flags(cpu, result, FLAG_ALL);
                cpu->a = result & 0xff;
                cpu->pc++;
                break;
            }
        case 0xCF: // RST 1
            not_implemented(*op);
            break;
        case 0xD0: // RNC
            if(!cpu->flags.c)
                ret(cpu);
            break;
        case 0xD1: // POP DE
            pop(cpu, &(cpu->d), &(cpu->e));
            break;
        case 0xD2: // JNC adr
            if(!cpu->flags.c){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xD3: // OUT D8
            not_implemented(*op);
            break;
        case 0xD4: // CNC addr
            if(!cpu->flags.c){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xD5: // PUSH DE
            push(cpu, &(cpu->d), &(cpu->e));
            break;
        case 0xD6: // SUI D8
            {
                uint16_t result = cpu->a - d16_l;
                check_flags(cpu, result, FLAG_ALL);
                cpu->a = result & 0xff;
                cpu->pc++;
                break;
            }
        case 0xD7: // RST 2
            not_implemented(*op);
            break;
        case 0xD8: // RC
            if(cpu->flags.c)
                ret(cpu);
            break;
        case 0xD9: // NOP
            break;
        case 0xDA: // JC addr
            if(cpu->flags.c){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xDB: // IN D8
            not_implemented(*op)
            cpu->pc++;
            break;
        case 0xDC: // CC addr
            if(cpu->flags.c){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xDD: // NOP
            break;
        case 0xDE: // SBI D8
            {
                uint16_t result = cpu->a - d16_l - cpu->flags.c;
                check_flags(cpu, result, FLAG_ALL);
                cpu->a = result & 0xff;
                cpu->pc++;
                break;
            }
            break;
        case 0xDF: // RST 3
            not_implemented(*op);
            break;
        case 0xE0: // RPO
            if(!cpu->flags.p)
                ret(cpu);
            break;
        case 0xE1: // POP HL
            pop(cpu, &(cpu->h), &(cpu->l));
            break;
        case 0xE2: // JPO addr
            if(!cpu->flags.p){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xE3: // XTHL
            {
                uint8_t prev_h = cpu->h;
                uint8_t prev_l = cpu->l;
                cpu->l = cpu->memory[cpu->sp];
                cpu->h = cpu->memory[cpu->sp + 1];
                cpu->memory[cpu->sp] = prev_l;
                cpu->memory[cpu->sp + 1] = prev_h;
            }
            break;
        case 0xE4: // CPO addr
            if(!cpu->flags.p){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc +=2;
            }
            break;
        case 0xE5: // PUSH H
            push(cpu, &(cpu->h), &(cpu->l));
            break;
        case 0xE6: // ANI D8
            {
                uint16_t result = cpu->a & d16_l;
                check_flags(cpu, result, FLAG_ALL);
                cpu->a = result & 0xff;
                cpu->pc++;
                break;
            }
            break;
        case 0xE7: // RST 4
            not_implemented(*op);
            break;
        case 0xE8: // RPE
            if(cpu->flags.p)
                ret(cpu);
            break;
        case 0xE9: // PCHL
            cpu->pc = MERGE_16BIT(d16_h, d16_l);
            break;
        case 0xEA: // JPE addr
            if(cpu->flags.p){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xEB: // XCHG
            {
                uint8_t prev_d = cpu->d;
                uint8_t prev_e = cpu->e;
                cpu->d = cpu->memory[cpu->sp];
                cpu->e = cpu->memory[cpu->sp + 1];
                cpu->memory[cpu->sp] = prev_d;
                cpu->memory[cpu->sp + 1] = prev_e;
            }
            break;
        case 0xEC: // CPE addr
            if(cpu->flags.p){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xED: // NOP
            break;
        case 0xEE: // XRI D8
            {
                uint16_t result = cpu->a ^ d16_l;
                check_flags(cpu, result, FLAG_ALL);
                cpu->a = result & 0xff;
                cpu->pc++;
                break;
            }
            break;
        case 0xEF: // RST 4
            not_implemented(*op);
            break;
        case 0xF0: // RPE
            if(cpu->flags.p)
                ret(cpu);
            break;
        case 0xF1: // POP PSW
            not_implemented(*op);
            break;
        case 0xF2: // JP addr
            if(cpu->flags.p){
                jmp(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xF3: // DI
            not_implemented(*op);
            break;
        case 0xF4: // CP addr
            if(cpu->flags.p){
                call(cpu, MERGE_16BIT(d16_h, d16_l));
            }else{
                cpu->pc += 2;
            }
            break;
        case 0xF5: // PUSH PSW
            not_implemented(*op);
            break;
        case 0xF6:
            break;
        case 0xF7:
            break;
        case 0xF8:
            break;
        case 0xF9:
            break;
        case 0xFA:
            break;
        case 0xFB:
            break;
        case 0xFC:
            break;
        case 0xFD:
            break;
        case 0xFE:
            break;
        case 0xFF:
            break;
        default: 
            break;
    }

    return I8080_OK;
}

int load_rom(i8080_state_t *cpu, char *rom_filename){
    //Open file
    FILE *rom_file;
    int rom_size;

    if((rom_file = fopen(rom_filename, "rb")) == NULL){
        return I8080_ERROR;
    }

    //Get file size
    fseek(rom_file, 0, SEEK_END);
    rom_size = ftell(rom_file);
    rewind(rom_file);
    
    //Load ROM into CPU memory
    if(rom_size > I8080_MAX_MEMORY_SIZE){
        return I8080_ERROR;
    }
    printf("ROM Size: %d\n", rom_size);
    cpu->loaded_rom_size = rom_size;
    int read_bytes = fread(cpu->memory, sizeof(uint8_t), rom_size, rom_file);
    if( read_bytes != rom_size){
        return I8080_ERROR;
    }

    fclose(rom_file);
    return I8080_OK;
}

void not_implemented(uint8_t op){
    fprintf(stderr, "OpCode: %02X not implemented\n", op);
}