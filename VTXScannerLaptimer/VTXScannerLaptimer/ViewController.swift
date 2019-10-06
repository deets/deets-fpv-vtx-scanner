//
//  ViewController.swift
//  VTXScannerLaptimer
//
//  Created by Diez B. Roggisch on 05.10.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    @IBOutlet weak var tableView: NSTableView!
  
    struct TestEntry {
        let name: String
        var status: String
    }
    
    let testData: [TestEntry] = [
        TestEntry(name: "Anne", status: "Ok"),
        TestEntry(name: "Diez", status: "Tired")
       ]
    
    override func viewDidLoad() {
        super.viewDidLoad()

        tableView.delegate = self
        tableView.dataSource = self
        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

extension ViewController: NSTableViewDataSource {
  
  func numberOfRows(in tableView: NSTableView) -> Int {
    return testData.count;
  }

}

extension ViewController: NSTableViewDelegate {

  fileprivate enum CellIdentifiers {
    static let NameCellID = NSUserInterfaceItemIdentifier(rawValue: "NameCellID")
    static let StatusCellID = NSUserInterfaceItemIdentifier(rawValue: "StatusCellID")
    
  }
    
   
  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {

    var text: String = ""
    if(row >= testData.count)
    {
        return nil
    }
    let item = testData[row]

    // 2
    if tableColumn?.identifier == CellIdentifiers.NameCellID {
      text = item.name
    } else if tableColumn?.identifier == CellIdentifiers.StatusCellID {
        text = item.status
    }
    
    // 3
    if let cell = tableView.makeView(withIdentifier: tableColumn!.identifier, owner: self) as? NSTableCellView {
        cell.textField?.stringValue = text
      return cell
    }
    return nil
  }

}
