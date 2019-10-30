// Copyright: 2019, Diez Roggisch, Berlin, all rights reserved
#include "peak_detection_strategy.hh"

#include <esp_log.h>

namespace  {

bool is_above_threshold(uint16_t rssi, const uint16_t& max_rssi, int threshold)
{
  return rssi > (max_rssi * threshold / 100);
}

}  // end ns anonymous

PeakDetector::PeakDetector(callback_t callback, peak_detection_t& config, uint16_t& max_rssi_reading)
  : _callback(callback)
  , _config(config)
  , _max_rssi_reading(max_rssi_reading)
{
  reset();
}


void PeakDetector::reset()
{
  _state = DETECTING_PEAK;
}


void PeakDetector::feed(ts_t now, uint16_t rssi)
{
  const auto old_state = _state;
  switch(_state)
  {
  case DETECTING_PEAK:
    {
      _state = state_detecting_peak(now, rssi);
    }
    break;
  case COOLDOWN:
    _state = state_cooldown(now, rssi);
    break;
  case LAPTIME:
    ESP_LOGE("pdetect", "Wrong state LAPTIME");
    break;
  }
  if(old_state != _state)
  {
    _callback(_state, now);
  }
}


PeakDetector::state_t PeakDetector::state_detecting_peak(ts_t now, uint16_t rssi)
{
  const bool above_threshold = is_above_threshold(
    rssi,
    _max_rssi_reading,
    _config.trigger_threshold_percent
    );
  if(above_threshold)
  {
    _peak_start = now;
    // Trigger recording of the laptime
    _callback(LAPTIME, now);
    return COOLDOWN;
  }
  return DETECTING_PEAK;
}


PeakDetector::state_t PeakDetector::state_cooldown(ts_t now, uint16_t)
{
  const auto diff = (now - _peak_start) / 1000000;
  if(diff < _config.laptime / 2)
  {
    return COOLDOWN;
  }
  return DETECTING_PEAK;
}
