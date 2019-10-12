//
//  AppDelegate.swift
//  VTXScannerLaptimer
//
//  Created by Diez B. Roggisch on 05.10.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    let scanners = VTXScannerList()


    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let window:NSWindow = NSApplication.shared.windows.first!

        scanners.addScanner("Diez", "Tired")

        if let controller = window.contentViewController  {
            if let current = controller as? VTXScannerListContainer {
                current.setScannerList(scanners)
            }
            for child in controller.children {
                if let top = child as? VTXScannerListContainer {
                    top.setScannerList(scanners)
                }
            }
        }
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}
