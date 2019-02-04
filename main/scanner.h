// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "vtx-info.h"
#include "channel_display.h"
#include "vtx-info.h"
#include "goggle_info.h"

typedef struct {
  channel_display_t channels;
  vtx_info_t* selected_vtx;
  goggle_info_t* selected_goggle;
  int has_ham; // are we ham-licensed?
} scanner_state_t;
