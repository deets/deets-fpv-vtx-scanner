#include "mode.hh"

#include "ble.hh"

#include <esp_log.h>

void Mode::s_periodic_task_callback(void* data)
{
  ((Mode*)data)->periodic_task_callback();
}


Mode::Mode(app_state_t& app_state)
  : _app_state(app_state)
{
  _main_task_handle = xTaskGetCurrentTaskHandle();
  _periodic_task_handle = xTaskCreateStatic(
    Mode::s_periodic_task_callback,       /* Function that implements the task. */
    "PER",          /* Text name for the task. */
    PERIODIC_TASK_STACK_SIZE,      /* Number of indexes in the xStack array. */
    ( void * )this,    /* Parameter passed into the task. */
    tskIDLE_PRIORITY,/* Priority at which the task is created. */
    _periodic_task_stack,          /* Array to use as the task's stack. */
    &_periodic_task_buffer
    );
  periodic(0);
}


void Mode::periodic(TickType_t period)
{
  // always suspend to allow for
  // several periodic-calls with period > 0 in a row
  vTaskSuspend(_periodic_task_handle );
  if(period)
  {
    ESP_LOGD("mode", "periodic task period: %ims", period);
    _wake_period = period;
    _periodic_start = _last_wake_time = xTaskGetTickCount();
    vTaskResume(_periodic_task_handle);
  }
}

void Mode::periodic_task_callback()
{
  while(true)
  {
    vTaskDelayUntil( &_last_wake_time, _wake_period );
    xTaskNotify(
      _main_task_handle,
      PERIODIC_TASK_WAKEUP_FLAG,
      eSetBits
      );
  }
}


int Mode::total_elapsed_ms() const
{
  return (_last_wake_time - _periodic_start) * 1000 / configTICK_RATE_HZ;
}


int Mode::now() const
{
  return xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
}


void Mode::notifyMainTask(uint32_t flags)
{
  xTaskNotify(
    _main_task_handle,
    flags,
    eSetBits
    );
}


ModeManager::ModeManager(app_state_t& app_state, app_mode_t start, std::unique_ptr<Mode> mode)
  : _app_state(app_state)
{
  add_mode(start, std::move(mode));
  active().setup();
  ble_update(NOTIFY_CURRENT_MODE);
}


Mode& ModeManager::active()
{
  return *_modes.at(_app_state.current_mode);
}


const Mode& ModeManager::active() const
{
   return *_modes.at(_app_state.current_mode);
}


void  ModeManager::add_mode(app_mode_t mode, std::unique_ptr<Mode> mode_object)
{
  if(_modes.count(mode))
  {
    ESP_LOGE("mode", "Tried to register mode %i several times!", mode);
    abort();
  }
  _modes.insert(std::make_pair(mode, std::move(mode_object)));
}


void ModeManager::change_active_mode(int mode)
{
  // cleaning the mode by explicit
  // checking the allowed range
  bool valid = false;
  switch(mode)
      {
      case SPLASH_SCREEN:
      case SCANNER:
      case LAPTIMER:
        valid = true;
        break;
      }
  if(!valid)
  {
    return;
  }
  change_active_mode((app_mode_t)mode);
}


void ModeManager::change_active_mode(app_mode_t next_mode)
{
  if(next_mode != _app_state.current_mode)
  {
    active().teardown();
    _app_state.current_mode = next_mode;
    active().setup();
    ble_update(NOTIFY_CURRENT_MODE);
  }
}


void ModeManager::input(input_t inp)
{
  switch(inp)
  {
  case RIGHT_BUTTON:
  case LEFT_BUTTON:
    active().input(inp);
    break;
  case MODE_BUTTON:
    // this defenis the implicit model cycling order
    switch(_app_state.current_mode)
    {
    case SCANNER:
      change_active_mode(LAPTIMER);
      break;
    case LAPTIMER:
      change_active_mode(SCANNER);
      break;
    case SPLASH_SCREEN:
      break;
    }
  case SETTINGS_BUTTON:
    break;
  }
}


void ModeManager::update(Display& display)
{
  change_active_mode(active().update(display));
}
