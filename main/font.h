// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "sprite.h"
#include "ssd1306.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int width;
  sprite_t glyphs[128];
} font_t;

int font_render(ssd1306_display_t *display, font_t* font, const char* text, int x, int y);

int font_text_width(font_t* font, const char* text);

#ifdef __cplusplus
}
#endif
