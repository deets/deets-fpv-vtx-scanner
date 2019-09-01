// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "mode.hh"
#include "rtc6715.hh"
#include "peak_detection_strategy.hh"
#include "laptime_tracker.hh"

#define LAPTIMER_TASK_STACK_SIZE 2000

class LapTimer : public Mode
{
  struct queue_message_t {
    PeakDetector::state_t state;
    ts_t peak;
  };

public:

  LapTimer(app_state_t&, RTC6715&, size_t display_width, LapTimeTracker&);
  app_mode_t update(Display&) override;

protected:
  void setup_impl() override;
  void teardown_impl() override;
  void input(input_t) override;

private:
  void peak_detector_callback(PeakDetector::state_t, ts_t peak);
  static void s_laptimer_task(void*);
  void laptimer_task();
  void process_queue();
  void display_waveform(Display&);
  void display_laptimes(Display&);

  RTC6715& _rtc;

  TaskHandle_t _laptimer_task_handle;
  StaticTask_t _laptimer_task_buffer;
  StackType_t  _laptimer_task_stack[LAPTIMER_TASK_STACK_SIZE];

  std::vector<uint16_t> _rssi_readings; // Display width

  PeakDetector _peak_detector;
  QueueHandle_t _task_q;

  LapTimeTracker& _lap_time_tracker;
};
