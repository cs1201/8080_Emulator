#ifndef INTEL8080_H
#define INTEL8080_H

#define I8080_ADDRESS_BUS_SIZE (16)
#define I8080_MAX_ADDRESS (65536)
#define I8080_MAX_MEMORY_SIZE (I8080_MAX_ADDRESS) 

#define I8080_OK (0)
#define I8080_ERROR (1)

enum{
    FLAG_C = 1;
    FLAG_AC = 2;
    FLAG_S = 4;
    FLAG_P = 8;
    FLAG_Z = 16;
}

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

// System Function Prototypes
int load_rom(i8080_state_t *cpu, char *rom_filename);
int run_instruction(i8080_state_t *cpu);

#endif