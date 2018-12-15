// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved

#pragma once

#include "ssd1306.h"

#define CHANNEL_NUM 40
#define CHANNELS_HEIGHT (64 - 13)
#define CHANNELS_BOTTOM (CHANNELS_HEIGHT)
#define CHANNEL_LEFT 4

typedef struct {
  int xpos;
  int adc_value;
  int legal;
} channel_reading_t;

typedef struct {
  channel_reading_t channels[40];
  int min;
  int max;
  int cursor_pos;
} channel_display_t;


void channel_display_init(channel_display_t* channel_display);
void channel_display_update_channel(int channel, int value, channel_display_t* channel_display);
void channel_display_draw(ssd1306_display_t* display, channel_display_t* channels_display);
void channel_display_step_cursor(channel_display_t* channels_display, int direction);
