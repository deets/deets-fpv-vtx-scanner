// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "goggle_info.hh"
#include "channel_display.hh"
#include "font.h"
#include "rtc6715.hh"

#include <sstream>

goggle_info_t aomway_commander_v1_info = {
  .name = "AOC1",
  .channel_names = {
    "A4", "E1", "A3", "A2", "E2", "A1", "C1", "E3", "B1", "D1", "C2", "B2",
    "D2", "C3", "E4", "B3", "D3", "C4", "B4", "D4", "C5", "E5", "B5", "D5",
    "C6", "B6", "D6", "E6", "C7", "B7", "D7", "C8", "B8", "D8", "E7", "A5",
    "A6", "E8", "A7", "A8",
  }
};


void goggle_display_draw(Display& display, goggle_info_t* goggle, int cursor_pos)
{
  int y = display.height();
  int x = 127 - CHANNEL_LEFT;
  const char* channel_name = goggle->channel_names[cursor_pos];

  char buffer[256];
  sprintf(buffer, "%s:%s", goggle->name, channel_name);
  x -= display.font_text_width(NORMAL, buffer);
  display.font_render(NORMAL, buffer, x, y);

  sprintf(buffer, "%i", RTC6715::frequency_for_channel(cursor_pos));
  x = display.width() / 2 - display.font_text_width(SMALL, buffer) / 2;
  display.font_render(SMALL, buffer, x, y);
}
