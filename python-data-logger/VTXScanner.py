import time
import struct
from enum import unique, IntEnum

import rx.subject
import objc
import libdispatch
from Foundation import NSObject

from vtxdatalogger.common import Mode

objc.loadBundle(
    "CoreBluetooth",
    globals(),
    bundle_path=objc.pathForFramework(u'/System/Library/Frameworks/IOBluetooth.framework/Versions/A/Frameworks/CoreBluetooth.framework')
)


VTX_SERVICE = CBUUID.UUIDWithString_(u'D27E29B4-4DBD-4103-A8B6-09301EFDDD01')
VTX_CURRENT_MODE = CBUUID.UUIDWithString_(u'903BB704-5ADC-48D3-B0D4-0628BDB35250')
VTX_LAPTIME_RSSI = CBUUID.UUIDWithString_(u'135BFFE1-E787-4A27-9402-D76493424B53')
VTX_LAST_RSSI = CBUUID.UUIDWithString_(u'FBD47252-6210-4692-A247-1DB3007CF668')
VTX_LAPTIME_EVENT = CBUUID.UUIDWithString_(u'C112478C-9801-481D-8A79-854D23FD9DF2')
VTX_TIMESTAMP = CBUUID.UUIDWithString_(u'438351FA-60D1-424F-A08A-90EA69BE91D5')

VALUE_ON_MODE_SWITCH = {
    Mode.LAPTIMER: (VTX_LAPTIME_RSSI, )
}

CBCharacteristicWriteWithoutResponse = 1


@unique
class CBManagerState(IntEnum):
    unknown = 0
    resetting = 1
    unsupported = 2
    unauthorized = 3
    poweredOff = 4
    poweredOn = 5


def execute_in_main_thread(callable):
    mq = libdispatch.dispatch_get_main_queue()

    def forward():
        callable()

    libdispatch.dispatch_async(mq, forward)


class CBCentralManagerDelegate(NSObject):
    """
    Scans for VTX scanners and manages them
    """

    def init(self):
        super().init()
        self._manager = CBCentralManager.alloc().initWithDelegate_queue_options_(self, None, None)
        # we seem to need to retain these because otherwise
        # connecting doesn't work
        self._discovered_peripherals = {}
        self._connected_peripherals = {}
        self.scanner_connected = rx.subject.Subject()
        return self

    def centralManagerDidUpdateState_(self, manager):
        state = manager.state()
        if state == CBManagerState.poweredOn:
            manager.scanForPeripheralsWithServices_options_(
                [VTX_SERVICE], None
            )

    def centralManager_didDiscoverPeripheral_advertisementData_RSSI_(self, manager, peripheral, data, rssi):
        identifier = peripheral.identifier()
        if identifier not in self._discovered_peripherals:
            self._discovered_peripherals[identifier] = peripheral
            manager.connectPeripheral_options_(peripheral, None)

    def centralManager_didConnectPeripheral_(self, manager, peripheral):
        vtx_delegate = VTXDelegate.alloc().initWithPeripheral_manager_(
            peripheral, manager
        )
        self._connected_peripherals[peripheral.identifier()] = vtx_delegate
        execute_in_main_thread(
            lambda: self.scanner_connected.on_next(vtx_delegate)
        )

    def centralManager_didFailToConnectPeripheral_error_(
            self, manager, peripheral, error,
            ):
        print("failed to connect")

    def centralManager_didDisconnectPeripheral_error_(
            self, manager, peripheral, error
            ):
        print("peripheral error, reconnecting")
        manager.scanForPeripheralsWithServices_options_([VTX_SERVICE], None)

    def __len__(self):
        return len(self._connected_peripherals)

    def __getitem__(self, index):
        if isinstance(index, int):
            return self._connected_peripherals[
                list(self._connected_peripherals.keys())[index]
            ]


