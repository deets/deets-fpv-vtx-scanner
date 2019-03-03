//
//  BTManager.swift
//  VTX Scanner
//
//  Created by Diez B. Roggisch on 02.03.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Foundation
import CoreBluetooth

struct BluetoothPeripheral {
    let name: String
    let uuid: String
    
    init(_ peripheral: CBPeripheral) {
        self.name = peripheral.name!
        self.uuid = peripheral.identifier.uuidString
    }
}

class BTManager : NSObject, CBCentralManagerDelegate {
    
    let btQueue: DispatchQueue
    let manager: CBCentralManager
    var vtxScanners = [UUID:BTVTXScannerDelegate]()

    var scanningRequested = true
    var scanningDuration = 15.0
    
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
            
            manager.scanForPeripherals(withServices: BTVTXScannerDelegate.serviceUUIDs, options: nil)
//            manager.scanForPeripherals(withServices: nil, options: nil)
        }
    }
    
    @objc func scanTimer() {
        manager.stopScan()
    }
    
    // MARK: - CBCentralManagerDelegate
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        NSLog("centralManagerDidUpdateState: %d", central.state.rawValue)
        if central.state == CBManagerState.poweredOn {
            startScanningIfNeeded()
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        NSLog("Found device")
        if let _ = vtxScanners[peripheral.identifier] {            
        } else {
            vtxScanners[peripheral.identifier] = BTVTXScannerDelegate(
                withPeripheral: peripheral,
                manager: manager
            )
        }
        // We do this here because this allows to re-connect disconnected
        // delegate
        manager.connect(peripheral)
    }
    
     func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
         NSLog("Connected to device %@", peripheral.name!)
         vtxScanners[peripheral.identifier]!.connected()
     }
    
     func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
         NSLog("Disconnected from device %@", peripheral.name!)
         vtxScanners[peripheral.identifier]!.disconnected()
     }
    
     func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
         NSLog("Connection to device %@ failed: %@", peripheral.name!, error?.localizedDescription ?? "unspecified error")
        vtxScanners.removeValue(forKey: peripheral.identifier)
     }
    
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
