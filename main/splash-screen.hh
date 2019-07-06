// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"

class SplashScreen : public Mode
{
public:
  SplashScreen(app_state_t&, app_mode_t follow_mode);
  void setup() override;
  app_mode_t update(Display&) override;
  void teardown() override;
  void input(input_t) override;

private:
  int _x;
  app_mode_t _follow_mode;

  int* _logo_arrived;
  bool _early_exit;
};
