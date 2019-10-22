from Cocoa import (
    NSView,
    NSBezierPath,
    NSColor,
    )

from objc import IBOutlet

class LaptimerView(NSView):

    FILTER_GAIN = 0.01

    rssiValuesPerSecond = IBOutlet("rssiValuesPerSecond")

    def initWithFrame_(self, frame):
        self.backgroundColor = NSColor.blackColor()
        self.graphColor = NSColor.whiteColor()
        self._last_ts = None
        self._filtered_value_per_second = 1000.0
        return super(LaptimerView, self).initWithFrame_(frame)

    def drawRect_(self, rect):
        f = self.frame()
        f.origin.x = f.origin.y = 0.0
        self.backgroundColor.set()
        NSBezierPath.fillRect_(f)

    def updateRssiValues_(self, args):
        ts, values = args
        if self._last_ts is not None:
            self._filtered_value_per_second += (len(values) / (ts - self._last_ts) - self._filtered_value_per_second) * self.FILTER_GAIN
            self.rssiValuesPerSecond.setIntValue_(int(self._filtered_value_per_second))
        self._last_ts = ts
