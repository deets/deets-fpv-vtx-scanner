// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "ssd1306.h"

#include <freertos/task.h>

#define PERIODIC_TASK_STACK_SIZE 500
#define READER_TASK_WAKEUP_FLAG (1 << 0)
#define RIGHT_PIN_ISR_FLAG (1 << 1)
#define LEFT_PIN_ISR_FLAG (1 << 2)
#define PERIODIC_TASK_WAKEUP_FLAG (1 << 3)

class Mode
{
public:
  Mode();
  virtual void setup()=0;
  virtual void update(ssd1306_display_t*)=0;
  virtual void teardown()=0;

  // Ensures peridodic wakeup
  // Compute argument like this:
  // pdMS_TO_TICKS(1000 / <FREQUENCY>)
  void periodic(TickType_t);

private:

  TaskHandle_t _main_task_handle;

  TickType_t _last_wake_time;
  TickType_t _wake_period;

  TaskHandle_t _periodic_task_handle;
  StaticTask_t _periodic_task_buffer;
  StackType_t  _periodic_task_stack[PERIODIC_TASK_STACK_SIZE];

  void periodic_task_callback();

  static void s_periodic_task_callback(void*);
};
