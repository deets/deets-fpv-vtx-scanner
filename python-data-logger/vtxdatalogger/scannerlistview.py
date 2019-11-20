# -*- coding: utf-8 -*-
# Copyright: 2019, Diez B. Roggisch, Berlin . All rights reserved.
from objc import IBOutlet

from Cocoa import (
    NSTableCellView,
    NSBezierPath,
    NSColor,
    )


class ScannerListDetailView(NSTableCellView):
    BACKGROUND_COLOR = NSColor.greenColor()

    name = IBOutlet("name")

    def init(self):
        self._name = None
        return super().init()

    def drawRect_(self, rect):
        f = self.frame()
        f.origin.x = f.origin.y = 0.0
        self.BACKGROUND_COLOR.set()
        NSBezierPath.fillRect_(f)

    def bindScanner_(self, scanner):
        self.name.setStringValue_(scanner.name)
