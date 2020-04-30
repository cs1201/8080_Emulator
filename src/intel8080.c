#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/intel8080.h"

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

    if(load_rom(cpu, rom_filename) != I8080_OK){
        fprintf(stderr, "[ERROR]: did not load ROM\n");
        return 1;
    }

    cpu->pc = 0;

    //TEST RUN
    while(cpu->pc < cpu->loaded_rom_size){
        run_instruction(cpu);
    }
    run_instruction(cpu);


    if (cpu)
        free(cpu);

    return 0;
}

void check_flags(i8080_state_t *cpu, uint16_t result, uint8_t flag_mask){
    if (flag_mask & FLAG_Z)
        cpu->flags.z = ((answer & 0xff) == 0); //Zero flag
    if (flag & FLAG_C)
        cpu->flags.c = (answer >> 0xff);
    if (flag & FLAG_S)
        cpu->flags.s = ((answer & 0x80) != 0);
}

int run_instruction(i8080_state_t *cpu){
    unsigned char *op = &cpu->memory[cpu->pc]; // Get op-code at program counter position
    //Pre-fetch high and low operands in case we need them
    unsigned char d16_l = cpu->memory[cpu->pc + 1];
    unsigned char d16_h = cpu->memory[cpu->pc + 2];
    uint16_t sum = 0;
    switch(*op){
        case 0x00: break; //NOP
        case 0x01: //LXI BC,D16
            cpu->b = d16_h;
            cpu->c = d16_l;
            cpu->pc += 3;
            printf("0x%04X written to BC\n", (cpu->b<<8 | cpu->c));
            break;
        case 0x02: //STAX BC
            cpu->memory[(cpu->b <<8) | cpu->c] = cpu->a;
            cpu->pc++;
            break;
        case 0x03: //INX BC
            sum = ( (cpu->b << 8) | cpu->c ) + 1;
            cpu->b = sum >> 8;
            cpu->c = sum & 0xffff;
            printf("Increment BC by 1 to 0x%04X\n", sum);
            cpu->pc++;
            break;
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF:
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7:
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
        default: cpu->pc++; break;
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

    if(fread(cpu->memory, sizeof(uint8_t), rom_size, rom_file) != rom_size){
        return I8080_ERROR;
    }

    fclose(rom_file);
    return I8080_OK;
}