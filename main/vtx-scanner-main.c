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
} vtx_scanner_t;


static vtx_scanner_t vtx_scanner;


#define CHANNELS_HEIGHT (64 - 10)
#define CHANNELS_BOTTOM (CHANNELS_HEIGHT + 3)

void reader_task(void* data)
{
  TickType_t last_wake_time;
  const TickType_t frequency =  pdMS_TO_TICKS(1000 / READS_PER_SECOND);
  last_wake_time = xTaskGetTickCount ();

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  for( ;; )
  {
    vTaskDelayUntil( &last_wake_time, frequency );
    vtx_scanner.channels[vtx_scanner.current_channel].adc_value = adc1_get_raw(ADC1_CHANNEL_6);
    vtx_scanner.current_channel = (vtx_scanner.current_channel + 1) % 40;
    xTaskNotify(
      vtx_scanner.display_task,
      READER_TASK_WAKUP_FLAG,
      eSetBits
      );
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
