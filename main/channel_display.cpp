// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "channel_display.hh"
#include "common.h"

namespace {

#include "cursor.xbm"

sprite_t cursor = {
  cursor_bits,
  cursor_width, cursor_height,
  1, -2
};

} // end ns anon

void channel_display_init(channel_display_t* channel_display, app_state_t* app_state)
{
  // we have 40 channels, and 128
  // pixels. Each channel gets 3
  // pixels, so we start at column
  // 4. As the xpos should be the middle,
  // it's actually 5
  for(int i=0; i < CHANNEL_NUM; ++i)
  {
    channel_display->channels[i].xpos = CHANNEL_LEFT + 1 + 3 * i;
    channel_display->channels[i].adc_value = 1;
    channel_display->channels[i].legal = i % 2;
  }
  channel_display->min = 4095;
  channel_display->max = 0;
  channel_display->app_state = app_state;
}


void channel_display_update_channel(int channel, int value, channel_display_t* channel_display)
{
  channel_display->channels[channel].adc_value = value;
  channel_display->min = min(channel_display->min, value);
  channel_display->max = max(channel_display->max, value);
  if(channel_display->min == channel_display->max)
  {
    ++channel_display->max;
  }
}


void channel_display_draw(Display& display, channel_display_t* channel_display)
{
  auto handle = display.handle();

  u8g2_SetDrawColor(handle, 1);
  for(int i=0; i < CHANNEL_NUM; ++i)
  {
    int height = CHANNELS_HEIGHT * \
    (channel_display->channels[i].adc_value - channel_display->min) \
       / (channel_display->max - channel_display->min);

    height = max(height, 1); // at least one pixel height

    u8g2_DrawFrame(
      handle,
      channel_display->channels[i].xpos - 1,
      CHANNELS_BOTTOM - height,
      3,
      height
      );
  }
  display.blit(cursor, 5 + 3 * channel_display->app_state->selected_channel, CHANNELS_BOTTOM);
}


void channel_display_step_cursor(channel_display_t* channel_display, int direction)
{
  channel_display->app_state->selected_channel = (
    // the '+ CHANNEL_NUM' is a trick to prevent
    // modulo from not doing it's job in case of
    // negative directions
    channel_display->app_state->selected_channel + direction + CHANNEL_NUM
    ) % CHANNEL_NUM;
}
