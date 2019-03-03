// Copyright: 2018, Ableton AG, Berlin. All rights reserved.

import UIKit

class LaptimerView: UIView {

  override func draw(_ rect: CGRect) {
    let path = UIBezierPath()
    path.move(to: CGPoint(x: 0.0, y: bounds.height))
    path.addLine(to: CGPoint(x: bounds.width, y: 0))
    path.stroke()
  }

}
