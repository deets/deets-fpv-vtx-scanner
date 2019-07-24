// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved

#include "rtc6715.hh"

#include <stdint.h>

// This ensures waiting for the mutex will block
#if !defined(INCLUDE_vTaskSuspend) ||  INCLUDE_vTaskSuspend != 1
#error "We expect  INCLUDE_vTaskSuspend to be 1!"
#endif

namespace {
/*
 * These are all the known frequencies.
 */
uint32_t frequency_table[] = {
  5645, 5658, 5665, 5685, 5695, 5705, 5725, 5732, 5733, 5740,
  5745, 5752, 5760, 5765, 5769, 5771, 5780, 5785, 5790, 5800,
  5805, 5806, 5809, 5820, 5825, 5828, 5840, 5843, 5845, 5847,
  5860, 5865, 5866, 5880, 5880, 5885, 5905, 5917, 5925, 5945
};

const int SPI_SPEED = 1000*1000;


uint32_t calc_register(uint32_t channel)
{
  uint32_t freq = frequency_table[channel];
  // calculatute F_LO
  freq = freq - 479;
  uint32_t n = freq >> 6;
  uint32_t a = (freq%64) >> 1;
  return a | (n << 7);
}

} // end ns anonymous


RTC6715::RTC6715(adc1_channel_t adc_channel, int cs, int clk, int mosi)
  : _adc_channel(adc_channel)
{
  esp_err_t ret;

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(_adc_channel, ADC_ATTEN_11db);

  // setup spi
  spi_bus_config_t buscfg = {
    .mosi_io_num=mosi,
    .miso_io_num=-1,
    .sclk_io_num=clk,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz=0,
    .flags=0,
    .intr_flags=0
  };

  spi_device_interface_config_t devcfg = {
    .command_bits=0,
    .address_bits=0,
    .dummy_bits=0,
    .mode=0,
    .duty_cycle_pos=0,
    .cs_ena_pretrans=0,
    .cs_ena_posttrans=0,
    .clock_speed_hz=SPI_SPEED,
    .input_delay_ns=0,
    .spics_io_num=cs,
    .flags=SPI_DEVICE_TXBIT_LSBFIRST,
    .queue_size=1,
    .pre_cb=nullptr,
    .post_cb=nullptr
  };
  //Initialize the SPI bus
  ret = spi_bus_initialize(VSPI_HOST, &buscfg, 0);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_add_device(VSPI_HOST, &devcfg, &_spi);
  ESP_ERROR_CHECK(ret);
  _semaphore = xSemaphoreCreateMutexStatic(&_semaphore_buffer);
}


int RTC6715::read_rssi()
{
  return adc1_get_raw(_adc_channel);
}


void RTC6715::select_channel(int channel)
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
  t.rxlength = 25;
  t.tx_buffer = (void*)cmd;
  t.rx_buffer = 0;
  t.flags = SPI_TRANS_USE_TXDATA;
  xSemaphoreTake(_semaphore, portMAX_DELAY);
  ret = spi_device_transmit(_spi, &t);  // synchronous
  xSemaphoreGive(_semaphore);
  ESP_ERROR_CHECK(ret);
}


int RTC6715::frequency_for_channel(int channel)
{
  return frequency_table[channel];
}
