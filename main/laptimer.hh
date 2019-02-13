// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#pragma once

#include "mode.hh"
#include "rtc6715.h"

#define LAPTIMER_TASK_STACK_SIZE 2000


class LapTimer : public Mode
{
public:

  LapTimer(app_state_t&, rtc6715_t&);
  void setup() override;
  app_mode_t update(ssd1306_display_t*) override;
  void teardown() override;

private:
  static void s_laptimer_task(void*);
  void laptimer_task();

  rtc6715_t& _rtc;

  TaskHandle_t _laptimer_task_handle;
  StaticTask_t _laptimer_task_buffer;
  StackType_t  _laptimer_task_stack[LAPTIMER_TASK_STACK_SIZE];

  uint16_t _rssi_readings[128];

  int64_t _last_laptime;
  bool _laptime_acquired;
};
