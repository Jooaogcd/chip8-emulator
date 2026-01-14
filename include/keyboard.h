#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdbool.h>
#include "config.h"


struct Keyboard {
    bool virtual_keyboard[NUMBER_OF_KEYBOARD_KEYS];
    const char* keymap;
};

void set_keyboard_map(struct Keyboard* keyboard, const char* map);

int translate_keyboard_map(struct Keyboard* keyboard, char key);

void keyboard_set_key_down(struct Keyboard* keyboard, int key);

void keyboard_set_key_up(struct Keyboard* keyboard, int key);

bool key_is_down(struct Keyboard* keyboard, int key);

#endif