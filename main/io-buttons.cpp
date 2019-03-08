#include "io-buttons.hh"

#include "mode.hh"

#include <soc/gpio_struct.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include <unordered_map>
#include <stdint.h>

#define GPIO_DOWN GPIO_NUM_21
#define GPIO_UP GPIO_NUM_15
#define GPIO_LEFT GPIO_NUM_17
#define GPIO_RIGHT GPIO_NUM_4

namespace
{

const int DEBOUNCE = (200 * 1000);

TaskHandle_t s_main_task_handle;

std::unordered_map<int, uint64_t> s_debounces = {
  { GPIO_DOWN, 200 * 1000},
  { GPIO_UP, 200 * 1000},
  { GPIO_LEFT, 20 * 1000},
  { GPIO_RIGHT, 20 * 1000},
};

std::unordered_map<int, uint64_t> s_last;

void IRAM_ATTR gpio_isr_handler(void* arg)
{
  BaseType_t higher_prio_has_woken;
  int pin = (int)arg;
  int bit = 0;
  int64_t ts = esp_timer_get_time();
  if(s_last.count(pin) && s_last[pin] + s_debounces[pin] > ts)
  {
    return;
  }
  s_last[pin] = ts;

  switch(pin)
  {
  case GPIO_RIGHT:
    bit = RIGHT_PIN_ISR_FLAG;
    break;
  case GPIO_LEFT:
    bit = LEFT_PIN_ISR_FLAG;
    break;
  case GPIO_DOWN:
    bit = DOWN_PIN_ISR_FLAG;
    break;
  case GPIO_UP:
    bit = UP_PIN_ISR_FLAG;
    break;
  }

  xTaskNotifyFromISR(
    s_main_task_handle,
    bit,
    eSetBits,
    &higher_prio_has_woken
    );
  if(higher_prio_has_woken)
  {
    portYIELD_FROM_ISR();
  }
}
} // end ns anonymous

void iobuttons_setup(TaskHandle_t main_task_handle)
{
  s_main_task_handle = main_task_handle;

  gpio_config_t io_conf;
  //interrupt of rising edge
  io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_POSEDGE;
  io_conf.pin_bit_mask = \
  (1ULL<< GPIO_DOWN) |
  (1ULL<< GPIO_RIGHT) |
  (1ULL<< GPIO_UP) |
  (1ULL<< GPIO_LEFT);

  //set as input mode
  io_conf.mode = GPIO_MODE_INPUT;
  //enable pull-up mode
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);

  // install global GPIO ISR handler
  gpio_install_isr_service(0);
  // install individual interrupts
  gpio_isr_handler_add(GPIO_DOWN, gpio_isr_handler, (void*)GPIO_DOWN);
  gpio_isr_handler_add(GPIO_RIGHT, gpio_isr_handler, (void*)GPIO_RIGHT);
  gpio_isr_handler_add(GPIO_UP, gpio_isr_handler, (void*)GPIO_UP);
  gpio_isr_handler_add(GPIO_LEFT, gpio_isr_handler, (void*)GPIO_LEFT);
}
