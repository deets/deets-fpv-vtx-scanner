#include "laptimer.hh"
#include "buzzer.hh"
#include "font.h"

#include <esp_log.h>
#include <sys/param.h>
#include <string.h>

#define DISPLAY_SPLIT 32
#define LINE_LENGTH 12

namespace {

#define FONT  u8g2_font_helvB08_tf

#include "dot.xbm"

sprite_t dot = {
  dot_bits,
  dot_width, dot_height,
  dot_width >> 1, dot_height >> 1
};

#include "cursor.xbm"

sprite_t cursor = {
  cursor_bits,
  cursor_width, cursor_height,
  1, -2
};


} // end ns anonymous

LapTimer::LapTimer(app_state_t& app_state, rtc6715_t& rtc)
  : Mode(app_state)
  , _rtc(rtc)
  , _last_laptime(0)
  , _laptime_acquired(false)
{
  app_state.laptime_buffer_pos = 0;
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

  std::fill(app_state.laptime_buffer.begin(), app_state.laptime_buffer.end(), 0);
}

void LapTimer::setup()
{
  auto screen_period = pdMS_TO_TICKS(1000 / 60);
  periodic(screen_period);
  rtc6715_select_channel(&_rtc, _app_state.selected_channel);
  vTaskResume(_laptimer_task_handle);
}

app_mode_t LapTimer::update(Display& display)
{
  static int _dotpos = 10;
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
  int trigger_arm_pos = DISPLAY_SPLIT - (DISPLAY_SPLIT * (_app_state.trigger_arm_threshold - min) / divider);
  int trigger_disarm_pos = DISPLAY_SPLIT - (DISPLAY_SPLIT * (_app_state.trigger_disarm_threshold - min) / divider);

  display.hline(
     0, LINE_LENGTH,
     trigger_arm_pos
   );
  display.hline(
    0, dw - 1,
    DISPLAY_SPLIT + 1
   );
  display.hline(
    dw - LINE_LENGTH, dw - 1,
    trigger_disarm_pos
   );

  switch(_state)
  {
  case DISARMED:
    display.blit(dot, LINE_LENGTH, trigger_arm_pos);
    break;
  case TRIGGERED:
    display.blit(dot, dw - LINE_LENGTH, trigger_disarm_pos);
    break;
  default:
    break;
  }

  _dotpos = (_dotpos + 1) % 120;
  display.blit(cursor, _dotpos, 20);

  if(_laptime_acquired)
  {
    ESP_LOGI("laptimer", "laptime: %i", (int)(_last_laptime / 1000));
    _laptime_acquired = false;
    buzzer_buzz(150, 1);
  }
  char buffer[256];
  sprintf(buffer, "%i", (int)(_last_laptime / 1000));
  display.font_render(
    NORMAL,
    buffer,
    24,
    64 - 8 - 8
   );
  return LAPTIMER;
}


void LapTimer::teardown()
{
  vTaskSuspend(_laptimer_task_handle);
  periodic(0);
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

  uint16_t trigger_reading = 0;
  int64_t trigger_time = 0;
  _state = DISARMED;

  size_t pos = 0;
  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, period );
    uint16_t reading = _rssi_readings[pos] = rtc6715_read_rssi(&_rtc);
    _app_state.laptime_buffer[_app_state.laptime_buffer_pos] = uint8_t(reading >> 4); // we read 12 bit, limit down to one byte
    _app_state.laptime_buffer_pos = (_app_state.laptime_buffer_pos + 1) % _app_state.laptime_buffer.size();

    _app_state.max_rssi_reading = MAX(_rssi_readings[pos], _app_state.max_rssi_reading);
    _app_state.min_rssi_reading = MIN(_rssi_readings[pos], _app_state.min_rssi_reading);
    pos = (pos + 1) % 128;

    auto now = esp_timer_get_time();
    switch(_state)
    {
    case DISARMED:
      if(reading > _app_state.trigger_arm_threshold)
      {
        _state = ARMED;
        trigger_time = now;
        trigger_reading = reading;
      }
      break;
    case ARMED:
      if(reading > trigger_reading)
      {
        trigger_time = now;
        trigger_reading = reading;
      }
      else
      {
        if(now - trigger_time > _app_state.trigger_max_latency)
        {
          _last_laptime = trigger_time;
          _laptime_acquired = true;
          _state = COOLDOWN;
        }
      }
      break;
    case COOLDOWN:
      if(now - trigger_time >= _app_state.trigger_cooldown)
      {
        _state = TRIGGERED;
      }
      break;
    case TRIGGERED:
      if(reading < _app_state.trigger_disarm_threshold)
      {
        _state = DISARMED;
      }
      break;
    }
  }
}
