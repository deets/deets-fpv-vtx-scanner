// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved

#include "rtc6715.h"

#include <driver/adc.h>

#include <stdint.h>

static uint32_t frequency_table[] = {
  0x2817, 0x281d, 0x2881, 0x288b, 0x2890, 0x2895,
  0x289f, 0x2902, 0x2903, 0x2906, 0x2909, 0x290c,
  0x2910, 0x2913, 0x2915, 0x2916, 0x291a, 0x291d,
  0x291f, 0x2984, 0x2987, 0x2987, 0x2989, 0x298e,
  0x2991, 0x2992, 0x2998, 0x299a, 0x299b, 0x299c,
  0x2a02, 0x2a05, 0x2a05, 0x2a0c, 0x2a0c, 0x2a0f,
  0x2a19, 0x2a1f, 0x2a83, 0x2a8d };

int rtc6715_setup(rtc6715_t* rtc, int adc_channel, int cs, int clk, int mosi)
{
  esp_err_t ret;

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(adc_channel, ADC_ATTEN_11db);
  rtc->adc_channel = adc_channel;

  // setup spi
  spi_bus_config_t buscfg={
    .miso_io_num=-1,
    .mosi_io_num=mosi,
    .sclk_io_num=clk,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz=4
  };

  spi_device_interface_config_t devcfg={
    .clock_speed_hz=1000*1000,           //Clock out at 2 MHz
    .mode=0,                                //SPI mode 0
    .spics_io_num=cs,               //CS pin
    .queue_size=1,                          //We want to be able to queue 7 transactions at a time
    .flags=SPI_DEVICE_TXBIT_LSBFIRST
  };

  //Initialize the SPI bus
  ret = spi_bus_initialize(VSPI_HOST, &buscfg, 0);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_add_device(VSPI_HOST, &devcfg, &rtc->spi);
  ESP_ERROR_CHECK(ret);
  return 0;
}


int rtc6715_read_rssi(rtc6715_t* rtc)
{
  return adc1_get_raw(rtc->adc_channel);
}


void rtc6715_select_channel(rtc6715_t* rtc, int channel)
{
  esp_err_t ret;

  if(channel < 0 && channel >= 40)
  {
    return;
  }
  spi_transaction_t t;
  uint32_t cmd = (0x1 | 0x10) | (frequency_table[channel] << 5);
  t.length = 25;
  t.tx_buffer = (void*)cmd;
  t.flags = SPI_TRANS_USE_TXDATA;
  ret = spi_device_transmit(rtc->spi, &t);  //Transmit!
  ESP_ERROR_CHECK(ret);
}
