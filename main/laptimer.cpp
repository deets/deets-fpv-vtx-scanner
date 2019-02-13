#include "laptimer.hh"

#include <esp_log.h>
#include <sys/param.h>

enum TriggerState
{
  DISARMED,
  ARMED,
  TRIGGERED
};


LapTimer::LapTimer(app_state_t& app_state, rtc6715_t& rtc)
  : Mode(app_state)
  , _rtc(rtc)
  , _last_laptime(0)
  , _laptime_acquired(false)
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

}

void LapTimer::setup()
{
  periodic(pdMS_TO_TICKS(1000 / 60));
  rtc6715_select_channel(&_rtc, _app_state.selected_channel);
  vTaskResume(_laptimer_task_handle);
}


app_mode_t LapTimer::update(ssd1306_display_t* display)
{
  for(int i=0; i < 128; ++i)
  {
    // FIXME: strictly speaking this must be 7, because we read 12 bit
    ssd1306_draw_pixel(display, i, 32 - (_rssi_readings[i] >> 6));
  }
  if(_laptime_acquired)
  {
    ESP_LOGI("laptimer", "laptime: %i", (int)(_last_laptime / 1000));
    _laptime_acquired = false;
  }
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
  const TickType_t frequency =  pdMS_TO_TICKS(1);
  ESP_LOGI("laptimer", "Frequency: %i", frequency);
  last_wake_time = xTaskGetTickCount ();

  uint16_t trigger_reading;
  int64_t trigger_time;
  TriggerState state = DISARMED;

  size_t pos = 0;
  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, frequency );
    uint16_t reading = _rssi_readings[pos] = rtc6715_read_rssi(&_rtc);
    _app_state.max_rssi_reading = MAX(_rssi_readings[pos], _app_state.max_rssi_reading);
    pos = (pos + 1) % 128;

    auto now = esp_timer_get_time();
    switch(state)
    {
    case DISARMED:
      if(reading > _app_state.trigger_arm_threshold)
      {
        state = ARMED;
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
          state = TRIGGERED;
        }
      }
      break;
    case TRIGGERED:
      if(reading < _app_state.trigger_disarm_threshold)
      {
        state = DISARMED;
      }
      break;
    }
  }
}
