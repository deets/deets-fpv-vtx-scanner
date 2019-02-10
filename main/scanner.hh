// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"
#include "vtx-info.h"
#include "channel_display.h"
#include "vtx-info.h"
#include "goggle_info.h"
#include "rtc6715.h"


#define SCANNER_TASK_STACK_SIZE 2000
#define SCANS_PER_SECOND 20

class Scanner : public Mode
{
public:
  Scanner(app_state_t&, rtc6715_t&);
  void setup() override;
  void update(ssd1306_display_t*) override;
  void teardown() override;
  void input(Input) override;

private:
  static void s_scanner_task(void*);
  void scanner_task();

  rtc6715_t _rtc;

  channel_display_t _channels;
  vtx_info_t* _selected_vtx;
  goggle_info_t* _selected_goggle;
  bool _has_ham; // are we ham-licensed?

  TaskHandle_t _scanner_task_handle;
  StaticTask_t _scanner_task_buffer;
  StackType_t  _scanner_task_stack[SCANNER_TASK_STACK_SIZE];

};
