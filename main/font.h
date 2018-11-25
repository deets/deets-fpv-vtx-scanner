// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "sprite.h"
#include "ssd1306.h"

typedef struct {
  int width;
  sprite_t glyphs[128];
} font_t;

int font_render(ssd1306_display_t *display, font_t* font, const char* text, int x, int y);
