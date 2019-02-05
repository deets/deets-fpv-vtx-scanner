// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include "scanner.h"

typedef struct {
  int current_channel; // The channel currently tuned
  uint16_t last_read_channel;
  uint16_t last_rssi_reading;
  scanner_state_t scanner_state;
} app_state_t;
