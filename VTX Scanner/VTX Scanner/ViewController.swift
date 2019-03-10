//
//  ViewController.swift
//  VTX Scanner
//
//  Created by Diez B. Roggisch on 02.03.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import UIKit
import ReactiveSwift
import Result
import SwiftSVG

class ViewController: UIViewController, BTDelegate  {

    @IBOutlet var scannerView : ScannerView?
    @IBOutlet var laptimerView : LaptimerView?
    @IBOutlet var scannerModeButton : UIView?

    let (connectedScanner, connectedScannerObserver) = Signal<BTVTXScannerDelegate, NoError>.pipe()
    
    func didConnectScanner(_ scanner: BTVTXScannerDelegate) {
        NSLog("connectedScanner: new scanner")
        connectedScannerObserver.send(value: scanner)
    }
    
    
    var btManager: BTManager?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        if let subscriber = scannerView?.scannerObserver {
            NSLog("connectedScanner.observe")
            connectedScanner.observe(subscriber)
        }
        if let subscriber = laptimerView?.scannerObserver {
            NSLog("connectedScanner.observe")
            connectedScanner.observe(subscriber)
        }
        btManager = BTManager()
        btManager!.delegate = self
        
        if let v = scannerModeButton {
            let svgView = UIView(SVGNamed: "scanner-icon") { (svgLayer) in svgLayer.resizeToFit(v.bounds) }
            svgView.backgroundColor = UIColor.blue
            v.addSubview(svgView)
        }
    }


}

