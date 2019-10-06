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
    static let NameCell = NSUserInterfaceItemIdentifier(rawValue: "NameCellID")
    static let StatusCell = NSUserInterfaceItemIdentifier(rawValue: "StatusCellID")
    
  }
    
   
  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {

    var text: String = ""
    var cellIdentifier: NSUserInterfaceItemIdentifier?
    if(row >= testData.count)
    {
        return nil
    }
    let item = testData[row]

    // 2
    if tableColumn == tableView.tableColumns[0] {
      text = item.name
      cellIdentifier = CellIdentifiers.NameCell
    } else if tableColumn == tableView.tableColumns[1] {
        text = item.status
        cellIdentifier = CellIdentifiers.StatusCell
    }
    
    // 3
    if let cell = tableView.makeView(withIdentifier: cellIdentifier!, owner: nil) as? NSTextField {
        cell.stringValue = text
      return cell
    } else {
        let cell = NSTextField.init()
        cell.stringValue = text
        cell.identifier = cellIdentifier
        return cell
    }
  }

}
