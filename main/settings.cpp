// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#include "settings.hh"
#include "font.h"
#include <esp_log.h>

#define FONT SMALL

class BackSetting : public Setting
{
public:
  using Setting::Setting;

  void right() override
  {
    settings_mode->_done = true;
  }
  void left() override
  {
    settings_mode->_done = true;
  }

  const char* value() const
  {
    return "<YES>";
  }
  SettingsMode* settings_mode;
};


namespace {

const int H_MIDDLE = 64;
const int V_MIDDLE = 32;
const int PADDING = 2;

auto _back_setting = new BackSetting{"Back"};

} // end ns anon

void SettingsMode::setup_impl()
{
  _done = false;
  _pos = 0;
  _back_setting->settings_mode = this;

  _all_settings.clear();
  _all_settings.push_back(_back_setting);
  std::copy(
    settings->begin(),
    settings->end(),
    std::back_inserter(_all_settings)
    );
}


app_mode_t SettingsMode::update(Display& display)
{
  const auto rows = display.height() / (FONT.size + PADDING) + 1;
  const auto start = -rows / 2;
  const auto end = rows + start;
  for(auto i=start; i < end; ++i)
  {
    const auto entry = i + _pos;
    if(entry >= 0 && entry < _all_settings.size())
    {
      const auto width = display.font_text_width(FONT, _all_settings[entry]->name());
      const auto v = V_MIDDLE + (FONT.size + PADDING) * i;
      display.font_render(
        FONT,
        _all_settings[entry]->name(),
        H_MIDDLE - width,
        v
        );
      if(entry == _pos)
      {
        display.font_render(
          FONT,
          _all_settings[entry]->value(),
          H_MIDDLE + PADDING,
          v
          );
      }
    }
  }
  return _done ? return_mode : SETTINGS;
}


void SettingsMode::input(input_t in)
{
  switch(in)
  {
  case LEFT_BUTTON:
    _all_settings[_pos]->left();
    break;
  case RIGHT_BUTTON:
    _all_settings[_pos]->right();
    break;
  case MODE_BUTTON:
    _pos = (_pos - 1) % _all_settings.size();
    break;
  case SETTINGS_BUTTON:
    _pos = (_pos + 1) % _all_settings.size();
    break;
  }
}
