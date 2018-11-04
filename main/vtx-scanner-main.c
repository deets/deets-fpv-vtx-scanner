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

typedef struct {
  int xpos;
  int adc_value;
} channel_reading_t;

static channel_reading_t channels[40];
#define CHANNELS_HEIGHT (64 - 10)
#define CHANNELS_BOTTOM (CHANNELS_HEIGHT + 3)

void init_channels()
{
  // we have 40 channels, and 128
  // pixels. Each channel gets 3
  // pixels, so we start at column
  // 4. As the xpos should be the middle,
  // it's actually 5

  for(int i=0; i < 40; ++i)
  {
    channels[i].xpos = 5 + 3 * i;
  }
}

void draw_channels(ssd1306_display_t* display)
{
  for(int i=0; i < 40; ++i)
  {
    int height = CHANNELS_HEIGHT * channels[i].adc_value / 4095;
    ssd1306_draw_vertical_line(
      display,
      channels[i].xpos,
      CHANNELS_BOTTOM,
      CHANNELS_BOTTOM - height
      );
  }
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

  starscroller_handle_t* stars = starscroller_create(
    STAR_COUNT,
    STAR_DISTANCE,
    STAR_SPEED,
    STAR_FACTOR,
    128,
    64
    );

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  init_channels();
  int channel_pos = 0;

  while(1)
  {
    ssd1306_clear(&display);
    starscroller_step(stars);

    for(int i=0; i < STAR_COUNT; ++i)
    {
      ssd1306_draw_pixel(&display, stars->projections[i].x, stars->projections[i].y);
    }

    channels[channel_pos].adc_value = adc1_get_raw(ADC1_CHANNEL_6);
    channel_pos = (channel_pos + 1) % 40;

    draw_channels(&display);
    ssd1306_update(&display);
  }
}
