import datetime
import pathlib
import threading
import queue
import csv

from Cocoa import (
    NSView,
    NSBezierPath,
    NSColor,
    )

from objc import IBOutlet, IBAction, python_method

class Capture:

    BASE = pathlib.Path("/tmp")

    def __init__(self):
        fname = self.BASE / "vtx-scanner-log-{}.csv".format(datetime.datetime.now().isoformat().replace(":", "-"))
        self._f = fname.open("w")
        self._q = queue.Queue()
        self._t = threading.Thread(target=self._run)
        self._t.start()

    def stop(self):
        self._q.put(None)
        self._t.join()

    def feed(self, samples):
        self._q.put(samples)

    @python_method
    def _run(self):
        while True:
            data = self._q.get()
            if data is None:
                break
            for value in data:
                self._f.write(f"{value}\n")
        self._f.close()


class LaptimerView(NSView):

    FILTER_GAIN = 0.005
    RETAINED_SAMPLES = 2000

    rssiValuesPerSecond = IBOutlet("rssiValuesPerSecond")
    currentRSSI = IBOutlet("currentRSSI")


    def initWithFrame_(self, frame):
        self.backgroundColor = NSColor.blackColor()
        self.graphColor = NSColor.whiteColor()
        self._last_ts = None
        self._last_display = None
        self._rssi_values = []
        self._capture = None
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
            self.currentRSSI.setIntValue_(v)

        self.rssiValuesPerSecond.setIntValue_(int(self._filtered_value_per_second))

    def updateRssiValues_(self, args):
        ts, values = args
        if self._last_ts is not None:
            elapsed = ts - self._last_ts
            self._filtered_value_per_second += (len(values) / elapsed - self._filtered_value_per_second) * self.FILTER_GAIN
            if self._last_display is None or (ts - self._last_display) > 1/30.0:
                self.setNeedsDisplay_(True)
                self._last_display = ts

        self._last_ts = ts
        self._rssi_values.extend(values)
        self._rssi_values[:-self.RETAINED_SAMPLES] = []
        if self._capture:
            self._capture.feed(values)

    @IBAction
    def captureLog_(self, sender):
        if self._capture is None:
            self._capture = Capture()
            sender.setTitle_("Stop")
        else:
            self._capture.stop()
            self._capture = None
            sender.setTitle_("Capture Log")
