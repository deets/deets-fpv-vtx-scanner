// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include <driver/gpio.h>


void buzzer_setup(gpio_num_t pin);

void buzzer_buzz(int ms, int repetitions);
