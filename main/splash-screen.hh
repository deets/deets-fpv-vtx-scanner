// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"

class SplashScreen : public Mode
{
public:
  using Mode::Mode;
  void setup() override;
  void update(ssd1306_display_t*) override;
  void teardown() override;
private:
  int _x;
};
