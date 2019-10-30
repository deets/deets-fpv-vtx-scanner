// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include <driver/spi_master.h>
#include <driver/adc.h>
#include <freertos/semphr.h>
#include <esp_adc_cal.h>


class RTC6715 {

public:
  RTC6715(adc1_channel_t adc_channel, int cs, int clk, int mosi);
  RTC6715(const RTC6715&)=delete;

  int read_rssi();
  void select_channel(int channel);
  void set_ifabf_gain(uint8_t);
  void set_ifaaf_gain(uint8_t);

  static int frequency_for_channel(int channel);

private:
  void write_register(uint8_t reg, uint32_t value);
  uint32_t read_register(uint8_t reg);

  adc1_channel_t _adc_channel;
  spi_device_handle_t _spi;

  SemaphoreHandle_t _semaphore;
  StaticSemaphore_t _semaphore_buffer;

  uint32_t _receiver_control_register_1;
  uint32_t _receiver_control_register_2;

  esp_adc_cal_characteristics_t _adc_chars;

};
