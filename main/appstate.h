// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include <stdint.h>

typedef enum {
    SPLASH_SCREEN,
    SCANNER,
    LAPTIMER
} app_mode_t;

typedef struct {
  uint32_t selected_channel;
  uint16_t last_read_channel;
  uint16_t last_rssi_reading;
  uint16_t max_rssi_reading;
  app_mode_t current_mode;
} app_state_t;
