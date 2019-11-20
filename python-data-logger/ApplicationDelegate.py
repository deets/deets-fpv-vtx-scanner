from contextlib import contextmanager
from objc import IBOutlet, ivar, python_method

from Foundation import (
    NSObject,
    NSLog,
    )

from AppKit import (
    NSApplication,
    )
from Cocoa import (
    NSColor,
    )

# these imports are needed to hook the created
# classes into the Objective C runtime, so later
# on they are found when NIB files refer to them
from VTXScanner import setup_bt_delegate, Mode
from vtxdatalogger import ScannerListDetailView
from TimeTracker import TimeTracker


class ApplicationDelegate(NSObject):

    scanView = IBOutlet("scanView")
    laptimerView = IBOutlet("laptimerView")
    scannerListView = IBOutlet("scannerListView")

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
        self._bt_delegate = None
        self._time_tracker = None
        self._rx_connections = []
        self._bound_scanner = None
        return self

    def applicationDidFinishLaunching_(self, _):
        NSLog("applicationDidFinishLaunching_")
        self._bt_delegate = setup_bt_delegate()
        self._bt_delegate.scanner_connected.subscribe(self._scanner_connected)
        # self._time_tracker = TimeTracker.alloc().initWithVTXDelegate_(self.vtx_delegate)
        # self._time_tracker.start()

    @python_method
    def _scanner_connected(self, scanner):
        self.scannerListView.reloadData()

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
            mode = self.MODE2INDEX.get(delegate_mode, 0)
            self.mode = mode

    def applicationWillTerminate_(self, sender):
        #self._time_tracker.stop()
        pass

    def tabView_shouldSelectTabViewItem_(self, tabview, item):
        if self._bound_scanner is not None:
            self._bound_scanner.changeMode_(self.NAME2MODE[item.identifier()])
            return self._allow_mode_change
        else:
            return True

    # NSTableViewDataSource
    def numberOfRowsInTableView_(self, table_view):
        print("numberOfRowsInTableView_", table_view)
        return len(self._bt_delegate) if self._bt_delegate is not None \
            else 0

    # NSTableViewDelegate
    def tableView_viewForTableColumn_row_(self, table_view, column, row):
        identifier = column.identifier()
        if column and identifier == "scanner_name":
            cell = table_view.makeViewWithIdentifier_owner_(identifier, self)
            cell.bindScanner_(self._bt_delegate[row])
            return cell

    def tableViewSelectionDidChange_(self, notification):
        table_view = notification.object()
        selected_rows = list(table_view.selectedRowIndexes())
        assert len(selected_rows) <= 1
        scanner = self._bt_delegate[selected_rows[0]] \
            if len(selected_rows) == 1 else None
        self._bind_scanner(scanner)

    @python_method
    def _bind_scanner(self, scanner):
        [d.dispose() for d in self._rx_connections]
        if scanner is not None:
            self._rx_connections.append(
                scanner.last_rssi_subject.subscribe(
                    lambda last_rssi: self.scanView.updateChannel_(last_rssi)
                )
            )
            self._rx_connections.append(
                scanner.mode_subject.subscribe(
                    self.modeChanged_,
                )
            )
            self._rx_connections.append(
                scanner.laptime_rssi_subject.subscribe(
                    self.laptimerView.updateRssiValues_
                )
            )
            self.modeChanged_(scanner.mode)
        else:
            self.scanView.reset()
        self._bound_scanner = scanner

    def windowWillClose_(self, _):
        NSApplication.sharedApplication().terminate_(self)
