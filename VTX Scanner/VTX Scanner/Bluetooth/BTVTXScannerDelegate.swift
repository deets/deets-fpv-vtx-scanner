import Foundation
import CoreBluetooth
import BinUtils
import ReactiveSwift
import Result

class BTVTXScannerDelegate : NSObject, CBPeripheralDelegate
{

    struct LatestRSSIReading {
        let channel: UInt16
        let value: UInt16
    }

    private let (latestRSSIReading_, latestRSSIReadingObserver) = Signal<LatestRSSIReading, NoError>.pipe()

    var latestRSSIReading : Signal<LatestRSSIReading, NoError> {
        get {
            return latestRSSIReading_
        }
    }

    let maxRSSI = MutableProperty<UInt16>(UInt16(0))

    // VTX scanner
    static let VTX_SCANNER_SERVICE = "D27E29B4-4DBD-4103-A8B6-09301EFDDD01"
    static let serviceUUIDs = [CBUUID(string: VTX_SCANNER_SERVICE)]

    // VTX scanner current mode characteristic
    private static let VTX_SCANNER_CURRENT_MODE = "903BB704-5ADC-48D3-B0D4-0628BDB35250"
    // VTX scanner last RSSI reading
    private static let VTX_SCANNER_LAST_RSSI_READING = "FBD47252-6210-4692-A247-1DB3007CF668"
    // max rssi
    private static let VTX_SCANNER_MAX_RSSI = "538D01D2-662F-4C0E-A808-1F23F159DF1A"

    private let characteristicUUIDs = [
      CBUUID(string: VTX_SCANNER_CURRENT_MODE),
      CBUUID(string: VTX_SCANNER_LAST_RSSI_READING),
      CBUUID(string: VTX_SCANNER_MAX_RSSI),
      ]

    private var peripheral: CBPeripheral
    private var manager: CBCentralManager

    init(withPeripheral thePeripheral: CBPeripheral, manager theManager: CBCentralManager) {
        peripheral = thePeripheral
        manager = theManager
        super.init()
        peripheral.delegate = self
    }

    func connected() {
        peripheral.discoverServices(BTVTXScannerDelegate.serviceUUIDs)
    }


    func disconnected() {

    }
    // // MARK: - CBPeripheralDelegate

    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if error != nil {
            NSLog("Discover services for device %@ failed: %@", peripheral.name!, error?.localizedDescription ?? "unspecified error")
            manager.cancelPeripheralConnection(peripheral)
        } else {
            NSLog("Discover services for device %@ succeeded: %d services", peripheral.name!, peripheral.services!.count)
            for service in peripheral.services! {
                peripheral.discoverCharacteristics(characteristicUUIDs, for: service)
            }
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if error == nil {
            NSLog("Discovered characteristics for service %@", service.uuid.uuidString)
            guard let characteristics = service.characteristics, !characteristics.isEmpty else {
                return
            }
            for c in characteristics {
                if characteristicUUIDs.contains(c.uuid) {
                    peripheral.setNotifyValue(true, for: c)
                    peripheral.readValue(for: c)
                }
            }
        } else {
            NSLog("Error discovering characteristics for service %@: %@", service.uuid.uuidString, error?.localizedDescription ?? "unspecified error")
            manager.cancelPeripheralConnection(peripheral)
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        if error == nil {
//            delegate?.connectedPeripheral(BluetoothPeripheral(peripheral))
        } else {
            NSLog("didUpdateNotificationStateForCharacteristic %@: %@", characteristic.uuid.uuidString, error?.localizedDescription ?? "unspecified error")
            manager.cancelPeripheralConnection(peripheral)
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if error == nil {
            print("didUpdateValueForCharacteristic",
                  characteristic.uuid,
                  "length:", characteristic.value!.count)
            // Lift data into main thread for further processing
            let data = characteristic.value!
            let uuid = characteristic.uuid
            if data.count > 0 {
                DispatchQueue.main.async {
                    self.dispatchCharacteristic(uuid: uuid, data: data)
                }
            }
        } else {
            NSLog("Error updating value for characteristic: %@", error?.localizedDescription ?? "unspecified error")
        }
    }

    private func dispatchCharacteristic(uuid: CBUUID, data: Data)
    {
        // This is ugly, but maybe one day I know enough swift
        if uuid.uuidString == BTVTXScannerDelegate.VTX_SCANNER_LAST_RSSI_READING {
            processLastRSSIReading(data: data)
        }
        if uuid.uuidString ==
            BTVTXScannerDelegate.VTX_SCANNER_MAX_RSSI {
            processMaxRSSI(data: data)
        }
    }

    private func processLastRSSIReading(data:Data)
    {
        do {
            let a = try unpack("<HH", data)
            let reading = LatestRSSIReading(channel: UInt16((a[0] as? Int)!), value: UInt16((a[1] as? Int)!))
            NSLog("lastRSSIReading %i, %i", reading.channel, reading.value)
            latestRSSIReadingObserver.send(value: reading)
        } catch {
        }
    }

    private func processMaxRSSI(data:Data)
    {
        do {
            let a = try unpack("<H", data)
            let v = UInt16((a[0] as? Int)!)
            maxRSSI.value = v
        } catch {
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        if error != nil {
            NSLog("Error writing value for characteristic: %@", error?.localizedDescription ?? "unspecified error")
        }
    }
}
