// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved

#include "rtc6715.h"

#include <driver/adc.h>

#include <stdint.h>

/*
 * These are all the known frequencies.
 */
static uint32_t frequency_table[] = {
  5645, 5658, 5665, 5685, 5695, 5705, 5725, 5732, 5733, 5740,
  5745, 5752, 5760, 5765, 5769, 5771, 5780, 5785, 5790, 5800,
  5805, 5806, 5809, 5820, 5825, 5828, 5840, 5843, 5845, 5847,
  5860, 5865, 5866, 5880, 5880, 5885, 5905, 5917, 5925, 5945
};

static uint32_t calc_register(uint32_t channel)
{
  uint32_t freq = frequency_table[channel];
  // calculatute F_LO
  freq = freq - 479;
  uint32_t n = freq >> 6;
  uint32_t a = (freq%64) >> 1;
  return a | (n << 7);
}


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

  uint32_t register_value = calc_register(channel);
  spi_transaction_t t;
  uint32_t cmd = (0x1 | 0x10) | (register_value << 5);
  t.length = 25;
  t.tx_buffer = (void*)cmd;
  t.flags = SPI_TRANS_USE_TXDATA;
  ret = spi_device_transmit(rtc->spi, &t);  //Transmit!
  ESP_ERROR_CHECK(ret);
}
