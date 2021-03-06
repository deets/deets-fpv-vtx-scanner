// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"
#include "vtx-info.hh"
#include "channel_display.hh"
#include "vtx-info.hh"
#include "goggle_info.hh"
#include "rtc6715.hh"


#define SCANNER_TASK_STACK_SIZE 2000
#define SCANS_PER_SECOND 20

class Scanner : public Mode
{
public:
  Scanner(app_state_t&, RTC6715&);
  app_mode_t update(Display&) override;
  void input(input_t) override;

protected:
  void setup_impl() override;
  void teardown_impl() override;

private:
  static void s_scanner_task(void*);
  void scanner_task();

  RTC6715& _rtc;

  channel_display_t _channels;
  vtx_info_t* _selected_vtx;
  goggle_info_t* _selected_goggle;
  bool _has_ham; // are we ham-licensed?

  TaskHandle_t _scanner_task_handle;
  StaticTask_t _scanner_task_buffer;
  StackType_t  _scanner_task_stack[SCANNER_TASK_STACK_SIZE];

};
