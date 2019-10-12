//
//  VTXScannerLaptimerTests.swift
//  VTXScannerLaptimerTests
//
//  Created by Diez B. Roggisch on 05.10.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import XCTest
@testable import VTXScannerLaptimer

class VTXScannerLaptimerTests: XCTestCase {

    override func setUp() {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    func testNewScannerProperty() {
        let sl = VTXScannerList()
        XCTAssertEqual(sl.numberOfScanners.value, 0)
        sl.addScanner("diez", "tired")
        XCTAssertEqual(sl.numberOfScanners.value, 1)
    }

    func testPerformanceExample() {
        // This is an example of a performance test case.
        self.measure {
            // Put the code you want to measure the time of here.
        }
    }

}
