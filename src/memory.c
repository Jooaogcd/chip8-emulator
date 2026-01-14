#include <assert.h>
#include "memory.h"

static void check_memory_bounds(int index) {
    assert(index >= 0 && index < TOTAL_MEMORY_SIZE);
}

unsigned char memory_get_byte(struct Memory* memory, int index) {
    check_memory_bounds(index);
    return memory->memory_array[index];
}

void memory_set_byte(struct Memory* memory, int index, unsigned char value) {
    check_memory_bounds(index);
    memory->memory_array[index] = value;
}

unsigned short memory_get_short(struct Memory* memory, int index) {
    unsigned char first_byte = memory_get_byte(memory, index);
    unsigned char second_byte = memory_get_byte(memory, index + 1);

    return first_byte << 8 | second_byte; // merge the two bytes and return as a short
}