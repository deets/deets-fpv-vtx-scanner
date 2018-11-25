// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#include "channel_display.h"
#include "common.h"

#define CHANNELS_HEIGHT (64 - 10)
#define CHANNELS_BOTTOM (CHANNELS_HEIGHT)

static uint32_t cursor_image[] = {
  0b010,
  0b111,
};

static sprite_t cursor = {
  .height=2,
  .hotspot_x=1,
  .hotspot_y=-2,
  .image_modulo=0,
  .mask_modulo=0,
  .image=cursor_image,
  .mask=cursor_image
};


static uint32_t filled_row_image[] = {
  0b111,
};

static uint32_t hollow_row_image[] = {
  0b101,
};

static sprite_t filled_row = {
  .height=1,
  .hotspot_x=1,
  .hotspot_y=0,
  .image_modulo=-1,
  .mask_modulo=-1,
  .image=filled_row_image,
  .mask=filled_row_image
};


static sprite_t hollow_row = {
  .height=1,
  .hotspot_x=1,
  .hotspot_y=0,
  .image_modulo=-1,
  .mask_modulo=-1,
  .image=hollow_row_image,
  .mask=filled_row_image
};


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
  channel_display->cursor_pos = 20;
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

    // nasty trick - by modifying the
    // sprite height, we blit the full
    // height of the column
    filled_row.height = height;
    ssd1306_blit(
      display,
      &filled_row,
      channel_display->channels[i].xpos,
      CHANNELS_BOTTOM - height
      );
  }
  ssd1306_blit(display, &cursor, 5 + 3 * channel_display->cursor_pos, CHANNELS_BOTTOM);
}


void channel_display_step_cursor(channel_display_t* channel_display)
{
  channel_display->cursor_pos = (channel_display->cursor_pos + 1) % CHANNEL_NUM;
}
