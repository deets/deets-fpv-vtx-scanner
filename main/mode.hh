// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "appstate.hh"
#include "display.hh"

#include <freertos/task.h>

#include <memory>
#include <map>

#define PERIODIC_TASK_STACK_SIZE 1024
#define READER_TASK_WAKEUP_FLAG (1 << 0)
#define RIGHT_PIN_ISR_FLAG (1 << 1)
#define LEFT_PIN_ISR_FLAG (1 << 2)
#define UP_PIN_ISR_FLAG (1 << 3)
#define DOWN_PIN_ISR_FLAG (1 << 4)
#define PERIODIC_TASK_WAKEUP_FLAG (1 << 5)


enum input_t
{
  RIGHT_BUTTON,
  LEFT_BUTTON,
  MODE_BUTTON,
  SETTINGS_BUTTON
};

class Mode
{
public:
  Mode(app_state_t&);
  void setup();
  void teardown();

  virtual app_mode_t update(Display&)=0;
  virtual void input(input_t) {};

protected:
  void periodic(int ms);

  virtual void setup_impl() {};
  virtual void teardown_impl() {};

  void notify_main_task(uint32_t flags);

  app_state_t& _app_state;

private:
  static void s_periodic_timer_callback(void*);

  TaskHandle_t _main_task_handle;
  esp_timer_handle_t _periodic_timer_handle;
};


class ModeManager
{
public:
  ModeManager(app_state_t& app_state, app_mode_t start, std::unique_ptr<Mode>);
  void add_mode(app_mode_t, std::unique_ptr<Mode>);

  Mode& active();
  const Mode& active() const;

  void change_active_mode(int);
  void change_active_mode(app_mode_t);

  void input(input_t);
  void update(Display& display);

private:
  app_state_t& _app_state;
  std::map<app_mode_t, std::unique_ptr<Mode>> _modes;
};
