#include <stdio.h>
#include <Windows.h>
#include "SDL2/SDL.h"
#include "chip8.h"

const char keyboard_map[NUMBER_OF_KEYBOARD_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3, 
    SDLK_4, SDLK_5, SDLK_6, SDLK_7, 
    SDLK_8, SDLK_9, SDLK_a, SDLK_b, 
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("There are no program to load!\n");
        return -1;
    }

    const char* filename = argv[1];

    FILE* file = fopen(filename, "rb");
    if(!file) {
        printf("Failed to open the program file.\n");
        return -1;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read the file and get the content
    char file_buffer[file_size];
    int content_read = fread(file_buffer, file_size, 1, file);
    if (content_read != 1) {
        printf("Failed to read the file.\n");
        fclose(file);
        return -1;
    }

    struct Chip8 chip8;
    chip8_init(&chip8); // initialize the emulator
    chip8_load_program(&chip8, file_buffer, file_size); // load the program to the chip8 memory
    set_keyboard_map(&chip8.keyboard, keyboard_map); // set the keyboard_map for the chip8 keyboard instance


    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow(
        MAIN_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    while(1) {

        SDL_Event event;
        while(SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_QUIT:
                    goto out;
                    break;

                case SDL_KEYDOWN:
                {
                    char real_key = event.key.keysym.sym;
                    int virtual_key = translate_keyboard_map(&chip8.keyboard, real_key);
                    if (virtual_key != -1) {
                        keyboard_set_key_down(&chip8.keyboard, virtual_key);
                    }
                }
                    break;

                case SDL_KEYUP:
                {
                    char real_key = event.key.keysym.sym;
                    int virtual_key = translate_keyboard_map(&chip8.keyboard, real_key);
                    if (virtual_key != -1) {
                        keyboard_set_key_up(&chip8.keyboard, virtual_key);
                    }
                }
                break;
            }

        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                if (screen_pixel_is_set(&chip8.screen, x, y)) {
                    SDL_Rect rect;
                    rect.x = x * CHIP8_WINDOW_SCALER;
                    rect.y = y * CHIP8_WINDOW_SCALER;
                    rect.w = CHIP8_WINDOW_SCALER;
                    rect.h = CHIP8_WINDOW_SCALER;
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        
        SDL_RenderPresent(renderer);

        if(chip8.registers.DT > 0) {
            Sleep(100); // Sleep from Windows.h (100 milliseconds)
            chip8.registers.DT -= 1;
        }

        if(chip8.registers.ST > 0) {
            Beep(1600, (16 * chip8.registers.ST)); // register value is the beep duration in seconds.
            chip8.registers.ST = 0;
        }

        unsigned short opcode = memory_get_short(&chip8.memory, chip8.registers.PC); // get the first opcode from the program
        chip8.registers.PC += 2; // set the PC to the next opcode
        chip8_execute_instruction(&chip8, opcode); // execute the instruction

    }



out:    
    SDL_DestroyWindow(window);

    return 0;
}