// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"
#include "rtc6715.h"

#define LAPTIMER_TASK_STACK_SIZE 2000


class LapTimer : public Mode
{
  enum TriggerState
  {
    DISARMED,
    ARMED,
    TRIGGERED,
    COOLDOWN
  };

public:

  LapTimer(app_state_t&, rtc6715_t&, size_t display_width);
  app_mode_t update(Display&) override;

protected:
  void setup_impl() override;
  void teardown_impl() override;

private:
  static void s_laptimer_task(void*);
  void laptimer_task();

  rtc6715_t& _rtc;

  TaskHandle_t _laptimer_task_handle;
  StaticTask_t _laptimer_task_buffer;
  StackType_t  _laptimer_task_stack[LAPTIMER_TASK_STACK_SIZE];

  std::vector<uint16_t> _rssi_readings; // Display width

  int64_t _last_laptime;
  bool _laptime_acquired;
  TriggerState _state;
};
