// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "goggle_info.hh"
#include "channel_display.hh"

#include <sstream>

#define FONT  u8g2_font_helvB08_tf

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
  std::stringbuf buf;
  std::ostream os (&buf);
  os << goggle->name << ":" << channel_name;
  x -= display.font_text_width(FONT, buf.str().c_str());
  display.font_render(FONT, buf.str().c_str(), x, y);
}
