# -*- coding: utf-8 -*-
# Copyright: 2019, Diez B. Roggisch, Berlin . All rights reserved.
from objc import IBOutlet, python_method

from Cocoa import (
    NSTableRowView,
    NSColor,
    )


from .common import Mode

MODE2COLOR = {
    None: NSColor.colorWithRed_green_blue_alpha_(0, 0, 0, 0),
    Mode.SPLASH_SCREEN: NSColor.colorWithRed_green_blue_alpha_(
        76 / 255, 6 / 255, 29 / 255, 1
    ),
    Mode.SCANNER: NSColor.colorWithRed_green_blue_alpha_(
        180 / 255, 194 / 255, 146 / 255, 1
    ),
    Mode.LAPTIMER: NSColor.colorWithRed_green_blue_alpha_(
        115 / 255, 111 / 255, 78 / 255, 1
    ),
    Mode.SETTINGS: NSColor.colorWithRed_green_blue_alpha_(
        59 / 255, 57 / 255, 35 / 255, 1
    ),
}


class Connected:

    def __init__(self):
        self._connections = []

    def disconnect(self):
        [d.dispose() for d in self._connections]

    def connect(self, subject, callback):
        self._connections.append(
            subject.subscribe(callback)
        )


class ScannerListDetailView(NSTableRowView):

    name = IBOutlet("name")

    def bindScanner_(self, scanner):
        self.name.setStringValue_(scanner.name)


class ScannerListRowView(NSTableRowView, Connected):

    SELECTION_HIGHLIGHT = 0.2

    def initWithFrame_(self, frame):
        Connected.__init__(self)
        self._scanner = None
        return super().initWithFrame_(frame)

    def bindScanner_(self, scanner):
        self.disconnect()
        self._scanner = scanner
        self.connect(
            scanner.mode_subject,
            lambda mode: self._mode_changed(mode)
        )
        self._mode_changed(scanner.mode)

    @python_method
    def _mode_changed(self, mode):
        color = MODE2COLOR[mode].highlightWithLevel_(
            self.SELECTION_HIGHLIGHT if self.isSelected() else 0
        )
        self.setBackgroundColor_(color)

    def updateColor(self):
        self._mode_changed(self._scanner.mode)
