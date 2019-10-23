from contextlib import contextmanager
from objc import IBAction, IBOutlet, ivar, python_method

from Foundation import (
    NSObject,
    NSLog,
    )

from Cocoa import (
    NSColor,
    )

from VTXScanner import setup_bt_delegate, Mode

class ApplicationDelegate(NSObject):

    scanView = IBOutlet("scanView")
    laptimerView = IBOutlet("laptimerView")

    mode = ivar.int("mode")

    MODE2INDEX = {
        Mode.SCANNER: 0,
        Mode.LAPTIMER: 1,
    }

    NAME2MODE = {
        "scanner": Mode.SCANNER,
        "laptimer": Mode.LAPTIMER,
    }

    def init(self):
        self = super(ApplicationDelegate, self).init()
        self.colors = [NSColor.blackColor(), NSColor.redColor(), NSColor.greenColor()]
        self._allow_mode_change = False
        return self

    def applicationDidFinishLaunching_(self, _):
        NSLog("applicationDidFinishLaunching_")
        self.bt_manager, self.vtx_delegate = setup_bt_delegate()
        self.vtx_delegate.addListener_for_(
            self.modeChanged_,
            "mode",
        )
        self.vtx_delegate.addListener_for_(
            self.scanView.updateChannel_,
            "last_rssi",
        )
        self.vtx_delegate.addListener_for_(
            self.laptimerView.updateRssiValues_,
            "laptime_rssi",
        )

    @python_method
    @contextmanager
    def allow_mode_change(self):
        self._allow_mode_change = True
        try:
            yield
        finally:
            self._allow_mode_change = False

    def modeChanged_(self, delegate_mode):
        with self.allow_mode_change():
            delegate_mode = delegate_mode[0]
            mode = self.MODE2INDEX.get(delegate_mode, 0)
            self.mode = mode

    def applicationWillTerminate_(self, sender):
        pass

    def tabView_shouldSelectTabViewItem_(self, tabview, item):
        self.vtx_delegate.changeMode_(self.NAME2MODE[item.identifier()])
        return self._allow_mode_change
