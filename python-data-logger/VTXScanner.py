import select
import socket
import sys
import struct
from enum import Enum, unique
from collections import defaultdict

import objc
import libdispatch
from PyObjCTools import AppHelper
from Foundation import NSObject


objc.loadBundle("CoreBluetooth", globals(),
    bundle_path=objc.pathForFramework(u'/System/Library/Frameworks/IOBluetooth.framework/Versions/A/Frameworks/CoreBluetooth.framework'))

@unique
class Mode(Enum):
    SPLASH_SCREEN, SCANNER, LAPTIMER, SETTINGS = range(4)


RSSI_PATH = "/rssi"
MODE_PATH = "/mode"
LAST_RSSI_PATH = "/lastrssi"
NEW_LAPTIME_PATH = "/laptime"

PORT = 34254
HOST = "localhost"


VTX_SERVICE = CBUUID.UUIDWithString_(u'D27E29B4-4DBD-4103-A8B6-09301EFDDD01')
VTX_CURRENT_MODE = CBUUID.UUIDWithString_(u'903BB704-5ADC-48D3-B0D4-0628BDB35250')
VTX_LAPTIME_RSSI = CBUUID.UUIDWithString_(u'135BFFE1-E787-4A27-9402-D76493424B53')
VTX_LAST_RSSI = CBUUID.UUIDWithString_(u'FBD47252-6210-4692-A247-1DB3007CF668')
VTX_LAPTIME_EVENT = CBUUID.UUIDWithString_(u'C112478C-9801-481D-8A79-854D23FD9DF2')

VALUE_ON_MODE_SWITCH = {
    Mode.LAPTIMER: (VTX_LAPTIME_RSSI, )
}


class VTXDelegate(NSObject):

    CHARACTERISTICS_TO_DISCOVER = [
        VTX_CURRENT_MODE,
        VTX_LAPTIME_RSSI,
        VTX_LAST_RSSI,
        VTX_LAPTIME_EVENT,
    ]

    def init(self):
        self.manager = None
        self.peripheral = None
        self.service = None
        self._mode = None
        self._characteristics = {}
        self._listeners = defaultdict(list)
        return self

    def emit_(self, value):
        message, args = value[0], value[1:]
        for listener in self._listeners[message]:
            listener(args)

    def addListener_for_(self, listener, message):
        self._listeners[message].append(listener)

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

    def centralManagerDidUpdateState_(self, manager):
        self.manager = manager
        manager.scanForPeripheralsWithServices_options_([VTX_SERVICE], None)

    def centralManager_didDiscoverPeripheral_advertisementData_RSSI_(self, manager, peripheral, data, rssi):
        print("peripheral", peripheral)
        self.peripheral = peripheral
        manager.connectPeripheral_options_(peripheral, None)

    def centralManager_didConnectPeripheral_(self, manager, peripheral):
        print("connected peripheral", peripheral)
        self.peripheral.setDelegate_(self)
        self.peripheral.discoverServices_([VTX_SERVICE])

    def centralManager_didFailToConnectPeripheral_error_(self, manager, peripheral, error):
        print("failed to connect")

    def centralManager_didDisconnectPeripheral_error_(self, manager, peripheral, error):
        print("peripheral error, reconnecting")
        manager.scanForPeripheralsWithServices_options_([VTX_SERVICE], None)

    def peripheral_didDiscoverServices_(self, peripheral, services):
        self.service = self.peripheral.services()[0]
        self.peripheral.discoverCharacteristics_forService_(self.CHARACTERISTICS_TO_DISCOVER, self.service)

    def peripheral_didDiscoverCharacteristicsForService_error_(self, peripheral, service, error):
        print( repr(error))
        for characteristic in self.service.characteristics():
            uuid = characteristic.UUID()
            if uuid in self.CHARACTERISTICS_TO_DISCOVER:
                self.peripheral.setNotifyValue_forCharacteristic_(True, characteristic)
                self.peripheral.readValueForCharacteristic_(characteristic)
                self._characteristics[uuid] = characteristic

    def peripheral_didWriteValueForCharacteristic_error_(self, peripheral, characteristic, error):
        pass

    def peripheral_didUpdateNotificationStateForCharacteristic_error_(self, peripheral, characteristic, error):
        print("Receiving notifications for", characteristic)

    def peripheral_didUpdateValueForCharacteristic_error_(self, peripheral, characteristic, error):
        # This is executed in a background thread. We need to
        # forward it to the main thread
        value = self.TRANSFORM[characteristic.UUID()](self, characteristic)
        self._trigger_value_read()
        mq = libdispatch.dispatch_get_main_queue()

        def forward():
            if value is not None:
                self.emit_(value)

        libdispatch.dispatch_async(mq, forward)

    def currentMode_(self, characteristic):
        data = characteristic.value().bytes().tobytes()
        mode = struct.unpack("<I", data)[0]
        self.mode = Mode(mode)
        print("mode switched:", self.mode)

    def laptimeRssi_(self, characteristic):
        value = characteristic.value()
        # b = osc_message_builder.OscMessageBuilder(RSSI_PATH)
        # for v in value:
        #     b.add_arg(v << 4) # scaled up to 4096
        # message = b.build()
        # self._client.send(message)

    def lastRssi_(self, characteristic):
        data = characteristic.value().bytes().tobytes()
        channel, value = struct.unpack("<HH", data)
        return ("last_rssi", channel, value)

    def laptime_(self, characteristic):
        data = characteristic.value().bytes().tobytes()
        count, laptime = struct.unpack("<Hq", data)
        laptime /= 1000_000 # to seconds

    TRANSFORM = {
        VTX_CURRENT_MODE: currentMode_,
        VTX_LAPTIME_RSSI: laptimeRssi_,
        VTX_LAST_RSSI: lastRssi_,
        VTX_LAPTIME_EVENT: laptime_,
    }

def setup_bt_delegate():
    delegate = VTXDelegate.alloc().init()
    manager = CBCentralManager.alloc()
    manager.initWithDelegate_queue_options_(delegate, None, None)
    return manager, delegate

if __name__ == "__main__":
    app = MyApp.sharedApplication()
    AppHelper.runEventLoop()
