// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "ssd1306.h"

typedef struct {
  char* name;
  char* channel_names[40];
} goggle_info_t;

extern goggle_info_t aomway_commander_v1_info;

void goggle_display_draw(ssd1306_display_t *display, goggle_info_t* goggle, int cursor_pos);
