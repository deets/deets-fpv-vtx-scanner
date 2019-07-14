// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"

class SettingsMode : public Mode
{
public:
  using Mode::Mode;
  app_mode_t update(Display& display) override;
  void input(input_t) override;
  app_mode_t return_mode;
protected:
  void setup_impl() override;
private:
  bool _done;
};
