// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"

class BackSetting;

template<typename T>
class LowerUpperBoundSetting : public Setting
{
public:
  LowerUpperBoundSetting(std::string name, T& value, T lower, T upper, T step)
    : Setting(name)
    , _value(value)
    , _lower(lower)
    , _upper(upper)
    , _step(step)
  {
    _value = clamp(_value);
  }

  void right() override
  {
    _value = clamp(_value + _step);
  }

  void left() override
  {
    _value = clamp(_value - _step);
  }

private:

  T clamp(T value)
  {
    return std::max(std::min(value, _upper), _lower);
  }

  T& _value;
  T _lower, _upper, _step;
};

class SettingsMode : public Mode
{
public:
  using Mode::Mode;
  app_mode_t update(Display& display) override;
  void input(input_t) override;
  app_mode_t return_mode;
  std::vector<Setting*>* settings;
protected:
  void setup_impl() override;
private:
  friend class BackSetting;

  bool _done;
  ssize_t _pos;
  std::vector<Setting*> _all_settings;
};
