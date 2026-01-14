#include <assert.h>
#include "keyboard.h"

static void check_keyboard_bounds(int key) {
    assert(key >= 0 && key < NUMBER_OF_KEYBOARD_KEYS);
}


void set_keyboard_map(struct Keyboard* keyboard, const char* map) {
    keyboard->keymap = map;
}

int translate_keyboard_map(struct Keyboard* keyboard, char key) {
    for (int i = 0; i < NUMBER_OF_KEYBOARD_KEYS; i++) {
        if (keyboard->keymap[i] == key) {
            return i;
        }
    }

    return -1; // key not found
}

void keyboard_set_key_down(struct Keyboard* keyboard, int key) {
    check_keyboard_bounds(key);
    keyboard->virtual_keyboard[key] = true;
}

void keyboard_set_key_up(struct Keyboard* keyboard, int key) {
    check_keyboard_bounds(key);
    keyboard->virtual_keyboard[key] = false;
}

bool key_is_down(struct Keyboard* keyboard, int key) {
    check_keyboard_bounds(key);
    return keyboard->virtual_keyboard[key];
}