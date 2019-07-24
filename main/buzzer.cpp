#include "buzzer.hh"

#include <esp_timer.h>


namespace {

gpio_num_t s_pin;

esp_timer_handle_t buzzer_timer_handle = NULL;

int buzzer_repetitions;

void buzzer_timer_task(void*)
{
  if(!buzzer_repetitions)
  {
    gpio_set_level(s_pin, 0);
    esp_timer_stop(buzzer_timer_handle);
  }
  else
  {
    gpio_set_level(s_pin, (buzzer_repetitions % 2) == 0);
    --buzzer_repetitions;
  }
}


} // end ns anonymous

void buzzer_setup(gpio_num_t pin)
{
  s_pin = pin;
  gpio_config_t io_conf = {
    .pin_bit_mask = 1ULL << s_pin,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en=GPIO_PULLUP_DISABLE,
    .pull_down_en=GPIO_PULLDOWN_DISABLE,
    .intr_type=GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf);

  esp_timer_create_args_t timer_args = {
    .callback = buzzer_timer_task,
    .arg=NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "buzzer_timer"
  };

  auto err = esp_timer_create(
    &timer_args,
    &buzzer_timer_handle
    );
  assert(err == ESP_OK);
}


void buzzer_buzz(int ms, int repetitions)
{
  esp_timer_stop(buzzer_timer_handle);
  gpio_set_level(s_pin, 1);
  buzzer_repetitions = repetitions * 2 - 1;
  esp_timer_start_periodic(buzzer_timer_handle, ms * 1000);
}
