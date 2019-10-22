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
        self._rssi_values = []
        self._filtered_value_per_second = 1000.0

        return super(LaptimerView, self).initWithFrame_(frame)

    def drawRect_(self, rect):
        f = self.frame()
        f.origin.x = f.origin.y = 0.0
        self.backgroundColor.set()
        NSBezierPath.fillRect_(f)
        if self._rssi_values:
            self.graphColor.set()
            hstep = f.size.width / len(self._rssi_values)
            vstep = f.size.height / 4096
            path = NSBezierPath.bezierPath()
            x = 0.0
            path.moveToPoint_((0, int(self._rssi_values[0] * vstep)))
            for v in self._rssi_values[1:]:
                x += hstep
                path.lineToPoint_((x, int(v * vstep)))
            path.stroke()

    def updateRssiValues_(self, args):
        ts, values = args
        if self._last_ts is not None:
            self._filtered_value_per_second += (len(values) / (ts - self._last_ts) - self._filtered_value_per_second) * self.FILTER_GAIN
            self.rssiValuesPerSecond.setIntValue_(int(self._filtered_value_per_second))
        self._last_ts = ts
        self._rssi_values.extend(values)
        self._rssi_values[:-1000] = []
        self.setNeedsDisplay_(True)
