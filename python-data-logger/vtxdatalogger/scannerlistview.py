# -*- coding: utf-8 -*-
# Copyright: 2019, Diez B. Roggisch, Berlin . All rights reserved.
from objc import IBOutlet

from Cocoa import (
    NSTableRowView,
    NSBezierPath,
    NSColor,
    )


class ScannerListDetailView(NSTableRowView):
    BACKGROUND_COLOR = NSColor.redColor()

    name = IBOutlet("name")

    def bindScanner_(self, scanner):
        self.name.setStringValue_(scanner.name)


class ScannerListRowView(NSTableRowView):
    pass
