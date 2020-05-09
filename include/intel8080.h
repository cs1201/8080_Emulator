#ifndef INTEL8080_H
#define INTEL8080_H

#define I8080_ADDRESS_BUS_SIZE (16)
#define I8080_MAX_ADDRESS (0xFFFF)
#define I8080_MAX_MEMORY_SIZE (I8080_MAX_ADDRESS) 

#define I8080_OK (0)
#define I8080_ERROR (1)

enum{
    FLAG_C =  0x1,
    FLAG_AC = 0x2,
    FLAG_S =  0x4,
    FLAG_P =  0x8,
    FLAG_Z =  0x10,
    FLAG_ALL = 0xff
};

/* Bitfield struct to represent CPU condition bits */
typedef struct i8080_flags_t{
    uint8_t c:1;    //Carry
    uint8_t ac:1;   //Auxillary Carry
    uint8_t s:1;    //Sign
    uint8_t p:1;    //Parity
    uint8_t z:1;    //Zero
    uint8_t  :3;    //--- Padding to 8-bits ---
}i8080_flags_t;

/* High-level CPU state struct */
typedef struct i8080_state_t{
    uint8_t a; //Registers
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp; //Stack pointer
    uint16_t pc; //Program counter
    uint8_t *memory; //CPU memory (RAM)
    uint16_t loaded_rom_size;
    uint8_t int_enable; //Interrupt enable
    i8080_flags_t flags; //State/Condition flags
}i8080_state_t;

// /* ROM data */
// typedef struct i8080_rom_t{
//     char *filename;
//     int size;
//     unsigned char *data;
// }i8080_rom_t;

/* System Function Prototypes */ 
int load_rom(i8080_state_t *cpu, char *rom_filename);
int run_instruction(i8080_state_t *cpu);
void check_flags(i8080_state_t *cpu, uint16_t result, uint8_t mask);
void display_flags(i8080_state_t *cpu);
void clear_flags(i8080_state_t *cpu);

/* Generic CPU Instruction functions */
void inr(i8080_state_t *cpu, uint8_t *reg);
void inx(uint8_t *reg_hi, uint8_t *reg_lo);
void dcr(i8080_state_t *cpu, uint8_t *reg);
void dcx(uint8_t *reg_hi, uint8_t *reg_lo);
void mvi(uint8_t *reg, uint8_t value);
void ldax(i8080_state_t *cpu, uint8_t *reg, uint16_t addr);
void add(i8080_state_t *cpu, uint8_t *reg);
void adc(i8080_state_t *cpu, uint8_t *reg);
void sub(i8080_state_t *cpu, uint8_t *reg);
void sbb(i8080_state_t *cpu, uint8_t *reg);
void ana(i8080_state_t *cpu, uint8_t *reg);
void xra(i8080_state_t *cpu, uint8_t *reg);
void ora(i8080_state_t *cpu, uint8_t *reg);
void cmp(i8080_state_t *cpu, uint8_t *reg);
void ret(i8080_state_t *cpu);
void pop(i8080_state_t *cpu, uint8_t *reg_hi, uint8_t *reg_lo);
void jmp(i8080_state_t *cpu, uint16_t addr);
void call(i8080_state_t *cpu, uint16_t addr);
void push(i8080_state_t *cpu, uint8_t *reg_hi, uint8_t *reg_lo);

void not_implemented(uint8_t op);

/* Test Functions */
void test_inr(i8080_state_t *cpu);
void test_dcr(i8080_state_t *cpu);
void test_mvi(i8080_state_t *cpu);
void test_ldax(i8080_state_t *cpu);
// Todo: TEST DAD

#endif