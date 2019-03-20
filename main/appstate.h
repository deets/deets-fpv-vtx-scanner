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
  uint16_t min_rssi_reading;
  app_mode_t current_mode;
  uint16_t trigger_arm_threshold;
  uint16_t trigger_disarm_threshold;
  int64_t trigger_max_latency;
  int64_t trigger_cooldown;
  // laptimer data
  uint16_t laptime_buffer_pos;
} app_state_t;
