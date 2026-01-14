#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdbool.h>
#include "config.h"

struct Screen
{
    bool pixels_array[SCREEN_WIDTH][SCREEN_HEIGHT];
};

void screen_pixel_set(struct Screen* screen, int x, int y);

bool screen_pixel_is_set(struct Screen* screen, int x, int y);

bool draw_sprite(struct Screen* screen, int x, int y, const char* sprite, int n_bytes);

void clear_screen(struct Screen* screen);

#endif