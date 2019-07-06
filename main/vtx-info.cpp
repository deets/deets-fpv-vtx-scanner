// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "vtx-info.hh"
#include "channel_display.hh"

#include <sstream>

#define FONT  u8g2_font_helvB08_tf

vtx_info_t tbs_unify_info = {
  .name = "TBS",
  .channel_names = {
    "E4", "R1", "E3", "E2", "R2", "E1", "A8", "R3", "B1", "F1", "A7",
    "B2", "F2", "A6", "R4", "B3", "F3", "A5", "B4", "F4", "A4", "R5",
    "B5", "F5", "A3", "B6", "F6", "R6", "A2", "B7", "F7", "A1", "B8",
    "F8", "R7", "E5", "E6", "R8", "E7", "E8"
  },
  .channel_legal = {
    FORBIDDEN, HAM, HAM, HAM, HAM, HAM, HAM, HAM, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL,
    LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, LEGAL, HAM, HAM, HAM, HAM, HAM, FORBIDDEN, FORBIDDEN,
  }
};


void vtx_display_draw(Display& display, vtx_info_t* vtx, int cursor_pos)
{
  int y = display.height();
  int x = CHANNEL_LEFT;

  std::stringbuf buf;
  std::ostream os (&buf);
  os << vtx->name << ":" << vtx->channel_names[cursor_pos];
  display.font_render(FONT, buf.str().c_str(), x, y);
}
