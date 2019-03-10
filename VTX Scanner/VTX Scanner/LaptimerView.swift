// Copyright: 2018, Ableton AG, Berlin. All rights reserved.

import UIKit
import ReactiveSwift
import Result


class LaptimerView: UIView {
    
    var scannerObserver : Signal<BTVTXScannerDelegate, NoError>.Observer?

    private var displayLink : CADisplayLink?
    private var color = UIColor(named: "black")
    
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
            );
        } )
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
    
    }

    override func draw(_ rect: CGRect) {
        let path = UIBezierPath()
        color?.setStroke()
        path.move(to: CGPoint(x: 0.0, y: bounds.height))
        path.addLine(to: CGPoint(x: bounds.width, y: 0))
        path.stroke()
    }
}
