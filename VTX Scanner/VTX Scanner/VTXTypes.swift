//
//  VTXTypes.swift
//  VTX Scanner
//
//  Created by Diez B. Roggisch on 10.03.19.
//  Copyright © 2019 Diez B. Roggisch. All rights reserved.
//

import Foundation


enum app_mode_t {
    case SPLASH_SCREEN
    case SCANNER
    case LAPTIMER
}

struct LatestRSSIReading {
    let channel: UInt16
    let value: UInt16
}
