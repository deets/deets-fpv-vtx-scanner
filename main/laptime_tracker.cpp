// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved

#include "laptime_tracker.hh"
#include "ble.hh"

#include <esp_log.h>

#include <algorithm>

LapTimeTracker::LapTimeTracker(app_state_t& app_state, Buzzer& buzzer)
  : _app_state(app_state)
  , _buzzer(buzzer)
{

}


void LapTimeTracker::record(ts_t t)
{
  ESP_LOGI("lpt", "record peak %i", _laps.size());
  if(_last_lap && !race_over())
  {
    const auto laptime = t - _last_lap.time;
    _laps.push_back({static_cast<uint16_t>(_laps.size() + 1), laptime});
    if(race_over())
    {
      _buzzer.buzz(RACE_OVER);
    }
    else
    {
      _buzzer.buzz(LAP);
    }
    ble_notify(NOTIFY_NEW_LAPTIME);
    ESP_LOGI("lpt", "laptime %i", static_cast<int>(laptime));
  }
  _last_lap = { 1, t };
}


bool LapTimeTracker::race_over() const
{
  const auto laps = _app_state.peak_detection_config.laps;
  return laps != 0 && _laps.size() >= laps;
}


laptime_t LapTimeTracker::laptime_at(int count) const
{
  if(_laps.size())
  {
    if(count == 0)
    {
      return _laps.back();
    }
    if(count > 0)
    {
      const auto it = std::find_if(_laps.begin(), _laps.end(), [&count](const laptime_t& lap) { return lap.count == count; });
      if(it != _laps.end())
      {
        return *it;
      }
    }
    else // count < 0
    {
      const int index = _laps.size() - 1 + count;
      if(index >= 0)
      {
        return _laps[index];
      }
    }
  }
  return { 0, 0 };
}


void LapTimeTracker::reset()
{
  _laps.clear();
  _last_lap = { 0, 0 };
}
