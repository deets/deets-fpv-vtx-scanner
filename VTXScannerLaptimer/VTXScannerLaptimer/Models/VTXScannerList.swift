//
//  VTXScannerList.swift
//  VTXScannerLaptimer
//
//  Created by Diez B. Roggisch on 11.10.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Foundation
import ReactiveSwift

class VTXScannerList {

    struct VTXScanner
    {
        var name: String
        var status: String
    }
    private var scanners: [VTXScanner] = []
    let numberOfScanners = MutableProperty<Int>(0)

    var count : Int {
        get {
            scanners.count
        }
    }

    func addScanner(_ name: String, _ status: String)
    {
        let new_scanner = VTXScannerList.VTXScanner(name: name, status: status)
        scanners.append(new_scanner)
        numberOfScanners.value = scanners.count
    }

    func at(_ index: Int) -> VTXScannerList.VTXScanner?
    {
        return scanners[index]
    }
}

protocol VTXScannerListContainer {
    func setScannerList(_ scannerList: VTXScannerList)
}
