// Copyright: 2019, Diez B. Roggisch, Berlin, all rights reserved
#pragma once

#include <stdint.h>
#include <vector>

using ts_t = int64_t;

struct laptime_t {
  uint16_t count;
  ts_t time;

  operator bool() const
  {
    return count != 0;
  }

  double seconds() const
  {
    double res = time;
    res /= 1000000;
    return res;
  }

};

class LapTimeTracker {

public:
  /**
   * Record a peak at the timestamp. Returns
   * true if this resulted in an actual laptime recorded.
   */
  bool record(ts_t);
  /**
   * Query a concrete laptime for a given count.
   * Here count can be 0 to get the *last* laptime.
   * The result will either be a proper laptime, or
   * also have a count and time of 0 to signify that there is
   * no laptime for that count (0 or otherwise).
   *
   * Negative numbers will yield laptimes from the back.
   * E.g. -1 will yield the second to last lap, etc.
   *
   * The operator bool() of the return-value will be false
   * if it's not a valid laptime.
   */
  laptime_t laptime(int count) const;
  void reset();

private:
  void trigger() const;

  std::vector<laptime_t> _laps;
  laptime_t _last_lap;

};
