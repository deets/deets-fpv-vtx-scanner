// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once
#include "appstate.hh"

#include <vector>

struct laptime_t {
  uint16_t count;
  ts_t time;

  operator bool() const
  {
    return count != 0;
  }

};

class LapTimeTracker {

public:
  /**
   * Record a lap at the timestamp.
   */
  void record(ts_t);
  /**
   * Query a concrete laptime for a given count.
   * Here count can be 0 to get the *last* laptime.
   * The result will either be a proper laptime, or
   * also have a count and time of 0 to signify that there is
   * no laptime for that count (0 or otherwise).
   */
  laptime_t laptime(uint16_t count) const;
  void reset();

private:

  std::vector<laptime_t> _laps;
};
