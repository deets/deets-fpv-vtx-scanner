// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "appstate.h"
#include "ssd1306.h"

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


enum class input_t
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
  virtual void setup()=0;
  virtual app_mode_t update(ssd1306_display_t*)=0;
  virtual void teardown()=0;
  virtual void input(input_t) {};

  // Ensures peridodic wakeup
  // Compute argument like this:
  // pdMS_TO_TICKS(1000 / <FREQUENCY>)
  void periodic(TickType_t);
  int total_elapsed_ms() const;

protected:

  void notifyMainTask(uint32_t flags);

  app_state_t& _app_state;

private:

  TaskHandle_t _main_task_handle;

  TickType_t _last_wake_time;
  TickType_t _wake_period;
  TickType_t _periodic_start;

  TaskHandle_t _periodic_task_handle;
  StaticTask_t _periodic_task_buffer;
  StackType_t  _periodic_task_stack[PERIODIC_TASK_STACK_SIZE];

  void periodic_task_callback();

  static void s_periodic_task_callback(void*);
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
  void update(ssd1306_display_t* display);

private:
  app_state_t& _app_state;
  std::map<app_mode_t, std::unique_ptr<Mode>> _modes;
};
