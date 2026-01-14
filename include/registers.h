#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include "config.h"

struct Registers {
    unsigned char   V[NUMBER_OF_REGISTERS]; // sixteen general purpose 8-bit registers 
    unsigned short  I;                      // 16-bit register (store memory addresses)
    unsigned char   DT;                     // delay timer register
    unsigned char   ST;                     // sound timer register
    unsigned short  PC;                     // 16-bit program counter register
    unsigned char   SP;                     // 8-bit stack pointer register
};



#endif