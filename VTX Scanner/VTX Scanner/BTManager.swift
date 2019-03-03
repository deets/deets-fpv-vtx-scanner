//
//  BTManager.swift
//  VTX Scanner
//
//  Created by Diez B. Roggisch on 02.03.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Foundation
import CoreBluetooth

// MARK: - BluetoothDelegate
protocol BluetoothDelegate : class {
    func foundPeripheral(_ peripheral: BluetoothPeripheral)
    func stoppedScanning()
    func connectedPeripheral(_ peripheral: BluetoothPeripheral)
    func failedToConnectToPeripheral(_ peripheral: BluetoothPeripheral, error: Error?)
    func disconnectedPeripheral(_ peripheral: BluetoothPeripheral)
    func receivedData(_ peripheral: BluetoothPeripheral, data: [UInt8])
}

struct BluetoothPeripheral {
    let name: String
    let uuid: String
    
    init(_ peripheral: CBPeripheral) {
        self.name = peripheral.name!
        self.uuid = peripheral.identifier.uuidString
    }
}

class BTManager : NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {

    // VTX scanner
    static let VTX_SCANNER_SERVICE = "D27E29B4-4DBD-4103-A8B6-09301EFDDD01"
    // VTX scanner current mode characteristic
    static let VTX_SCANNER_CURRENT_MODE = "903BB704-5ADC-48D3-B0D4-0628BDB35250"
    
    let serviceUUIDs = [CBUUID(string: VTX_SCANNER_SERVICE)]
    let characteristicUUIDs = [CBUUID(string: VTX_SCANNER_CURRENT_MODE)]
    
    let btQueue: DispatchQueue
    let manager: CBCentralManager
    var peripherals = [CBPeripheral]()
    var activePeripheral: CBPeripheral?
    
    var scanningRequested = true
    var scanningDuration = 15.0
    
    var delegate: BluetoothDelegate?
    
    override init() {
        btQueue = DispatchQueue(label: "bluetoothQueue", attributes: [])
        manager = CBCentralManager(delegate: nil, queue: btQueue, options: [CBCentralManagerOptionShowPowerAlertKey : NSNumber(value: 1 as Int)])
        super.init()
        manager.delegate = self
    }
    
    func startScanning(_ duration: Double  = 5.0) {
        scanningRequested = true
        scanningDuration = duration
        if manager.state != CBManagerState.poweredOn {
            NSLog("CoreBluetooth powered off")
            return
        }
        startScanningIfNeeded()
    }
    
    fileprivate func startScanningIfNeeded() {
        if scanningRequested {
            scanningRequested = false
            Timer.scheduledTimer(timeInterval: scanningDuration, target:self, selector: #selector(BTManager.scanTimer), userInfo: nil, repeats: false)
            
            manager.scanForPeripherals(withServices: serviceUUIDs, options: nil)
//            manager.scanForPeripherals(withServices: nil, options: nil)
        }
    }
    
    @objc func scanTimer() {
        manager.stopScan()
        delegate?.stoppedScanning()
    }
    
    // MARK: - CBCentralManagerDelegate
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        NSLog("centralManagerDidUpdateState: %d", central.state.rawValue)
        if central.state == CBManagerState.poweredOn {
            startScanningIfNeeded()
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if peripheral.name == nil {
            //NSLog("Device has nil name, ignored")
            return
        }
        if peripheral.name! != "VTX Gizmo" {
            return
        }
        NSLog("Found device")

        for (i, p) in peripherals.enumerated() {
            if p.identifier.uuidString == peripheral.identifier.uuidString {
                NSLog("Existing device replaced: " + p.identifier.uuidString)
                peripherals[i] = peripheral
                delegate?.foundPeripheral(BluetoothPeripheral(peripheral))
                
                return
            }
        }
        NSLog("New device %@ (%@)", peripheral.name!, peripheral.identifier.uuidString)
        peripherals.append(peripheral)
        delegate?.foundPeripheral(BluetoothPeripheral(peripheral))
        central.connect(peripheral)
    }
    
     func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
         activePeripheral = peripheral
         activePeripheral?.delegate = self
        
         activePeripheral?.discoverServices(serviceUUIDs)
        
         NSLog("Connected to device %@", peripheral.name!)
     }
    
