#Intel 8080 - Emulator
Intel 8080 Emulator project written in C. 
Using this tutorial resource: http://emulator101.com

##8080 CPU Background

https://en.wikipedia.org/wiki/Intel_8080

- Released by Intel in 1974 as successor to the 8008. It largely shares the same instruction set as the 8008, but with an expanded 16-bit address bus (allowing access to up to 64kb of memory) and initally a 2MHz clock speed.

    #### Registers
    - The 8080 has 7 8-bit registers: **A,B,C,D,E,H and L**.
    - **A** is the primary 8-bit accumulator, and the remanining registers can be used as individual 8-bit registers or as three 16-bit pairs (**BC, DE, HL**)
    - Some instructions use **HL** as a 16-bit accumulator
    - A psuedo-register **M** can be used for most funcitons
    - There is a 16-bit stack pointer to memory and 16-bit program counter

    #### Flags
    - The 8080 maintains internal flag bits (state register) .
    - **S**: Sign flag - set if result is negative
    - **Z**: Zero flag - set if result is zero
    - **P**: Parity flag - set if number of 1 bits in result is even
    - **C**: Carry flag - set if the last addition operation resulted in carry, or subtraction resulted in borrow
    - **AC/H**: Auxillary carry - used for binary-coded decimal arithmetic

    #### Instructions
    - All instructions are encoded in 1 byte (including register numbers)
    - ##### 8-bit Instructions:
        - Most 8-bit operations can be performed in the A regsiter/accumulator.
        - For 2-operand operations, the other operand can be an immediate value, another register or an 8-bit memory location addressed by **HL**
