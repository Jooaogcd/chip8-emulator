#include <assert.h>
#include <string.h>
#include "screen.h"

static void check_screen_bounds(int x, int y) {
    assert((x >= 0 && x < SCREEN_WIDTH) && (y >= 0 && y < SCREEN_HEIGHT));
}

void screen_pixel_set(struct Screen* screen, int x, int y) {
    check_screen_bounds(x, y);
    screen->pixels_array[x][y] = true;
}

bool screen_pixel_is_set(struct Screen* screen, int x, int y) {
    check_screen_bounds(x, y);
    return screen->pixels_array[x][y];
}

bool draw_sprite(struct Screen* screen, int x, int y, const char* sprite, int n_bytes) {
    bool pixel_change = false;

    for (int ly = 0; ly < n_bytes; ly++) {
        
        char c = sprite[ly];

        for (int lx = 0; lx < 8; lx++) {
            if ((c & (0b10000000 >> lx)) == 0) {
                continue;
            }

            if (screen->pixels_array[(lx + x) % SCREEN_WIDTH][(ly + y) % SCREEN_HEIGHT]) {
                pixel_change = true;
            }

            screen->pixels_array[(lx + x) % SCREEN_WIDTH][(ly + y) % SCREEN_HEIGHT] ^= true;
        }
    }

    return pixel_change;
}

void clear_screen(struct Screen* screen) {
    memset(screen->pixels_array, 0, sizeof(screen->pixels_array));
}