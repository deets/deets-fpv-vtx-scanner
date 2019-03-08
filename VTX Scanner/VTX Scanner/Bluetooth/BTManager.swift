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

protocol BTDelegate : class {
    func didConnectScanner(_ scanner: BTVTXScannerDelegate)
}



class BTManager : NSObject, CBCentralManagerDelegate {
    
    let btQueue: DispatchQueue
    let manager: CBCentralManager
    var vtxScanners = [UUID:BTVTXScannerDelegate]()

    var scanningRequested = true
    var scanningDuration = 15.0
    
    var delegate: BTDelegate?
    
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
            let d = BTVTXScannerDelegate(
                withPeripheral: peripheral,
                manager: manager
            )
            vtxScanners[peripheral.identifier] = d
            delegate?.didConnectScanner(d)
        }
        // We do this here because this allows to re-connect a disconnected
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
}

extension CBCentralManager {
    internal var centralManagerState: CBManagerState  {
        get {
            return CBManagerState(rawValue: state.rawValue) ?? .unknown
        }
    }
}