class VTXDelegate(NSObject):

    CHARACTERISTICS_TO_DISCOVER = [
        VTX_CURRENT_MODE,
        VTX_LAPTIME_RSSI,
        VTX_LAST_RSSI,
        VTX_LAPTIME_EVENT,
        VTX_TIMESTAMP,
    ]

    def initWithPeripheral_manager_(self, peripheral, manager):
        super().init()
        self.manager = manager
        self.peripheral = peripheral
        self.service = None
        self._mode = None
        self._characteristics = {}

        self.peripheral.setDelegate_(self)
        self.peripheral.discoverServices_([VTX_SERVICE])
        self.last_rssi_subject = rx.subject.Subject()
        self.mode_subject = rx.subject.Subject()
        self.laptime_rssi_subject = rx.subject.Subject()
        return self

    @property
    def name(self):
        return self.peripheral.identifier()

    @property
    def mode(self):
        return self._mode

    @mode.setter
    def mode(self, value):
        if value != self._mode:
            self._mode = value
            self._trigger_value_read()

    def _trigger_value_read(self):
        for uuid in VALUE_ON_MODE_SWITCH.get(self.mode, ()):
            self.peripheral.readValueForCharacteristic_(
                self._characteristics[uuid]
            )

    def changeMode_(self, mode):
        self.peripheral.writeValue_forCharacteristic_type_(
            struct.pack("<I", mode.value),
            self._characteristics[VTX_CURRENT_MODE],
            CBCharacteristicWriteWithoutResponse,
        )

    def peripheral_didDiscoverServices_(self, peripheral, services):
        self.service = self.peripheral.services()[0]
        self.peripheral.discoverCharacteristics_forService_(self.CHARACTERISTICS_TO_DISCOVER, self.service)

    def peripheral_didDiscoverCharacteristicsForService_error_(self, peripheral, service, error):
        for characteristic in self.service.characteristics():
            uuid = characteristic.UUID()
            if uuid in self.CHARACTERISTICS_TO_DISCOVER:
                self.peripheral.setNotifyValue_forCharacteristic_(True, characteristic)
                self.peripheral.readValueForCharacteristic_(characteristic)
                self._characteristics[uuid] = characteristic

    def peripheral_didWriteValueForCharacteristic_error_(self, peripheral, characteristic, error):
           print("peripheral_didWriteValueForCharacteristic_error_")

    def peripheral_didUpdateNotificationStateForCharacteristic_error_(self, peripheral, characteristic, error):
        print("Receiving notifications for", characteristic)

    def peripheral_didUpdateValueForCharacteristic_error_(self, peripheral, characteristic, error):
        value = self.TRANSFORM[characteristic.UUID()](self, characteristic)
        self._trigger_value_read()

    def readTimestamp(self):
        self.peripheral.readValueForCharacteristic_(
            self._characteristics[VTX_TIMESTAMP],
        )

    def currentMode_(self, characteristic):
        data = characteristic.value().bytes().tobytes()
        mode = struct.unpack("<I", data)[0]
        self.mode = Mode(mode)
        execute_in_main_thread(lambda: self.mode_subject.on_next(self.mode))

    def laptimeRssi_(self, characteristic):
        value = characteristic.value()
        now = time.monotonic()
        v = [v << 4 for v in value]
        execute_in_main_thread(lambda: self.laptime_rssi_subject.on_next((now, v)))

    def lastRssi_(self, characteristic):
        data = characteristic.value().bytes().tobytes()
        channel, value = struct.unpack("<HH", data)
        execute_in_main_thread(
            lambda: self.last_rssi_subject.on_next((channel, value))
        )

    def laptime_(self, characteristic):
        data = characteristic.value().bytes().tobytes()
        count, laptime = struct.unpack("<Hq", data)
        laptime /= 1000_000 # to seconds

    def timestamp_(self, characteristic):
        data = characteristic.value().bytes()
        timestamp, = struct.unpack("<q", data)
        timestamp /= 1000_000 # to seconds
        return ("timestamp", timestamp)

    TRANSFORM = {
        VTX_CURRENT_MODE: currentMode_,
        VTX_LAPTIME_RSSI: laptimeRssi_,
        VTX_LAST_RSSI: lastRssi_,
        VTX_LAPTIME_EVENT: laptime_,
        VTX_TIMESTAMP: timestamp_,
    }


def setup_bt_delegate():
    return CBCentralManagerDelegate.alloc().init()
