// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include <driver/spi_master.h>
#include <driver/adc.h>
#include <freertos/semphr.h>

class RTC6715 {

public:
  RTC6715(adc1_channel_t adc_channel, int cs, int clk, int mosi);
  RTC6715(const RTC6715&)=delete;

  int read_rssi();
  void select_channel(int channel);

  static int frequency_for_channel(int channel);

private:
  adc1_channel_t _adc_channel;
  spi_device_handle_t _spi;

  SemaphoreHandle_t _semaphore;
  StaticSemaphore_t _semaphore_buffer;
};
