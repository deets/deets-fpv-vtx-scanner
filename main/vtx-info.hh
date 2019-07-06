// Copyright: 2018, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "display.hh"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LEGAL,
  HAM,
  FORBIDDEN
} channel_type_t;

typedef struct {
  const char* name;
  const char* channel_names[40];
  channel_type_t channel_legal[40];
} vtx_info_t;

extern vtx_info_t tbs_unify_info;

void vtx_display_draw(Display& display, vtx_info_t* vtx, int cursor_pos);

#ifdef __cplusplus
}
#endif
