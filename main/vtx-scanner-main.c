#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

#include "ssd1306.h"
#include "starscroller.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22
#define PIN_NUM_DC   16
#define PIN_NUM_RST  18

#define STAR_SPEED 50
#define STAR_FACTOR 1024
#define STAR_DISTANCE 8192
#define STAR_COUNT 300

#define READER_TASK_STACK_SIZE 2000
#define READS_PER_SECOND 20
#define READER_TASK_WAKUP_FLAG 1

typedef struct {
  int xpos;
  int adc_value;
} channel_reading_t;

typedef struct {
  int current_channel;
  channel_reading_t channels[40];

  TaskHandle_t display_task;
  TaskHandle_t reader_task_handle;
  StaticTask_t reader_task_buffer;
  StackType_t  reader_task_stack[READER_TASK_STACK_SIZE];
  spi_device_handle_t spi;
} vtx_scanner_t;


static vtx_scanner_t vtx_scanner;


#define CHANNELS_HEIGHT (64 - 10)
#define CHANNELS_BOTTOM (CHANNELS_HEIGHT + 3)

static uint32_t frequency_table[] = {
  0x2817, 0x281d, 0x2881, 0x288b, 0x2890, 0x2895,
  0x289f, 0x2902, 0x2903, 0x2906, 0x2909, 0x290c,
  0x2910, 0x2913, 0x2915, 0x2916, 0x291a, 0x291d,
  0x291f, 0x2984, 0x2987, 0x2987, 0x2989, 0x298e,
  0x2991, 0x2992, 0x2998, 0x299a, 0x299b, 0x299c,
  0x2a02, 0x2a05, 0x2a05, 0x2a0c, 0x2a0c, 0x2a0f,
  0x2a19, 0x2a1f, 0x2a83, 0x2a8d };

void reader_task(void* data)
{
  esp_err_t ret;
  TickType_t last_wake_time;
  const TickType_t frequency =  pdMS_TO_TICKS(1000 / READS_PER_SECOND);
  last_wake_time = xTaskGetTickCount ();

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  // setup spi
  spi_bus_config_t buscfg={
    .miso_io_num=-1,
    .mosi_io_num=12,
    .sclk_io_num=13,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz=4
  };

  spi_device_interface_config_t devcfg={
    .clock_speed_hz=1000*1000,           //Clock out at 2 MHz
    .mode=0,                                //SPI mode 0
    .spics_io_num=14,               //CS pin
    .queue_size=1,                          //We want to be able to queue 7 transactions at a time
    .flags=SPI_DEVICE_TXBIT_LSBFIRST
  };

  //Initialize the SPI bus
  ret = spi_bus_initialize(VSPI_HOST, &buscfg, 0);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_add_device(VSPI_HOST, &devcfg, &vtx_scanner.spi);
  ESP_ERROR_CHECK(ret);

  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, frequency );
    vtx_scanner.channels[vtx_scanner.current_channel].adc_value = adc1_get_raw(ADC1_CHANNEL_6);
    xTaskNotify(
      vtx_scanner.display_task,
      READER_TASK_WAKUP_FLAG,
      eSetBits
      );
    // switch channel
    vtx_scanner.current_channel = (vtx_scanner.current_channel + 1) % 40;
    spi_transaction_t t;
    uint32_t cmd = (0x1 | 0x10) | (frequency_table[vtx_scanner.current_channel] << 5);
    t.length = 25;
    t.tx_buffer = (void*)cmd;
    t.flags = SPI_TRANS_USE_TXDATA;
    ret = spi_device_transmit(vtx_scanner.spi, &t);  //Transmit!
    ESP_ERROR_CHECK(ret);
  }
}


void init_channels()
{
  vtx_scanner.display_task = xTaskGetCurrentTaskHandle();
  // we have 40 channels, and 128
  // pixels. Each channel gets 3
  // pixels, so we start at column
  // 4. As the xpos should be the middle,
  // it's actually 5
  for(int i=0; i < 40; ++i)
  {
    vtx_scanner.channels[i].xpos = 5 + 3 * i;
    vtx_scanner.channels[i].adc_value = i * 20;
  }
  vtx_scanner.current_channel = 0;
  vtx_scanner.reader_task_handle = xTaskCreateStatic(
                  reader_task,       // Function that implements the task.
                  "RTC",          // Text name for the task.
                  READER_TASK_STACK_SIZE,      // Stack size in bytes, not words.
                  &vtx_scanner,
                  tskIDLE_PRIORITY,// Priority at which the task is created.
                  vtx_scanner.reader_task_stack,          // Array to use as the task's stack.
                  &vtx_scanner.reader_task_buffer // Variable to hold the task's data structure.
    );
}

void draw_channels(ssd1306_display_t* display)
{
  for(int i=0; i < 40; ++i)
  {
    int height = CHANNELS_HEIGHT * vtx_scanner.channels[i].adc_value / 4095;
    ssd1306_draw_vertical_line(
      display,
      vtx_scanner.channels[i].xpos,
      CHANNELS_BOTTOM,
      CHANNELS_BOTTOM - height
      );
  }
}


void wait_for_notification()
{
  xTaskNotifyWait(
    pdFALSE,
    ULONG_MAX,
    NULL,
    portMAX_DELAY
    );
}


void app_main()
{
  ssd1306_display_t display;
  ssd1306_init_static(
    &display,
    PIN_NUM_CS,
    PIN_NUM_CLK,
    PIN_NUM_MOSI,
    PIN_NUM_MISO,
    PIN_NUM_DC,
    PIN_NUM_RST
    );

  init_channels();

  while(1)
  {
    wait_for_notification();
    ssd1306_clear(&display);
    draw_channels(&display);
    ssd1306_update(&display);
  }
}
