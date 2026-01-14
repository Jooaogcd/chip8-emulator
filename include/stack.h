#ifndef _STACK_H_
#define _STACK_H_

#include "config.h"

struct Stack {
    unsigned short stack_array[STACK_SIZE];
};

struct Chip8;

void stack_push(struct Chip8* chip8, unsigned short value);

unsigned short stack_pop(struct Chip8* chip8);

#endif