#include "laptimer.hh"
#include "buzzer.hh"
#include "font.h"
#include "buzzer.hh"
#include "ble.hh"

#include <esp_log.h>
#include <sys/param.h>
#include <string.h>

#define DISPLAY_SPLIT 32
#define LINE_LENGTH 12
#define LAPTIME_FORMAT "%2i: %.3f"


LapTimer::LapTimer(app_state_t& app_state, RTC6715& rtc, size_t display_width, LapTimeTracker& lpt)
  : Mode(app_state)
  , _rtc(rtc)
  , _rssi_readings(display_width)
  , _peak_detector(
    [this](PeakDetector::state_t state, ts_t ts) {
      peak_detector_callback(state, ts);
    },
    app_state.peak_detection_config,
    app_state.max_rssi_reading
    )
  , _lap_time_tracker(lpt)

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
  _lap_time_tracker.reset();
  vTaskResume(_laptimer_task_handle);
}


void LapTimer::display_waveform(Display& display)
{
  int max = MAX(_app_state.max_rssi_reading, _app_state.trigger_arm_threshold);
  int min = _app_state.min_rssi_reading;
  int divider = max - min;
  // We don't have enough data yet
  if(divider <= 0)
  {
    return;
  }

  const auto dw = display.width();

  for(int i=0; i < dw; ++i)
  {
    display.draw_pixel(i, DISPLAY_SPLIT - (DISPLAY_SPLIT * (_rssi_readings[i] - min) / divider));
  }
}


void LapTimer::display_laptimes(Display& display)
{
  char large_line[256];
  auto y = DISPLAY_SPLIT + 1;
  const auto current = _lap_time_tracker.laptime(0);
  std::vector<std::vector<char>> prev_laptimes;
  if(current)
  {
    sprintf(large_line, LAPTIME_FORMAT, current.count, current.seconds());

    for(int i=1; i < 5; ++i)
    {
      const auto prev = _lap_time_tracker.laptime(-i);
      if(prev)
      {
        char buffer[256];
        sprintf(buffer, LAPTIME_FORMAT, prev.count, prev.seconds());
        const auto len = strlen(buffer);
        std::vector<char> s;
        std::copy(buffer, buffer + len + 1, std::back_inserter(s));
        prev_laptimes.push_back(s);
      }
      else
      {
        break;
      }
    }
  }
  else
  {
    sprintf(large_line, "Waiting..");
  }
  display.font_render(
    NORMAL,
    large_line,
    0,
    y + NORMAL.size + 2
    );
  auto sy = y + SMALL.size + 3;
  for(const auto& prev : prev_laptimes)
  {

    display.font_render(
      SMALL,
      prev.data(),
      display.width() / 2 + 2,
      sy
    );
    sy += SMALL.size + 2;
  }
}


app_mode_t LapTimer::update(Display& display)
{
  process_queue();
  display_waveform(display);
  // horizontal separation
  display.hline(0, display.width() - 1, DISPLAY_SPLIT + 1);
  display_laptimes(display);

  return LAPTIMER;
}


void LapTimer::input(input_t button)
{
  switch(button)
  {
  case input_t::RIGHT_BUTTON:
    break;
  case input_t::LEFT_BUTTON:
    _lap_time_tracker.reset();
    break;
  default:
    break;
  }
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
      if(_lap_time_tracker.record(m.peak))
      {
        ble_notify(NOTIFY_NEW_LAPTIME);
      }
      break;
    case PeakDetector::BELOW_THRESHOLD:
      ESP_LOGI("laptimer", "BELOW_THRESHOLD");
      break;
    case PeakDetector::DETECTING_PEAK:
      ESP_LOGI("laptimer", "DETECTING_PEAK");
      break;
    case PeakDetector::COOLDOWN:
      ESP_LOGI("laptimer", "COOLDOWN");
      break;
    }
  }
}

void LapTimer::peak_detector_callback(PeakDetector::state_t s, ts_t peak)
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
