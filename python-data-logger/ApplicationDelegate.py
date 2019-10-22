from objc import IBAction, IBOutlet, ivar

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

    def init(self):
        self = super(ApplicationDelegate, self).init()
        self.colors = [NSColor.blackColor(), NSColor.redColor(), NSColor.greenColor()]
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

    def modeChanged_(self, delegate_mode):
        delegate_mode = delegate_mode[0]
        mode = self.MODE2INDEX.get(delegate_mode, 0)
        self.mode = mode

    def applicationWillTerminate_(self, sender):
        pass
