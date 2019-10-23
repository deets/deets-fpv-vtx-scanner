// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "appstate.hh"

#include <driver/gpio.h>
#include <esp_timer.h>


class Buzzer {

public:
  Buzzer(app_state_t&, gpio_num_t pin);
  void buzz(int ms, int repetitions);

private:
  void timer_task();
  static void s_timer_task(void*);

  app_state_t& _app_state;
  gpio_num_t _pin;
  esp_timer_handle_t _buzzer_timer_handle;
  int _buzzer_repetitions;

};
