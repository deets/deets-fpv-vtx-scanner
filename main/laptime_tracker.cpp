// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "laptime_tracker.hh"

#include <esp_log.h>

#include <algorithm>

bool LapTimeTracker::record(ts_t t)
{
  bool res = false;
  ESP_LOGI("lpt", "record peak %i", _laps.size());
  if(_last_lap)
  {
    const auto laptime = t - _last_lap.time;
    ESP_LOGI("lpt", "laptime %i", static_cast<int>(laptime));
    _laps.push_back({static_cast<uint16_t>(_laps.size() + 1), laptime});
    res = true;
  }
  _last_lap = { 1, t };
  return res;
}


laptime_t LapTimeTracker::laptime(uint16_t count) const
{
  if(_laps.size())
  {
    if(count == 0)
    {
      return _laps.back();
    }
    const auto it = std::find_if(_laps.begin(), _laps.end(), [&count](const laptime_t& lap) { return lap.count == count; });
    if(it != _laps.end())
    {
      return *it;
    }
  }
  return { 0, 0 };
}


void LapTimeTracker::reset()
{
  _laps.clear();
  _last_lap = { 0, 0 };
}
