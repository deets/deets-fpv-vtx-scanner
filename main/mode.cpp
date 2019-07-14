#include "mode.hh"

#include "ble.hh"

#include <esp_log.h>

Mode::Mode(app_state_t& app_state)
  : _app_state(app_state)
{
  esp_timer_create_args_t timer_args = {
    .callback = &Mode::s_periodic_timer_callback,
    .arg=this,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "periodic_timer"
  };
  auto err = esp_timer_create(
    &timer_args,
    &_periodic_timer_handle
    );
  assert(err == ESP_OK);
  _main_task_handle = xTaskGetCurrentTaskHandle();
}


void Mode::s_periodic_timer_callback(void* data)
{
  ((Mode*)data)->notify_main_task(READER_TASK_WAKEUP_FLAG);
}


void Mode::notify_main_task(uint32_t flags)
{
  xTaskNotify(
    _main_task_handle,
    flags,
    eSetBits
    );
}


void Mode::periodic(int ms)
{
  if(ms == 0)
  {
    esp_timer_stop(_periodic_timer_handle);
  }
  else
  {
    esp_timer_start_periodic(_periodic_timer_handle, ms * 1000);
  }
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


void Mode::setup()
{
  setup_impl();
}


void Mode::teardown()
{
  teardown_impl();
  periodic(0);
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
