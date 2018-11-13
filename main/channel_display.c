// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "channel_display.h"
#include "common.h"

#define CHANNELS_HEIGHT (64 - 10)
#define CHANNELS_BOTTOM (CHANNELS_HEIGHT + 3)

void channel_display_init(channel_display_t* channel_display)
{
  // we have 40 channels, and 128
  // pixels. Each channel gets 3
  // pixels, so we start at column
  // 4. As the xpos should be the middle,
  // it's actually 5
  for(int i=0; i < CHANNEL_NUM; ++i)
  {
    channel_display->channels[i].xpos = 5 + 3 * i;
    channel_display->channels[i].adc_value = 1;
  }
  channel_display->min = 4095;
  channel_display->max = 0;
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


void channel_display_draw(ssd1306_display_t* display, channel_display_t* channel_display)
{
  for(int i=0; i < CHANNEL_NUM; ++i)
  {
    int height = CHANNELS_HEIGHT * \
    (channel_display->channels[i].adc_value - channel_display->min) \
       / (channel_display->max - channel_display->min);

    height = max(height, 1); // at least one pixel height
    ssd1306_draw_vertical_line(
      display,
      channel_display->channels[i].xpos,
      CHANNELS_BOTTOM,
      CHANNELS_BOTTOM - height
      );
  }
}