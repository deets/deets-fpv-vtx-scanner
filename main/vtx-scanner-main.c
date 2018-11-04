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
  int x, y, z;
} star_t;


star_t stars[STAR_COUNT];

int random(int start, int end)
{
  int r = esp_random();
  r = r % (end - start);
  r += start;
  return r;
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

  int x = 0;
  int y = 0;
  int z = 0;

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);

  for(int i=0; i < STAR_COUNT; ++i)
  {
    stars[i].x = random(-1024, 1024);
    stars[i].y = random(-1024, 1024);
    stars[i].z = random(0, STAR_DISTANCE) + STAR_DISTANCE;
  }
  while(1)
  {
    ssd1306_clear(&display);
    for(int i=0; i < STAR_COUNT;++i)
    {
      x = stars[i].x * STAR_FACTOR / stars[i].z + (display.width / 2);
      y = stars[i].y * STAR_FACTOR / stars[i].z + (display.height / 2);
      ssd1306_draw_pixel(&display, x, y);
    }

    /* frame[z] = 0x1; */
    /* z = (z + 1) % sizeof(frame); */
    ssd1306_update(&display);
    // Read ADC and obtain result in mV
    int voltage = adc1_get_raw(ADC1_CHANNEL_6);
    printf("%d mV\n",voltage);
    //vTaskDelay(10);
    for(int i=0; i < STAR_COUNT; ++i)
    {
      stars[i].z -= STAR_SPEED;
      if(stars[i].z <= 0)
      {
        stars[i].x = random(-1024, 1024);
        stars[i].y = random(-1024, 1024);
        stars[i].z = random(0, STAR_DISTANCE) + STAR_DISTANCE;
      }
    }
  }
}
