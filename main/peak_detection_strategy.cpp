// Copyright: 2019, Diez Roggisch, Berlin, all rights reserved
#include "peak_detection_strategy.hh"

#include <esp_log.h>

namespace  {

bool is_above_threshold(uint16_t rssi, const uint16_t& max_rssi, int threshold)
{
  return rssi > (max_rssi * threshold / 100);
}

bool is_large_enough_peak(ts_t peak_start, ts_t now, ts_t peak_size)
{
  return (now - peak_start) / 1000 /*  ms */ >= peak_size;
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
  _state = BELOW_THRESHOLD;
}


void PeakDetector::feed(ts_t now, uint16_t rssi)
{
  const auto old_state = _state;
  switch(_state)
  {
  case BELOW_THRESHOLD:
    {
      const bool above_threshold = is_above_threshold(rssi, _max_rssi_reading, _config.trigger_threshold_percent);
      _state = state_below_threshold(above_threshold, now, rssi);
    }
    break;
  case DETECTING_PEAK:
    {
      const bool above_threshold = is_above_threshold(
        rssi,
        _max_rssi_reading,
        _config.trigger_threshold_percent + _config.trigger_threshold_hysteresis
        );
      _state = state_detecting_peak(above_threshold, now, rssi);
    }
    break;
  case COOLDOWN:
    _state = state_cooldown(true, now, rssi);
    break;
  case PEAK:
    assert(false); // this shouldn't happen, it's only for the purpose of communicating the peak
    break;
  }
  if(old_state != _state)
  {
    _callback(_state, now);
  }
}


PeakDetector::state_t PeakDetector::state_below_threshold(bool above_threshold, ts_t now, uint16_t rssi)
{
  if(above_threshold)
  {
    _peak_start = now;
    _peak_reading = rssi;
    return DETECTING_PEAK;
  }
  return BELOW_THRESHOLD;
}


PeakDetector::state_t PeakDetector::state_detecting_peak(bool above_threshold, ts_t now, uint16_t rssi)
{
  if(above_threshold)
  {
    // If we are getting a greater reading
    // than ever before - record that
    if(rssi > _peak_reading)
    {
      _peak_start = now;
      _peak_reading = rssi;
    }
    // as long as we are above the threshold, we continue to look for the ever higher peak.
    return DETECTING_PEAK;
  }
  else
  {
    // falling below the threshold
    // means we first have to validate that the
    // overall peak was large enough
    const bool large_enough_peak = is_large_enough_peak(_peak_start, now, _config.peak_size);
    if(large_enough_peak)
    {
      _callback(PEAK, _peak_start);
    }
    return BELOW_THRESHOLD;
  }
}

PeakDetector::state_t PeakDetector::state_cooldown(bool, ts_t now, uint16_t)
{
  if((now - _peak_start) / 1000 < _config.cooldown_period)
  {
    return COOLDOWN;
  }
  return BELOW_THRESHOLD;
}
