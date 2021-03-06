// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved

#pragma once

#include "appstate.hh"
#include "display.hh"

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
  app_state_t* app_state;
} channel_display_t;


void channel_display_init(channel_display_t* channel_display, app_state_t* app_state);
void channel_display_update_channel(int channel, int value, channel_display_t* channel_display);
void channel_display_draw(Display& display, channel_display_t* channels_display);
void channel_display_step_cursor(channel_display_t* channels_display, int direction);
