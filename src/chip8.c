#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"
#include "SDL2/SDL.h"

const char char_set[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9 
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
};

void chip8_init(struct Chip8* chip8) {
    memset(chip8, 0, sizeof(struct Chip8));
    memcpy(&chip8->memory.memory_array, char_set, sizeof(char_set));
}

void chip8_load_program(struct Chip8* chip8, const char* buffer, size_t size) {
    assert((size + PROGRAM_LOAD_ADRESS) < TOTAL_MEMORY_SIZE);
    memcpy(&chip8->memory.memory_array[PROGRAM_LOAD_ADRESS], buffer, size);
    chip8->registers.PC = PROGRAM_LOAD_ADRESS;
}

static void chip8_execute_eight_prefix_instruction(struct Chip8* chip8, unsigned short opcode) {
    unsigned char n = opcode & 0x000F;          // n or nibble - A 4-bit value, the lowest 4 bits of the instruction 
    unsigned char x = (opcode & 0x0F00) >> 8;   // x - A 4-bit value, the lower 4 bits of the high byte of the instruction 
    unsigned char y = (opcode & 0x00F0) >> 4;   // y - A 4-bit value, the upper 4 bits of the low byte of the instruction 

    switch (n) {
        // 8xy0 - LD Vx, Vy: Set Vx = Vy.
        case 0x0:
            chip8->registers.V[x] = chip8->registers.V[y];
        break;

        // 8xy1 - OR Vx, Vy: Set Vx = Vx OR Vy.
        case 0x1:
            chip8->registers.V[x] |= chip8->registers.V[y];
        break;

        // 8xy2 - AND Vx, Vy: Set Vx = Vx AND Vy.
        case 0x2:
            chip8->registers.V[x] &= chip8->registers.V[y];
        break;

        // 8xy3 - XOR Vx, Vy: Set Vx = Vx XOR Vy.
        case 0x3:
            chip8->registers.V[x] ^= chip8->registers.V[y];
        break;

        // 8xy4 - ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
        case 0x4:
            chip8->registers.V[0xF] = (chip8->registers.V[x] + chip8->registers.V[y] > 0xFF) ? 1 : 0;
            chip8->registers.V[x] += chip8->registers.V[y];
        break;

        // 8xy5 - SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
        case 0x5:
            chip8->registers.V[0xF] = (chip8->registers.V[x] > chip8->registers.V[y]) ? 1 : 0;
            chip8->registers.V[x] -= chip8->registers.V[y];
        break;

        // 8xy6 - SHR Vx {, Vy}: Set Vx = Vx SHR 1.
        case 0x6:
            chip8->registers.V[0xF] = chip8->registers.V[x] & 0x1;
            chip8->registers.V[x] >>= 1;
        break;

        // 8xy7 - SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
        case 0x7:
            chip8->registers.V[0xF] = (chip8->registers.V[y] > chip8->registers.V[x]) ? 1 : 0;
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
        break;

        // 8xyE - SHL Vx {, Vy}: Set Vx = Vx SHL 1.
        case 0xE:
            chip8->registers.V[0xF] = (chip8->registers.V[x] & 0x80) >> 7;
            chip8->registers.V[x] <<= 1;
        break;
    }
}

static void chip8_execute_E_prefix_instruction(struct Chip8* chip8, unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00) >> 8;   // x - A 4-bit value, the lower 4 bits of the high byte of the instruction 

    switch (opcode & 0x00FF) {
        // Ex9E - SKP Vx: Skip next instruction if key with the value of Vx is pressed.
        case 0x9E:
            if (chip8->keyboard.virtual_keyboard[chip8->registers.V[x]]) {
                chip8->registers.PC += 2;
            }
        break;

        // ExA1 - SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
        case 0xA1:
            if (!chip8->keyboard.virtual_keyboard[chip8->registers.V[x]]) {
                chip8->registers.PC += 2;
            }
        break;
    }
}

static char wait_key_press(struct Chip8* chip8) {
    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                char real_key = event.key.keysym.sym;
                int virtual_key = translate_keyboard_map(&chip8->keyboard, real_key);
                if (virtual_key != -1) {
                    return virtual_key;
                }
            }
        }
    }
}

