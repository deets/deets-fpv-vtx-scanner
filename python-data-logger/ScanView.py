from Cocoa import (
    NSView,
    NSBezierPath,
    NSColor,
    )


class ScanView(NSView):

    CHANNELS = 40
    PADDING = 0.1

    def initWithFrame_(self, frame):
        self.backgroundColor = NSColor.blackColor()
        self.barColor = NSColor.whiteColor()
        self.channelValues = [0] * self.CHANNELS
        return super(ScanView, self).initWithFrame_(frame)

    def drawRect_(self, rect):
        f = self.frame()
        f.origin.x = f.origin.y = 0.0
        self.backgroundColor.set()
        NSBezierPath.fillRect_(f)
        padding_pixels = int(self.PADDING * f.size.width / (self.CHANNELS + 1))
        bar_width = int((f.size.width - (self.CHANNELS + 1) * padding_pixels) / self.CHANNELS)
        vstep = f.size.height / 4096
        self.barColor.set()
        left_padding = padding_pixels + (f.size.width - (bar_width + padding_pixels) * self.CHANNELS) // 2
        for i, value in enumerate(self.channelValues):
            x = i * (bar_width + padding_pixels) + left_padding
            y = 0
            h = int(value * vstep)
            r = ((x, y), (bar_width, h))
            NSBezierPath.fillRect_(r)

    def updateChannel_(self, args):
        channel, value = args
        if self.channelValues[channel] != value:
            self.channelValues[channel] = value
            self.setNeedsDisplay_(True)

    def reset(self):
        self.channelValues = [0] * self.CHANNELS
        self.setNeedsDisplay_(True)
