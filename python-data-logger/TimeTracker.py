# -*- coding: utf-8 -*-
# Copyright: 2019, Diez B. Roggisch, Berlin . All rights reserved.
import time
import csv

from objc import python_method
from Foundation import (
    NSObject,
    NSTimer,
)

class TimeTracker(NSObject):

    FILTER_GAIN = 0.98

    def initWithVTXDelegate_(self, vtx_delegate):
        super().init()
        self._stats = []
        self._vtx_delegate = vtx_delegate
        vtx_delegate.addListener_for_(
            self.gotTimestamp_,
            "timestamp",
        )
        self._timestamp_timer = None
        self._t0 = None
        self._filtered_offset = None
        self._filtered_delay = None
        self._burst_count = 10
        return self

    def start(self):
        delay = 2.0 if self._burst_count <= 0 else 0.5
        self._timestamp_timer = NSTimer.scheduledTimerWithTimeInterval_target_selector_userInfo_repeats_(
            delay,
            self,
            self.timestampTimerCallback_,
            None,
            True,
        )

    def timestampTimerCallback_(self, _timer):
        print("timestampTimerCallback_")
        self._t0 = time.monotonic()
        self._vtx_delegate.readTimestamp()

    def gotTimestamp_(self, timestamp):
        timestamp = timestamp[0]
        if self._t0 is not None:
            t0 = self._t0
            t3 = time.monotonic()
            t1, t2 = timestamp, timestamp # always the same, as we only obtain one value
            print(t0, t1, t2, t3)
            offset = (t1 - t0) - (t2 - t3) / 2
            delay = (t3 - t0) - (t2 - t1)
            self._update(offset, delay)
            self._stats.append((offset, delay))
            self._t0 = None

        if self._burst_count > 0:
            self._burst_count -= 1
        print("gotTimestamp_", self._burst_count)

        if self._burst_count <= 0:
            self._burst_count = 0
            self._timestamp_timer.invalidate()
            self.start()


    def stop(self):
        self._timestamp_timer.invalidate()
        with open("/tmp/timer-stats.csv", "w") as outf:
            writer = csv.writer(outf)
            writer.writerows(self._stats)

    @python_method
    def _update(self, offset, delay):
        if self._filtered_delay is None:
            self._filtered_delay, self._filtered_offset = delay, offset
        else:
            self._filtered_delay += (delay - self._filtered_delay) * self.FILTER_GAIN
            self._filtered_offset += (offset - self._filtered_offset) * self.FILTER_GAIN
        print("filtered", self._filtered_offset, self._filtered_delay)
