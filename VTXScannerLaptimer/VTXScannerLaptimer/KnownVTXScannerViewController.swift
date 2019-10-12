//
//  ViewController.swift
//  VTXScannerLaptimer
//
//  Created by Diez B. Roggisch on 05.10.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Cocoa

class KnownVTXScannerViewController: NSViewController {

    @IBOutlet weak var tableView: NSTableView!

    var scanners : VTXScannerList? {
        didSet {
            // reload on change
            scanners?.numberOfScanners.signal.observeValues({(count) in
                if let tableView = self.tableView {
                    tableView.reloadData()
                }
            })
            // reload on set
            tableView.reloadData()
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        tableView.delegate = self
        tableView.dataSource = self
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
}

extension KnownVTXScannerViewController: NSTableViewDataSource {

  func numberOfRows(in tableView: NSTableView) -> Int {
    let result = scanners != nil ? scanners!.count : 0
    return result
  }

}

extension KnownVTXScannerViewController: NSTableViewDelegate {

  fileprivate enum CellIdentifiers {
    static let NameCellID = NSUserInterfaceItemIdentifier(rawValue: "NameCellID")
    static let StatusCellID = NSUserInterfaceItemIdentifier(rawValue: "StatusCellID")

  }


  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {

    var text: String = ""
    if(row >= numberOfRows(in: tableView))
    {
        return nil
    }
    let item = scanners!.at(row)!

    if tableColumn?.identifier == CellIdentifiers.NameCellID {
      text = item.name
    } else if tableColumn?.identifier == CellIdentifiers.StatusCellID {
        text = item.status
    }

    if let cell = tableView.makeView(withIdentifier: tableColumn!.identifier, owner: self) as? NSTableCellView {
        cell.textField?.stringValue = text
      return cell
    }
    return nil
  }

}


extension KnownVTXScannerViewController: VTXScannerListContainer {
    func setScannerList(_ scannerList: VTXScannerList) {
        self.scanners = scannerList
    }
}
