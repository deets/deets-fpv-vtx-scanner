// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#include "settings.hh"
#include "font.h"

void SettingsMode::setup_impl()
{
  _done = false;
}


app_mode_t SettingsMode::update(Display& display)
{
  display.font_render(
    NORMAL,
    "settings",
    24,
    64 - 8 - 8
   );

  return _done ? return_mode : SETTINGS;
}


void SettingsMode::input(input_t)
{
  _done = true;
}
