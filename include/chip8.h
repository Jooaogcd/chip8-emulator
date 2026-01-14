#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stddef.h>

#include "config.h"
#include "memory.h"
#include "stack.h"
#include "registers.h"
#include "keyboard.h"
#include "screen.h"

struct Chip8 {
    struct Memory memory;
    struct Stack stack;
    struct Registers registers;
    struct Keyboard keyboard;
    struct Screen screen;
};

void chip8_init(struct Chip8* chip8);

void chip8_load_program(struct Chip8* chip8, const char* buffer, size_t size);

void chip8_execute_instruction(struct Chip8* chip8, unsigned short opcode);

static void chip8_execute_complex_instruction(struct Chip8* chip8, unsigned short opcode);

static void chip8_execute_eight_prefix_instruction(struct Chip8* chip8, unsigned short opcode);

static void chip8_execute_E_prefix_instruction(struct Chip8* chip8, unsigned short opcode);

static void chip8_execute_F_prefix_instruction(struct Chip8* chip8, unsigned short opcode);

#endif