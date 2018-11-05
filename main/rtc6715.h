// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "driver/spi_master.h"

typedef struct {
  int adc_channel;
  spi_device_handle_t spi;
} rtc6715_t;

int rtc6715_setup(rtc6715_t* rtc, int adc_channel, int cs, int clk, int mosi);

int rtc6715_read_rssi(rtc6715_t* rtc);

void rtc6715_select_channel(rtc6715_t* rtc, int channel);
