// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "font.h"

#include <string.h>

int font_render(ssd1306_display_t *display, font_t* font, const char* text, int x, int y)
{
  size_t len = strlen(text);
  for(int i=0; i < len; ++i)
  {
    ssd1306_blit(display, &font->glyphs[(int)text[i]], x, y);
    x += font->width;
  }
  return x;
}

int font_text_width(font_t* font, const char* text)
{
  int width = 0;
  size_t len = strlen(text);
  for(int i=0; i < len; ++i)
  {
    width += font->width;
  }
  return width;
}
