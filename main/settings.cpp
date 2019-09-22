// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#include "settings.hh"
#include "font.h"

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
  const auto rows = display.height() / (NORMAL.size + PADDING) + 1;
  for(size_t i=0; i < rows; ++i)
  {
    if(i < _all_settings.size())
    {
      display.font_render(
        NORMAL,
        _all_settings[i]->name().c_str(),
        24,
        V_MIDDLE + (NORMAL.size + PADDING) * i
        );
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
  default:
    break;
  }
}
