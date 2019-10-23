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
    , _unit("")
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

  const char* value() const override
  {
    static char buffer[50];
    sprintf(buffer, "<%i%s>", static_cast<int>(_value), _unit);
    return buffer;
  }

  LowerUpperBoundSetting* unit(const char* u)
  {
    _unit = u;
    return this;
  }

private:

  T clamp(T value)
  {
    return std::max(std::min(value, _upper), _lower);
  }

  T& _value;
  T _lower, _upper, _step;

  const char* _unit;
};

class BoolSetting : public Setting
{
public:
  BoolSetting(std::string name, bool& value)
    : Setting(name)
    , _value(value)
  {
  }

  void right() override
  {
    _value = !_value;
  }

  void left() override
  {
    _value = !_value;
  }

  const char* value() const override
  {
    static char buffer[50];
    sprintf(buffer, "<%s>", _value ? "ON" : "OFF");
    return buffer;
  }

private:

  bool& _value;
};


class SettingsMode : public Mode
{
public:
  SettingsMode(app_state_t&);
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

  std::unique_ptr<BackSetting> _back_setting;
};
