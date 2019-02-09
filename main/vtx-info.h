// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "ssd1306.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LEGAL,
  HAM,
  FORBIDDEN
} channel_type_t;

typedef struct {
  char* name;
  char* channel_names[40];
  channel_type_t channel_legal[40];
} vtx_info_t;

extern vtx_info_t tbs_unify_info;

void vtx_display_draw(ssd1306_display_t *display, vtx_info_t* vtx, int cursor_pos);

#ifdef __cplusplus
}
#endif
