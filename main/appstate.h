// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include <stdint.h>

typedef struct {
  int selected_channel;
  uint16_t last_read_channel;
  uint16_t last_rssi_reading;
} app_state_t;
