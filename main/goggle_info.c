// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "goggle_info.h"
#include "channel_display.h"
#include "font.h"
#include "p2font.h"

#include <string.h>

goggle_info_t aomway_commander_v1_info = {
  .name = "AOC1",
  .channel_names = {
    "A4", "E1", "A3", "A2", "E2", "A1", "C1", "E3", "B1", "D1", "C2", "B2",
    "D2", "C3", "E4", "B3", "D3", "C4", "B4", "D4", "C5", "E5", "B5", "D5",
    "C6", "B6", "D6", "E6", "C7", "B7", "D7", "C8", "B8", "D8", "E7", "A5",
    "A6", "E8", "A7", "A8",
  }
};


void goggle_display_draw(ssd1306_display_t *display, goggle_info_t* goggle, int cursor_pos)
{
  int y = CHANNELS_BOTTOM + 5;
  int x = 127 - CHANNEL_LEFT;
  const char* channel_name = goggle->channel_names[cursor_pos];
  x -= p2font.width * (strlen(goggle->name) + 1 + strlen(channel_name));
  x = font_render(display, &p2font, goggle->name, x, y);
  x = font_render(display, &p2font, ":", x, y);
  x = font_render(display, &p2font, channel_name, x, y);
}
