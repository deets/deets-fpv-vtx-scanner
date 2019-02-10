#include "scanner.hh"
#include "ble.h"

#include <freertos/task.h>

namespace {

void copy_legal_channel_info(vtx_info_t* vtx, channel_display_t* channels, int has_ham)
{
  for(size_t i=0; i < CHANNEL_NUM; ++i)
  {
    channels->channels[i].legal = vtx->channel_legal[i] == LEGAL ||
       (vtx->channel_legal[i] == HAM && has_ham);
  }
 }
} // end ns anonymous


Scanner::Scanner(app_state_t& app_state, rtc6715_t& rtc)
  : Mode(app_state)
  ,_rtc(rtc)
  , _has_ham(false)
{
  _selected_vtx = &tbs_unify_info;
  _selected_goggle = &aomway_commander_v1_info;
  channel_display_init(&_channels, &_app_state);

  copy_legal_channel_info(
    _selected_vtx,
    &_channels,
    _has_ham
    );

  app_state.selected_channel = 0;
  _scanner_task_handle = xTaskCreateStatic(
    s_scanner_task,       // Function that implements the task.
    "SCR",          // Text name for the task.
    SCANNER_TASK_STACK_SIZE,      // Stack size in bytes, not words.
    this,
    tskIDLE_PRIORITY,// Priority at which the task is created.
    _scanner_task_stack,          // Array to use as the task's stack.
    &_scanner_task_buffer // Variable to hold the task's data structure.
    );
  vTaskSuspend(_scanner_task_handle);
}


void Scanner::setup()
{
  vTaskResume(_scanner_task_handle);
}


app_mode_t Scanner::update(ssd1306_display_t* display)
{
  channel_display_draw(display, &_channels);
  vtx_display_draw(display, _selected_vtx, _app_state.selected_channel);
  goggle_display_draw(display, _selected_goggle, _app_state.selected_channel);
  return SCANNER;
}


void Scanner::teardown()
{
  vTaskSuspend(_scanner_task_handle);
}


void Scanner::s_scanner_task(void* data)
{
  ((Scanner*)data)->scanner_task();
}


void Scanner::scanner_task()
 {
  TickType_t last_wake_time;
  const TickType_t frequency =  pdMS_TO_TICKS(1000 / SCANS_PER_SECOND);
  last_wake_time = xTaskGetTickCount ();
  uint16_t current_channel = 0;

  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, frequency );
    _app_state.last_rssi_reading = rtc6715_read_rssi(&_rtc);
    _app_state.last_read_channel = current_channel;
    channel_display_update_channel(
      current_channel,
      _app_state.last_rssi_reading,
      &_channels
      );
    notifyMainTask(READER_TASK_WAKEUP_FLAG);
    // switch channel to the next one after reading
    // so we get the maximum of stabilisation time.
    current_channel = (current_channel + 1) % CHANNEL_NUM;
    rtc6715_select_channel(&_rtc, current_channel);
  }
}


void Scanner::input(input_t button)
{
  switch(button)
  {
  case input_t::RIGHT_BUTTON:
    channel_display_step_cursor(&_channels, 1);
    ble_update(NOTIFY_CURRENT_CHANNEL);
    break;
  case input_t::LEFT_BUTTON:
    channel_display_step_cursor(&_channels, -1);
    ble_update(NOTIFY_CURRENT_CHANNEL);
    break;
  default:
    break;
  }
}
