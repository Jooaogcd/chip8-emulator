#include <assert.h>
#include "stack.h"
#include "chip8.h"

static void check_stack_bounds(struct Chip8* chip8) {
    assert(chip8->registers.SP < sizeof(chip8->stack.stack_array));
}

void stack_push(struct Chip8* chip8, unsigned short value) {
    chip8->registers.SP += 1;
    check_stack_bounds(chip8);
    chip8->stack.stack_array[chip8->registers.SP] = value;
}

unsigned short stack_pop(struct Chip8* chip8) {
    check_stack_bounds(chip8);
    unsigned short value = chip8->stack.stack_array[chip8->registers.SP];
    chip8->registers.SP -= 1;
    return value;
}