// Copyright: 2018, Ableton AG, Berlin. All rights reserved.

import UIKit
import ReactiveSwift
import Result

class ScannerView: UIView {

    let channelCount = CGFloat(40.0)
    
    var scannerObserver : Signal<BTVTXScannerDelegate, NoError>.Observer?
    
    struct RSSIReading {
        var value : Int
    }

    var maxRSSI = 0 {
        didSet {
            setNeedsDisplay()
        }
    }
    
    var outsidePadding = CGFloat(0.05) {
        didSet {
            setNeedsDisplay()
        }
    }
    
    var barSpacing = CGFloat(0.1) {
        didSet {
            setNeedsDisplay()
        }
    }
  
    private var readings: [RSSIReading] = []

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        initialize()
    }

    override init(frame: CGRect) {
        super.init(frame: frame)
        initialize()
    }

    private func initialize() {
        readings = Array(
            repeating: RSSIReading(value: 1),
            count: 40 // we always have 40 channels
        )
        scannerObserver = Signal<BTVTXScannerDelegate, NoError>.Observer(value: {
            $0.latestRSSIReading.observe(
                Signal<BTVTXScannerDelegate.LatestRSSIReading, NoError>.Observer(value: {
                    self.readings[Int($0.channel)].value = Int($0.value)
                    self.setNeedsDisplay()
                })
            )
            self.maxRSSI = Int($0.maxRSSI.value)
            $0.maxRSSI.signal.observe(
                Signal<UInt16, NoError>.Observer(value: { (v : UInt16) -> Void in
                    self.maxRSSI = Int(v)
                })
            );
        } )
    }

    override func draw(_ rect: CGRect) {
        let drawArea = CGRect(
            x: bounds.width * outsidePadding / 2,
            y: bounds.height * outsidePadding / 2,
            width: bounds.width - (bounds.width * outsidePadding),
            height: bounds.height - (bounds.height * outsidePadding)
        )
        let barWidth = (drawArea.width - drawArea.width * barSpacing) / channelCount
        let barStep = drawArea.width * barSpacing / (channelCount - 1.0)
        let heightFactor = drawArea.height / CGFloat(max(1, maxRSSI, readings.map{$0.value}.max()!))
        for (n, reading) in readings.enumerated() {
            let h = CGFloat(reading.value) * heightFactor
            let r = CGRect(
                x: drawArea.origin.x + CGFloat(n) * (barWidth + barStep),
                y: drawArea.origin.y + drawArea.height - h,
                width: barWidth,
                height: h
            )
            let path = UIBezierPath.init(
                rect: r
            )
            path.fill()
        }
    }

}
