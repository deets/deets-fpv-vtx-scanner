// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "driver/spi_master.h"
#include <stdint.h>

typedef struct {
  uint8_t* frame;
  int dc_pin;
  int rst_pin;
  int dc_level;
  int width, height;
  int frame_byte_size;
  spi_device_handle_t spi;
} ssd1306_display_t;


int ssd1306_init_static(
  ssd1306_display_t *ssd1306_display,
  int cs, int clk, int mosi, int miso,
  int dc, int rst
  );

void ssd1306_update(ssd1306_display_t *display);

void ssd1306_clear(ssd1306_display_t *display);

void ssd1306_draw_pixel(ssd1306_display_t *display, int x, int y);
