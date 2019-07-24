// Copyright: 2019, Diez Roggisch, Berlin, all rights reserved
#pragma once

#include "appstate.hh"

#include <functional>

class PeakDetector
{

public:

  enum state_t {
    BELOW_THRESHOLD,
    DETECTING_PEAK,
    PEAK
  };

  using callback_t = std::function<void(state_t, ts_t)>;


  PeakDetector(callback_t peak_callback, peak_detection_t& config, uint16_t& max_rssi_reading);

  void reset();
  void feed(ts_t now, uint16_t rssi);

private:
  state_t state_below_threshold(bool above_threshold, ts_t now, uint16_t rssi);
  state_t state_detecting_peak(bool above_threshold, ts_t now, uint16_t rssi);

  state_t _state;
  ts_t _peak_start;
  uint16_t _peak_reading;

  callback_t _callback;
  peak_detection_t& _config;
  uint16_t& _max_rssi_reading;
};
