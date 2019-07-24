// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "laptime_tracker.hh"

#include <esp_log.h>

#include <algorithm>

void LapTimeTracker::record(ts_t t)
{
  _laps.push_back({static_cast<uint16_t>(_laps.size() + 1), t});
  ESP_LOGI("lpt", "record laptime %i", _laps.size());
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
}
