#include "mode.hh"

void Mode::s_periodic_task_callback(void* data)
{
  ((Mode*)data)->periodic_task_callback();
}


Mode::Mode()
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
  if(period)
  {
    _wake_period = period;
    _periodic_start = _last_wake_time = xTaskGetTickCount();
    vTaskResume(_periodic_task_handle);
  }
  else
  {
    vTaskSuspend(_periodic_task_handle );
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
