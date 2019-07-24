#include "laptimer.hh"
#include "buzzer.hh"
#include "font.h"
#include "buzzer.hh"

#include <esp_log.h>
#include <sys/param.h>
#include <string.h>

#define DISPLAY_SPLIT 32
#define LINE_LENGTH 12

LapTimer::LapTimer(app_state_t& app_state, RTC6715& rtc, size_t display_width)
  : Mode(app_state)
  , _rtc(rtc)
  , _rssi_readings(display_width)
  , _peak_detector(
    [this](PeakDetector::state_t state, PeakDetector::ts_t ts) {
      peak_detector_callback(state, ts);
    },
    app_state.peak_detection_config,
    app_state.max_rssi_reading
    )
{
  _laptimer_task_handle = xTaskCreateStaticPinnedToCore(
    s_laptimer_task,       // Function that implements the task.
    "LPT",          // Text name for the task.
    LAPTIMER_TASK_STACK_SIZE,      // Stack size in bytes, not words.
    this,
    tskIDLE_PRIORITY,// Priority at which the task is created.
    _laptimer_task_stack,          // Array to use as the task's stack.
    &_laptimer_task_buffer, // Variable to hold the task's data structure.
    1 // Core 1
    );
  vTaskSuspend(_laptimer_task_handle);
  _task_q = xQueueCreate(20, sizeof(queue_message_t));
  assert(_task_q);
}

void LapTimer::setup_impl()
{
  ESP_LOGE("laptimer", "setup");
  auto screen_period = pdMS_TO_TICKS(1000 / 60);
  periodic(screen_period);
  _rtc.select_channel(_app_state.selected_channel);
  _peak_detector.reset();
  vTaskResume(_laptimer_task_handle);
}

app_mode_t LapTimer::update(Display& display)
{
  int max = MAX(_app_state.max_rssi_reading, _app_state.trigger_arm_threshold);
  int min = _app_state.min_rssi_reading;
  int divider = max - min;
  // We don't have enough data yet
  if(divider <= 0)
  {
    return LAPTIMER;
  }

  const auto dw = display.width();

  for(int i=0; i < dw; ++i)
  {
    display.draw_pixel(i, DISPLAY_SPLIT - (DISPLAY_SPLIT * (_rssi_readings[i] - min) / divider));
  }
  // horizontal separation
  display.hline(
    0, dw - 1,
    DISPLAY_SPLIT + 1
   );
  process_queue();
  // char buffer[256];
  // sprintf(buffer, "%i", (int)(_last_laptime / 1000));
  // display.font_render(
  //   NORMAL,
  //   buffer,
  //   24,
  //   64 - 8 - 8
  //  );

  return LAPTIMER;
}


void LapTimer::process_queue()
{
  queue_message_t m;
  if(xQueueGenericReceive(_task_q, &m, 0, false))
  {
    switch(m.state)
    {
    case PeakDetector::PEAK:
      ESP_LOGI("laptimer", "laptime!");
      buzzer_buzz(100, 1);
      break;
    case PeakDetector::BELOW_THRESHOLD:
      ESP_LOGI("laptimer", "BELOW_THRESHOLD");
      break;
    case PeakDetector::DETECTING_PEAK:
      ESP_LOGI("laptimer", "DETECTING_PEAK");
      break;
    }
  }
}

void LapTimer::peak_detector_callback(PeakDetector::state_t s, PeakDetector::ts_t peak)
{
  const queue_message_t m = { s, peak };
  xQueueSendToBack(_task_q, &m, 0);
}


void LapTimer::teardown_impl()
{
  vTaskSuspend(_laptimer_task_handle);
  while(eTaskGetState(_laptimer_task_handle) != eSuspended)
  {
    ESP_LOGI("laptimer", "task not yet suspended");
  }
}

void LapTimer::s_laptimer_task(void* data)
{
  ((LapTimer*)data)->laptimer_task();
}


void LapTimer::laptimer_task()
 {
  TickType_t last_wake_time;
  const TickType_t period =  pdMS_TO_TICKS(1);
  ESP_LOGI("laptimer", "laptimer task period: %ims", period);
  last_wake_time = xTaskGetTickCount ();

  size_t pos = 0;
  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, period );

    uint16_t reading = _rssi_readings[pos] = _rtc.read_rssi();

    _app_state.max_rssi_reading = MAX(_rssi_readings[pos], _app_state.max_rssi_reading);
    _app_state.min_rssi_reading = MIN(_rssi_readings[pos], _app_state.min_rssi_reading);

    _app_state.laptime_buffer[_app_state.laptime_buffer_pos] = uint8_t(reading >> 4); // we read 12 bit, limit down to one byte
    _app_state.laptime_buffer_pos = (_app_state.laptime_buffer_pos + 1) % _app_state.laptime_buffer.size();

    pos = (pos + 1) % _rssi_readings.size();

    auto now = esp_timer_get_time();
    _peak_detector.feed(now, reading);
  }
}