     func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
         NSLog("Disconnected from device %@", peripheral.name!)
         if peripheral === activePeripheral {
             activePeripheral = nil
             delegate?.disconnectedPeripheral(BluetoothPeripheral(peripheral))
         }
     }
    
     func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
         NSLog("Connection to device %@ failed: %@", peripheral.name!, error?.localizedDescription ?? "unspecified error")
         delegate?.failedToConnectToPeripheral(BluetoothPeripheral(peripheral), error: error)
     }
    
    // // MARK: - CBPeripheralDelegate
    
     func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
         if error != nil {
             NSLog("Discover services for device %@ failed: %@", peripheral.name!, error?.localizedDescription ?? "unspecified error")
             activePeripheral = nil
             manager.cancelPeripheralConnection(peripheral)
             delegate?.failedToConnectToPeripheral(BluetoothPeripheral(peripheral), error: error)
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
//             var characteristic: CBCharacteristic!
//             if service.uuid.uuidString == BTManager.RBLServiceUUID {
//                 for char in service.characteristics! {
//                     if char.uuid.uuidString == BTManager.RBLCharTxUUID {
//                         characteristic = char
//                     }
//                 }
//             } else {
//                 guard let characteristics = service.characteristics, !characteristics.isEmpty else {
//                     return
//                 }
//                 characteristic = characteristics[0]
//             }
//             peripheral.setNotifyValue(true, for: characteristic)
         } else {
             NSLog("Error discovering characteristics for service %@: %@", service.uuid.uuidString, error?.localizedDescription ?? "unspecified error")
             activePeripheral = nil
             manager.cancelPeripheralConnection(peripheral)
             delegate?.failedToConnectToPeripheral(BluetoothPeripheral(peripheral), error: error)
         }
     }
    
     func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
         if error == nil {
             delegate?.connectedPeripheral(BluetoothPeripheral(peripheral))
         } else {
             NSLog("didUpdateNotificationStateForCharacteristic %@: %@", characteristic.uuid.uuidString, error?.localizedDescription ?? "unspecified error")
             activePeripheral = nil
             manager.cancelPeripheralConnection(peripheral)
             delegate?.failedToConnectToPeripheral(BluetoothPeripheral(peripheral), error: error)
         }
     }
    
     func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
         if error == nil {
             //NSLog("didUpdateValueForCharacteristic %@", characteristic.value!)
             let nsdata = characteristic.value!
             var data = [UInt8](repeating: 0, count: nsdata.count)
             (nsdata as NSData).getBytes(&data, length:nsdata.count)
            
             //NSLog("Read %@", beautifyData(data))
             delegate?.receivedData(BluetoothPeripheral(peripheral), data: data)
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
    
    // // MARK:
    
    // func connect(_ peripheral: BluetoothPeripheral) {
    //     for p in peripherals {
    //         if peripheral.uuid == p.identifier.uuidString && p.state != .connected && p.state != .connecting {
    //             manager.connect(p, options: nil)
    //             return
    //         }
    //     }
    //     delegate?.failedToConnectToPeripheral(peripheral, error: NSError(domain: "com.flyinghead", code: 1, userInfo: [NSLocalizedDescriptionKey : "Device cannot be found"]))
    // }
    
    // func disconnect(_ peripheral: BluetoothPeripheral) {
    //     for p in peripherals {
    //         if peripheral.uuid == p.identifier.uuidString && (p.state == .connected || p.state == .connecting) {
    //             manager.cancelPeripheralConnection(p)
    //             return
    //         }
    //     }
    //     delegate?.disconnectedPeripheral(peripheral)
    // }
    
    // func writeData(_ peripheral: BluetoothPeripheral, data: [UInt8]) {
    //     if peripheral.uuid != activePeripheral?.identifier.uuidString {
    //         return
    //     }
    //     guard let services = activePeripheral!.services, !services.isEmpty else {
    //         return
    //     }
    //     let service = services[0]
    //     if service.characteristics == nil || service.characteristics!.isEmpty {
    //         return
    //     }
    //     var characteristic: CBCharacteristic!
    //     if service.uuid.uuidString == BTManager.RBLServiceUUID {
    //         for char in service.characteristics! {
    //             if char.uuid.uuidString == BTManager.RBLCharRxUUID {
    //                 characteristic = char
    //                 break
    //             }
    //         }
    //     } else {
    //         characteristic = service.characteristics![0]
    //     }
    //     if characteristic == nil {
    //         return
    //     }
        
    //     //NSLog("Writing %@", beautifyData(data))
    //     let nsdata = Data(bytes: UnsafePointer<UInt8>(data), count: data.count)
    //     var i = 0
    //     while i < nsdata.count {
    //         let datarange = nsdata.subdata(in: i ..< i + min(nsdata.count - i, 20))
    //         if characteristic.properties.contains(.writeWithoutResponse) {
    //             activePeripheral!.writeValue(datarange, for: characteristic, type: .withoutResponse)
    //         } else {
    //             activePeripheral!.writeValue(datarange, for: characteristic, type: .withResponse)
    //         }
    //         i += 20
    //     }
    // }
    
    // fileprivate func beautifyData(_ data: [UInt8]) -> String {
    //     var string = ""
    //     for var c in data {
    //         if c >= 32 && c < 128 {
    //             string += NSString(bytes: &c, length: 1, encoding: String.Encoding.ascii.rawValue)! as String
    //         } else {
    //             string += String(format: "'%d'", c)
    //         }
    //         string += " "
    //     }
    //     return string
    // }
    
    // func readRssi(_ peripheral: BluetoothPeripheral) {
    //     for p in peripherals {
    //         if peripheral.uuid == p.identifier.uuidString {
    //             p.readRSSI()
    //             return
    //         }
    //     }
    // }
}

extension CBCentralManager {
    internal var centralManagerState: CBManagerState  {
        get {
            return CBManagerState(rawValue: state.rawValue) ?? .unknown
        }
    }
}
