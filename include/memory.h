#ifndef _CHIP8_MEMORY_H_
#define _CHIP8_MEMORY_H_

#include "config.h"

struct Memory {
    unsigned char memory_array[TOTAL_MEMORY_SIZE];
};

unsigned char memory_get_byte(struct Memory* memory, int index);

void memory_set_byte(struct Memory* memory, int index, unsigned char value);

unsigned short memory_get_short(struct Memory* memory, int index);

#endif