static void chip8_execute_F_prefix_instruction(struct Chip8* chip8, unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00) >> 8;   // x - A 4-bit value, the lower 4 bits of the high byte of the instruction 

    switch (opcode & 0x00FF) {
        // Fx07 - LD Vx, DT: Set Vx = delay timer value.
        case 0x07:
            chip8->registers.V[x] = chip8->registers.DT;
        break;

        // Fx0A - LD Vx, K: Wait for a key press, store the value of the key in Vx.
        case 0x0A:
            chip8->registers.V[x] = wait_key_press(chip8);
        break;

        // Fx15 - LD DT, Vx: Set delay timer = Vx.
        case 0x15:
            chip8->registers.DT = chip8->registers.V[x];
        break;

        // Fx18 - LD ST, Vx: Set sound timer = Vx.
        case 0x18:
            chip8->registers.ST = chip8->registers.V[x];
        break;

        // Fx1E - ADD I, Vx: Set I = I + Vx.
        case 0x1E:
            chip8->registers.I += chip8->registers.V[x];
        break;

        // Fx29 - LD F, Vx: Set I = location of sprite for digit Vx.
        case 0x29:
            chip8->registers.I = chip8->registers.V[x] * 5;
        break;

        // Fx33 - LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
        case 0x33:
        {
            unsigned char value = chip8->registers.V[x];
            chip8->memory.memory_array[chip8->registers.I + 2] = value % 10;
            value /= 10;
            chip8->memory.memory_array[chip8->registers.I + 1] = value % 10;
            value /= 10;
            chip8->memory.memory_array[chip8->registers.I] = value % 10;
        }

        // Fx55 - LD [I], Vx: Store registers V0 through Vx in memory starting at location I.
        case 0x55:
        {
            for (unsigned char reg_index = 0; reg_index <= x; ++reg_index) {
                chip8->memory.memory_array[chip8->registers.I + reg_index] = chip8->registers.V[reg_index];
            }
        }
        break;

        // Fx65 - LD Vx, [I]: Read registers V0 through Vx from memory starting at location I.
        case 0x65:
        {
            for (unsigned char reg_index = 0; reg_index <= x; ++reg_index) {
                chip8->registers.V[reg_index] = chip8->memory.memory_array[chip8->registers.I + reg_index];
            }
        }
        break;
    }
}

static void chip8_execute_complex_instruction(struct Chip8* chip8, unsigned short opcode) {
    unsigned short nnn = opcode & 0x0FFF;       // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    unsigned char n = opcode & 0x000F;          // n or nibble - A 4-bit value, the lowest 4 bits of the instruction 
    unsigned char x = (opcode & 0x0F00) >> 8;   // x - A 4-bit value, the lower 4 bits of the high byte of the instruction 
    unsigned char y = (opcode & 0x00F0) >> 4;   // y - A 4-bit value, the upper 4 bits of the low byte of the instruction 
    unsigned char kk = opcode & 0x00FF;         // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
    
    switch (opcode & 0xF000) {
    
        // 1nnn - JP addr: Jump to location nnn.
        case 0x1000:
           chip8->registers.PC = nnn;
        break;
    
        // 2nnn - CALL addr: Call subroutine at nnn.
        case 0x2000:
            stack_push(chip8, chip8->registers.PC);
            chip8->registers.PC = nnn;
        break;

        // 3xkk - SE Vx, byte: Skip next instruction if Vx = kk.
        case 0x3000:
            if (chip8->registers.V[x] == kk) {
                chip8->registers.PC += 2;
            }
        break;

        // 4xkk - SNE Vx, byte: Skip next instruction if Vx != kk.
        case 0x4000:
            if (chip8->registers.V[x] != kk) {
                chip8->registers.PC += 2;
            }
        break;

        // 5xy0 - SE Vx, Vy: Skip next instruction if Vx = Vy.
        case 0x5000:
            if (chip8->registers.V[x] == chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
        break;

        // 6xkk - LD Vx, byte: Set Vx = kk.
        case 0x6000:
            chip8->registers.V[x] = kk;
        break;

        // 7xkk - ADD Vx, byte: Set Vx = Vx + kk.
        case 0x7000:
            chip8->registers.V[x] += kk;
        break;

        // 8xy_ - Multiple instructions starting with 8 prefix
        case 0x8000:
            chip8_execute_eight_prefix_instruction(chip8, opcode);
        break;

        // 9xy0 - SNE Vx, Vy: Skip next instruction if Vx != Vy.
        case 0x9000:
            if (chip8->registers.V[x] != chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
        break;

        // Annn - LD I, addr: Set I = nnn.
        case 0xA000:
            chip8->registers.I = nnn;
        break;

        // Bnnn - JP V0, addr: Jump to location nnn + V0.
        case 0xB000:
            chip8->registers.PC = nnn + chip8->registers.V[0];
        break;

        // Cxkk - RND Vx, byte: Set Vx = random byte AND kk.
        case 0xC000:
            srand(clock());
            chip8->registers.V[x] = (rand() % 255) & kk;
        break;

        // Dxyn - DRW Vx, Vy, n: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        case 0xD000:
        {
            const char* sprite = (const char*) &chip8->memory.memory_array[chip8->registers.I];
            chip8->registers.V[0xF] = draw_sprite(&chip8->screen, chip8->registers.V[x], chip8->registers.V[y], sprite, n);
        }
        break;

        // Ex__ - Multiple instructions starting with E prefix
        case 0xE000:
            chip8_execute_E_prefix_instruction(chip8, opcode);
        break;

        // Fx__ - Multiple instructions starting with F prefix
        case 0xF000:
            chip8_execute_F_prefix_instruction(chip8, opcode);
        break;
    }
}

void chip8_execute_instruction(struct Chip8* chip8, unsigned short opcode) {
    switch (opcode) {
    
    // 00E0 - CLS: Clear the display
    case 0x00E0:
        clear_screen(&chip8->screen);
        break;

    // 00EE - RET: Return from a subroutine
    case 0x00EE:
        chip8->registers.PC = stack_pop(chip8);
        break;

    default:
        chip8_execute_complex_instruction(chip8, opcode);
        break;
    }
}