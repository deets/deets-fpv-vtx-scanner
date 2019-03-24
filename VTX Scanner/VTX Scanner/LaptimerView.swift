// Copyright: 2018, Ableton AG, Berlin. All rights reserved.

import UIKit
import ReactiveSwift
import Result


class LaptimerView: UIView {
    
    var scannerObserver : Signal<BTVTXScannerDelegate, NoError>.Observer?

    private var displayLink : CADisplayLink?
    private var color = UIColor(named: "black")
    
    private var maxRSSI = CGFloat(1400)
    private var points: [CGFloat] = []
    private var writePos: Int=0
    
    var duration = 1.0 {
        didSet {
            if duration < 1.0 {
                duration = 1.0
            }
            initializePointArray()
        }
    }
    
    private func initializePointArray() {
        //let numExtraBufferingPoints = pointsPerSecond() * extraBufferingDuration
        let numPoints = Int(Double(bounds.width) + 1) // + numExtraBufferingPoints)
        if points.count != numPoints {
            points = Array(repeating: 0, count: numPoints)
        }
        writePos = 0
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        initialize()
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        initialize()
    }
    
    private func initialize() {
        scannerObserver = Signal<BTVTXScannerDelegate, NoError>.Observer(value: {
            $0.mode.signal.observe(
                Signal<app_mode_t, NoError>.Observer(value: {
                    print("mode switched", $0)
                    switch $0 {
                    case app_mode_t.LAPTIMER:
                        self.run()
                    default:
                        self.stop()
                    }
                })
            )
            $0.laptimeMessage.observeValues({ (m) in
                self.processLaptimeMessage(m)
            })
            $0.maxRSSI.signal.observeValues({ (maxRSSI) in
                self.maxRSSI = CGFloat(maxRSSI)
                self.setNeedsDisplay()

            })
        } )
        initializePointArray()
    }
    
    private func run() {
        displayLink = CADisplayLink(target: self,
                                    selector: #selector(step))
        displayLink!.add(to: .current,
                         forMode: .common)
        color = UIColor.red
        setNeedsDisplay()
    }
    
    private func stop() {
        displayLink?.invalidate()
        displayLink = nil
        color = UIColor.black
        setNeedsDisplay()
    }
    
    @objc private func step() {
//        setNeedsDisplay()
    }
    
    private func processLaptimeMessage(_ message: BTVTXScannerDelegate.LaptimeMessage) {
        message.values.forEach({ (v) in
            let v = CGFloat(v) * 16 // remove right-shift
            points[writePos] = v;
            writePos = (writePos + 1 ) % points.count
        })
        setNeedsDisplay();
    }

    override func draw(_ rect: CGRect) {
        let path = UIBezierPath()
        color?.setStroke()
        let step = bounds.width / CGFloat(points.count - 1)
        let hf =  bounds.height / maxRSSI;
        var readPos = writePos
        path.move(to: CGPoint(x: 0.0, y: bounds.height - hf * points[readPos]))
        for x in 1..<points.count {
            readPos = (readPos + 1) % points.count
            let v = points[readPos]
            path.addLine(to: CGPoint(x: CGFloat(x) * step, y: bounds.height - hf * v))
        }
        path.stroke()
    }
}
