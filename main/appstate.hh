// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "laptime_tracker.hh"

#include <vector>

enum app_mode_t
{
    SPLASH_SCREEN,
    SCANNER,
    LAPTIMER,
    SETTINGS
};

/**
 * This contains the data necessary to track and
 * detect a close flyby (based on RSSI levels)
 *
 * For a in detail description of the approach
 * and meaning of the values, see peak_detection_strategy.hh
 */
struct peak_detection_t
{
  int trigger_threshold_percent; // 1-100, percent
  int trigger_threshold_hysteresis; // 1-100, percent, added to trigger_threshold_percent
  ts_t peak_size; // 1-n, milliseconds.
  ts_t cooldown_period; // 0-n, milliseconds

  peak_detection_t()
    : trigger_threshold_percent(70)
    , trigger_threshold_hysteresis(-15)
    , peak_size(200)
    , cooldown_period(5000)
  {}
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
    , laptime_buffer(8192, 0)
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
  peak_detection_t peak_detection_config;
};
