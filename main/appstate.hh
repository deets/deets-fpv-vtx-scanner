// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include <stdint.h>
#include <vector>

enum app_mode_t
{
    SPLASH_SCREEN,
    SCANNER,
    LAPTIMER
};

struct app_state_t
{
  app_state_t()
    : selected_channel(0)
    , last_read_channel(0)
    , last_rssi_reading(0)
    , max_rssi_reading(0)
    , min_rssi_reading(8191)
    , current_mode(SPLASH_SCREEN)
    , trigger_arm_threshold(0)
    , trigger_disarm_threshold(0)
    , trigger_max_latency(0)
    , trigger_cooldown(0)
    , laptime_buffer_pos(0)
  {}
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
  std::vector<uint8_t> laptime_buffer;
};
