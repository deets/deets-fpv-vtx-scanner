import Foundation
import CoreBluetooth

class BTVTXScannerDelegate : NSObject, CBPeripheralDelegate
{

    // VTX scanner
    static let VTX_SCANNER_SERVICE = "D27E29B4-4DBD-4103-A8B6-09301EFDDD01"
    
    static let serviceUUIDs = [CBUUID(string: VTX_SCANNER_SERVICE)]
    // VTX scanner current mode characteristic
    static let VTX_SCANNER_CURRENT_MODE = "903BB704-5ADC-48D3-B0D4-0628BDB35250"
    // VTX scanner last RSSI reading
    static let VTX_SCANNER_LAST_RSSI_READING = "FBD47252-6210-4692-A247-1DB3007CF668"

    let characteristicUUIDs = [
      CBUUID(string: VTX_SCANNER_CURRENT_MODE),
      CBUUID(string: VTX_SCANNER_LAST_RSSI_READING),
    ]

    var peripheral: CBPeripheral
    var manager: CBCentralManager

    
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
                if c.uuid.uuidString == BTVTXScannerDelegate.VTX_SCANNER_LAST_RSSI_READING {
                    peripheral.setNotifyValue(true, for: c)
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
            NSLog("didUpdateValueForCharacteristic %@", characteristic.uuid)
            let nsdata = characteristic.value!
            var data = [UInt8](repeating: 0, count: nsdata.count)
            (nsdata as NSData).getBytes(&data, length:nsdata.count)
            
            NSLog("Read %@", beautifyData(data))
        } else {
            NSLog("Error updating value for characteristic: %@", error?.localizedDescription ?? "unspecified error")
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        if error != nil {
            NSLog("Error writing value for characteristic: %@", error?.localizedDescription ?? "unspecified error")
        }
    }
    
    // func peripheral(_ peripheral: CBPeripheral, didReadRSSI RSSI: NSNumber, error: Error?) {
    //     //NSLog("BT RSSI=%@", RSSI)       // -104 -> -26 ?
    //     let appDelegate = UIApplication.shared.delegate as! AppDelegate
    //     appDelegate.msp.setRssi(RSSI.doubleValue)
    // }
    
    
    fileprivate func beautifyData(_ data: [UInt8]) -> String {
        var string = ""
        for var c in data {
            if c >= 32 && c < 128 {
                string += NSString(bytes: &c, length: 1, encoding: String.Encoding.ascii.rawValue)! as String
            } else {
                string += String(format: "'%d'", c)
            }
            string += " "
        }
        return string
    }

}
