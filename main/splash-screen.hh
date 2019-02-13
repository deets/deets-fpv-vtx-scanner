// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"

class SplashScreen : public Mode
{
public:
  SplashScreen(app_state_t&, app_mode_t follow_mode);
  void setup() override;
  app_mode_t update(ssd1306_display_t*) override;
  void teardown() override;
private:
  int _x;
  app_mode_t _follow_mode;
};
